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

static inline int callHandler(connection *conn, ConnectionCallbackFunc handler) {
    if (handler) 
        handler(conn);
    return C_OK;
}

static int connSocketAccept(connection *conn, ConnectionCallbackFunc accept_handler) {
    int ret = C_OK;

    if (conn->state != CONN_STATE_ACCEPTING) return C_ERR;
    conn->state = CONN_STATE_CONNECTED;

    if (!callHandler(conn, accept_handler)) ret = C_ERR;

    return ret;
}


ConnectionType CT_Socket = {
    .events_handler = connSocketEventHandler,
    .close = connSocketClose,
    .write = connSocketWrite,
    .read = connSocketRead,
    .accept = connSocketAccept
  /*.connect = connSocketConnect,
    .set_write_handler = connSocketSetWriteHandler,
    .set_read_handler = connSocketSetReadHandler,
    .get_last_error = connSocketGetLastError,
    .blocking_connect = connSocketBlockingConnect,
    .sync_write = connSocketSyncWrite,
    .sync_read = connSocketSyncRead,
    .sync_readline = connSocketSyncReadLine,
    .get_type = connSocketGetType
*/
};

connection *connCreateSocket() {
    connection *conn = malloc(sizeof(connection));
    conn->type = &CT_Socket;
    conn->fd = -1;

    return conn;
}

int connGetSocketError(connection *conn) {
    int sockerr = 0;
    socklen_t errlen = sizeof(sockerr);

    if (getsockopt(conn->fd, SOL_SOCKET, SO_ERROR, &sockerr, &errlen) == -1)
        sockerr = errno;
    return sockerr;
}

static void connSocketEventHandler(struct _eventloop *el, int fd, void *clientData, int mask)
{
    connection *conn = clientData;

    if (conn->state == CONN_STATE_CONNECTING &&
            (mask & WRITABLE) && conn->conn_handler) {

        int conn_error = connGetSocketError(conn);
        if (conn_error) {
            conn->last_errno = conn_error;
            conn->state = CONN_STATE_ERROR;
        } else {
            conn->state = CONN_STATE_CONNECTED;
        }

        if (!conn->write_handler){}
         //event_rm(server.ev,conn->fd);

        if (!callHandler(conn, conn->conn_handler)) return;
        conn->conn_handler = NULL;
    }

    int call_write = (mask & WRITABLE) && conn->write_handler;
    int call_read = (mask & READABLE) && conn->read_handler;

    /* Fire the readable event. */
    if (call_read) {
        if (callHandler(conn, conn->read_handler)) return;
    }
    /* Fire the writable event. */
    if (call_write) {
        if (callHandler(conn, conn->write_handler)) return;
    }

}

/* Close the connection and free resources. */
static void connSocketClose(connection *conn) {
    if (conn->fd != -1) {
       // event_rm(server.el,conn->fd);
        close(conn->fd);
        conn->fd = -1;
    }

    free(conn);
}

static int connSocketWrite(connection *conn, const void *data, size_t data_len) {
    int ret = write(conn->fd, data, data_len);
    if (ret < 0 && errno != EAGAIN) {
        conn->last_errno = errno;

        /* Don't overwrite the state of a connection that is not already
         * connected, not to mess with handler callbacks.
         */
        if (errno != EINTR && conn->state == CONN_STATE_CONNECTED)
            conn->state = CONN_STATE_ERROR;
    }

    return ret;
}

static int connSocketRead(connection *conn, void *buf, size_t buf_len) {
    int ret = read(conn->fd, buf, buf_len);
    if (!ret) {
        conn->state = CONN_STATE_CLOSED;
    } else if (ret < 0 && errno != EAGAIN) {
        conn->last_errno = errno;

        /* Don't overwrite the state of a connection that is not already
         * connected, not to mess with handler callbacks.
         */
        if (errno != EINTR && conn->state == CONN_STATE_CONNECTED)
            conn->state = CONN_STATE_ERROR;
    }

    return ret;
}

int init_conn(int port){

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    assert(sockfd !=-1);
    // set socket to NON BLOCKING
    fcntl(sockfd, F_SETFL, O_NONBLOCK);
    
    assert((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) == 0);
   
    assert((listen(sockfd, 20)) == 0);

    return sockfd;
        
}