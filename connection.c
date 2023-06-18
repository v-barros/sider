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
  /*.events_handler = connSocketEventHandler,
    .close = connSocketClose,
    .write = connSocketWrite,
    .read = connSocketRead,
  */.accept = connSocketAccept
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