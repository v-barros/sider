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

client *createClient(connection *conn) {
    client *c = malloc(sizeof(client));

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