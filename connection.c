/*
 * connection.c
 *
 *  Created on: 2023-05-23
 *      Author: @v-barros
 */
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <assert.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stddef.h>
#include "server.h"
#include "connection.h"

static inline int callHandler(connection *conn, ConnectionCallbackFunc handler)
{
    if (handler){
        handler(conn);
        //printf("%d - %s\n",__LINE__,__func__);
        return C_OK;
    }
    else{
        //printf("%d - %s\n",__LINE__,__func__);
        return C_ERR;
    }
        
    
}

static int connSocketAccept(connection *conn, ConnectionCallbackFunc accept_handler)
{
    int ret = C_OK;

    if (conn->state != CONN_STATE_ACCEPTING)
        return C_ERR;
    conn->state = CONN_STATE_CONNECTED;
   // printf("%d - %s\n",__LINE__,__func__);
    return callHandler(conn, accept_handler);
}

ConnectionType CT_Socket = {
    .events_handler = connSocketEventHandler,
    .close = connSocketClose,
    .write = connSocketWrite,
    .read = connSocketRead,
    .accept = connSocketAccept,
    .set_write_handler = connSocketSetWriteHandler,
    .set_read_handler = connSocketSetReadHandler
};

connection *connCreateSocket()
{
    connection *conn = calloc(sizeof(connection),1);
    conn->type = &CT_Socket;
    conn->fd = -1;

    return conn;
}

connection *connCreateAcceptedSocket(int fd)
{
    connection *conn = connCreateSocket();
    conn->fd = fd;
    conn->state = CONN_STATE_ACCEPTING;
    return conn;
}

int connGetSocketError(connection *conn)
{
    int sockerr = 0;
    socklen_t errlen = sizeof(sockerr);
    printf("%d - %s\n", __LINE__,__func__);
    if (getsockopt(conn->fd, SOL_SOCKET, SO_ERROR, &sockerr, &errlen) == -1)
        sockerr = errno;
    return sockerr;
}

/*  This function is meant to handle the events at registered_event structs.
    We assign it to registered_event->read_event_handler and registered_event->write_event_handler
    and when an event is triggered, eventloop will call this function to handle the event

    Then, we process the event(read or write) using the connection-specific functions

    In this case, I'm using only TCP connections, but the real Redis uses this layer of abstraction
    to handle different types of sockets (Unix, TCP and TLS so far)

*/
static void connSocketEventHandler(struct _eventloop *el, int fd, void *clientData, int mask)
{
    connection *conn = clientData;
   // printf("%d - %s\n", __LINE__,__func__);
    if (conn->state == CONN_STATE_CONNECTING &&
        (mask & WRITABLE) && conn->conn_handler)
    {

        int conn_error = connGetSocketError(conn);
        if (conn_error)
        {
            conn->last_errno = conn_error;
            conn->state = CONN_STATE_ERROR;
        }
        else
        {
            conn->state = CONN_STATE_CONNECTED;
        }

        if (!conn->write_handler)
        {
        }
        // event_rm(server.ev,conn->fd);

        if (!callHandler(conn, conn->conn_handler))
            return;
        conn->conn_handler = NULL;
    }
  //  printf("%d - %s\n", __LINE__,__func__);
    int call_write = (mask & WRITABLE) && conn->write_handler;
    int call_read = (mask & READABLE) && conn->read_handler;

    /* Fire the readable event. */
    if (call_read)
    {
      //  printf("%d - %s\n", __LINE__,__func__);
        if (callHandler(conn, conn->read_handler)==C_ERR)
            return;
    }
   // printf("%d - %s\n", __LINE__,__func__);
    /* Fire the writable event. */
    if (call_write)
    {
      //  printf("%d - %s\n", __LINE__,__func__);
        if (callHandler(conn, conn->write_handler)==C_ERR)
            return;
    }
}

/* Close the connection and free resources. */
static void connSocketClose(connection *conn)
{
    if (conn->fd != -1)
    {
        event_close(server.el,conn->fd,READABLE|WRITABLE);
        close(conn->fd);
        conn->fd = -1;
    }

    free(conn);
}

static int connSocketWrite(connection *conn, const void *data, size_t data_len)
{
    int ret = write(conn->fd, data, data_len);
    if (ret < 0 && errno != EAGAIN)
    {
        conn->last_errno = errno;

        /* Don't overwrite the state of a connection that is not already
         * connected, not to mess with handler callbacks.
         */
        if (errno != EINTR && conn->state == CONN_STATE_CONNECTED)
            conn->state = CONN_STATE_ERROR;
    }

    return ret;
}

static int connSocketRead(connection *conn, void *buf, size_t buf_len)
{
    int ret = read(conn->fd, buf, buf_len);
    if (!ret)
    {
        conn->state = CONN_STATE_CLOSED;
    }
    else if (ret < 0 && errno != EAGAIN)
    {
        conn->last_errno = errno;

        /* Don't overwrite the state of a connection that is not already
         * connected, not to mess with handler callbacks.
         */
        if (errno != EINTR && conn->state == CONN_STATE_CONNECTED)
            conn->state = CONN_STATE_ERROR;
    }

    return ret;
}

int connGetState(connection *conn)
{
    return conn->state;
}

/* Get the associated private data pointer */
void *connGetPrivateData(connection *conn)
{
    return conn->private_data;
}

void connSetPrivateData(connection *conn,void* privateData){
    if(conn)
        conn->private_data = privateData;
}

/* Register a write handler, to be called when the connection is writable.
 * If NULL, the existing handler is removed.
 *
 */
static int connSocketSetWriteHandler(connection *conn, ConnectionCallbackFunc func){
    if (func == conn->write_handler) return C_OK;
    conn->write_handler = func;
    if (!conn->write_handler)
        event_close(server.el,conn->fd,WRITABLE);
    else
        if (event_create(server.el,conn->fd,
                            conn->type->events_handler,
                                WRITABLE,conn) == C_ERR) return C_ERR;
    return C_OK;
}

/* Register a read handler, to be called when the connection is readable.
 * If NULL, the existing handler is removed.
 */
static int connSocketSetReadHandler(connection *conn, ConnectionCallbackFunc func)
{
   // printf("%d - %s\n", __LINE__,__func__);
    if (func == conn->read_handler)
        return C_OK;
 //   printf("%d - %s\n", __LINE__,__func__);
    conn->read_handler = func;
    if (!conn->read_handler)
    {
   //     printf("%d - %s\n", __LINE__,__func__);
    }
    // event_rm(server.el,conn->fd);
    else if (event_create(server.el, conn->fd, conn->type->events_handler,
                          READABLE, conn) == C_ERR){
    //        printf("%d - %s\n", __LINE__,__func__);
            return C_ERR;
    }
  //  printf("%d - %s\n", __LINE__,__func__);
    return C_OK;
}

int init_conn(int port)
{

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    assert(sockfd != -1);
    // set socket to NON BLOCKING
    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    assert((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) == 0);

    assert((listen(sockfd, 20)) == 0);

    return sockfd;
}