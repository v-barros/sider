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

void clientAcceptHandler(connection *conn) {
    client *c = connGetPrivateData(conn);

    if (connGetState(conn) != CONN_STATE_CONNECTED) {
        freeClient(c);
        return;
    }
    printf("%d - %s\n",__LINE__,__func__);
    server.stat_numconnections++;
}

void readQueryFromClient(connection *conn) {
    client *c = connGetPrivateData(conn);
    char buf[1024];
    int nread,readlen=1024;

    printf("%d - %s\n",__LINE__,__func__);
    
    //nread = connRead(c->conn, buf, readlen);
    if (nread == -1) {
        if (connGetState(conn) == CONN_STATE_CONNECTED) {
            return;
        } else {
            printf("%s, error: %s\n",__func__,strerror(errno));
        }
    } else if (nread == 0) {
        printf("connection closed by client, %s, error: %s\n",__func__,strerror(errno));
        freeClient(c);
    }

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
    if(!c){
        return NULL;
    }
    if (conn) {
        connSetReadHandler(conn, readQueryFromClient);
        connSetPrivateData(conn, c);
    } 

    c->argc = 0;
    c->argv_len = 0;
    c->id = ++server.next_client_id;
    c->conn = conn;
    c->argv = NULL;
    c->argv_len_sum = 0;
    c->cmd = NULL;
    c->bulklen = -1;
    c->sentlen = 0;
    c->ctime = c->lastinteraction = server.unixtime;
    c->flags = 0;
    c->bufpos = 0;

    printf("client created, id = %ld\n",c->id);
    return c;
}

void freeClient(client *c) {
   return;
}

static void acceptCommonHandler(connection *conn, int flags) {
    client *c;
    char conninfo[100];
    if (connGetState(conn) != CONN_STATE_ACCEPTING) {
        connClose(conn);
        return;
    }
    printf("%d - %s\n",__LINE__,__func__);
    /* Create connection and client */
    if ((c = createClient(conn)) == NULL) {
        connClose(conn); /* May be already closed, just ignore errors */
        printf("client not created\n");
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
    printf("%d - %s\n",__LINE__,__func__);
}