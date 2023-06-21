/*
 * networking.h
 *
 *  Created on: 2023-06-18
 *      Author: @v-barros
 */
#include "server.h"
static void acceptCommonHandler(connection *conn, int flags);
void clientAcceptHandler(connection *conn);
void freeClient(client *c);
client *createClient(connection *conn);

void readQueryFromClient(connection *conn) {
    client *c = connGetPrivateData(conn);
}

int accept_con(int fd)
{
    struct sockaddr_in cin;
    socklen_t addr_len = sizeof(cin);
    int cfd;
  
    if ((cfd = accept(fd, (struct sockaddr *)&cin,&addr_len)) == -1) {
        printf("%s: accept, %s\n", __func__, strerror(errno));
        return C_ERR;
    }

    // set socket to NON BLOCKING
    if ((fcntl(cfd, F_SETFL, O_NONBLOCK)) < 0) {
        printf("%s: fcntl(), %s\n", __func__, strerror(errno));
        return C_ERR;
    }
    printf("new connection at fd %d\n",cfd);
    return cfd;
}

    
void acceptTcpHandler(eventloop *el, int fd, void *privdata, int mask) {
    int cport, cfd;

    cfd = accept_con(fd);
    
    if (cfd==C_ERR){
        printf("%s accept_con(), error: %s\n",__func__,strerror(errno));
    }
    acceptCommonHandler(connCreateAcceptedSocket(cfd),0);
}

client *createClient(connection *conn) {
    client *c = malloc(sizeof(client));
    connSetReadHandler(conn, readQueryFromClient);
    return c;
}

void freeClient(client *c) {
   return;
}

void clientAcceptHandler(connection *conn) {
    client *c = connGetPrivateData(conn);

    if (connGetState(conn) != CONN_STATE_CONNECTED) {
        freeClient(c);
        return;
    }

    server.stat_numconnections++;
}

static void acceptCommonHandler(connection *conn, int flags) {
    client *c;
    char conninfo[100];

    if (connGetState(conn) != CONN_STATE_ACCEPTING) {
        connClose(conn);
        return;
    }

    /* Create connection and client */
    if ((c = createClient(conn)) == NULL) {
        connClose(conn); /* May be already closed, just ignore errors */
        return;
    }

    /* Last chance to keep flags */
    c->flags |= flags;

    /* Initiate accept.
     *
     * Note that connAccept() is free to do two things here:
     * 1. Call clientAcceptHandler() immediately;
     * 2. Schedule a future call to clientAcceptHandler().
     *
     * Because of that, we must do nothing else afterwards.
     */
    if (connAccept(conn, clientAcceptHandler) == C_ERR) {
        char conninfo[100];
        if (connGetState(conn) == CONN_STATE_ERROR)
            freeClient(connGetPrivateData(conn));
        return;
    }
}