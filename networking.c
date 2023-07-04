/*
 * networking.h
 *
 *  Created on: 2023-06-18
 *      Author: @v-barros
 */
#include "server.h"
#include "utils.h"
static void acceptCommonHandler(connection *conn, int flags);
void clientAcceptHandler(connection *conn);
void freeClient(client *c);
client *createClient(connection *conn);
int processMultibulkBuffer (client *c);
int processInputBuffer(client *c);

int unlinkClient(client *c){

    /* Certain operations must be done only if the client has an active connection.
     * If the client was already unlinked or if it's a "fake client" the
     * conn is already set to NULL. */
    if (c->conn) {
        connClose(c->conn);
        c->conn = NULL;
    }


}


/* "*2\r\n$5\r\nhello\r\n$5\r\nworld\r\n" */
int processMultibulkBuffer (client *c){
    char *newline = NULL;
    char *aux=NULL;
    int ok;
    long long ll=0;
    printf("%d - %s\n",__LINE__,__func__);

    if (c->multibulklen == 0) {
        printf("%d - %s\n",__LINE__,__func__);

        /* Multi bulk length cannot be read without a \r\n */
        newline = strchr(c->querybuf,'\r');
        if (newline == NULL) {
            printf("%d - %s\n",__LINE__,__func__);
            return C_ERR;
        }
       
        if (*(newline+1)!='\n') {
            printf("%d - %s\n",__LINE__,__func__);
            return C_ERR;
        }
        newline++;
        /*  pass a len to string2ll() by finding next
            CRLF and calculating the offset from '*' to '\n'
            *2\r\n$5\r\nhello\r\n$5\r\nworld\r\n
        */
        ok = string2ll(c->querybuf+1,newline-2-(c->querybuf),&ll);
        
        c->multibulklen = ll;
       
     //   printf("\nmultibulk c->querybuf:[%p]\n",c->querybuf);
     //   printf("\nmultibulk newline: [%p]\n",newline);
     //   printf("\nmultibulk ll:[%lld]\n",ll);
        
        /* Setup argv array on client structure */
        if (c->argv) free(c->argv);
        c->argv_len = min(c->multibulklen, 1024);
        c->argv = malloc(sizeof(char*)*c->argv_len);
        c->argc=0;
    }

    while(c->multibulklen){
        // 8 9 \A \B C D \E \F 1 2 3 4 5 \6 \7 8 9 \A \B C D E F 1 \2 \3
        // 2 * \r \n $ 5 \r \n h e l l o \r \n $ 5 \r \n w o r l d \r \n
        // gotta loop 2 times, extracting the len and
        //0 putting the arguments at c->argv

       
        /* Read bulk length if unknown */
        if (c->bulklen == -1) {
            newline = strchr(newline+1,'$');
            if (newline == NULL) {
                    printf("%d - %s\n",__LINE__,__func__);
                    break;
            }
            /* Multi bulk length cannot be read without a \r\n */
            aux=newline;
            newline = strchr(newline,'\r');
            if (newline == NULL) {
                printf("%d - %s\n",__LINE__,__func__);
                return C_ERR;
            }
        
            if (*(newline+1)!='\n') {
                printf("%d - %s\n",__LINE__,__func__);
                return C_ERR;
            }
            newline++;
            /*  pass a len to string2ll by finding next
                CRLF and calculating the offset
                *2\r\n$5\r\nhello\r\n$5\r\nworld\r\n
            */
            ok = string2ll(aux+1,newline-2-(aux),&ll);
            if(ok){
                c->bulklen=ll;
            }else{
                free(c->argv);
                return C_ERR;
            }
        }
        //Read bulk data
        /*  check wether there's a '\r\n' after c->bulklen bytes.
            in other words, checks for the actual size of the argument
        */
        if(*(newline+(c->bulklen)+1)!='\r' &&
           *(newline+(c->bulklen)+2)!='\n'){
            printf("%d - %s\n",__LINE__,__func__);
            return C_ERR;
        }

        /* Allocate memory to store the argument at c->argv*/
        c->argv[c->argc]=malloc(sizeof(char)*c->bulklen+1);
        
        if(!c->argv[c->argc]) abort();
        memcpy(c->argv[c->argc],newline+1,c->bulklen);
        c->argv[c->argc][c->bulklen]='\0';

       // printf("arg at pos[%d] : '%s'\n",c->argc,c->argv[c->argc]);
        //printf("\nmultibulk ll:[%lld]\n",ll);
        //printf("\nmultibulk newline: [%s]\n",newline);
        
        c->multibulklen--; // 1 argument less left to read
        c->bulklen=-1; // next argument's length is unknown
        c->argc++;  // 1 argument more that we read, use this for indexing at argv
    }

    return C_OK;
}
int processInputBuffer(client *c){

    /* Don't process more buffers from clients that have already pending
        * commands to execute in c->argv. */
    if (c->flags & CLIENT_PENDING_COMMAND) return C_ERR;
       
    if (!c->querybuf == '*') return C_ERR;
    if (processMultibulkBuffer(c) != C_OK) return C_ERR;
    if (processCommand(c) == C_ERR) return C_ERR;
          
    return C_OK;
}

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
    
    nread = connRead(c->conn, c->querybuf, readlen);
    if (nread == -1) {
        if (connGetState(conn) == CONN_STATE_CONNECTED) {
            return;
        } else {
            printf("%s, error: %s\n",__func__,strerror(errno));
        }
    } else if (nread == 0) {
        printf("connection closed by client, %s, error: %s\n",__func__,strerror(errno));
        freeClient(c);
        return;
    }
    processInputBuffer(c);
  //  printf("query from client: %s\n",c->querybuf);

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
    c->multibulklen = 0;
    c->bulklen = -1;
    c->sentlen = 0;
    c->ctime = c->lastinteraction = server.unixtime;
    c->flags = 0;
    c->bufpos = 0;

    printf("client created, id = %ld\n",c->id);
    return c;
}

void freeClient(client *c) {

    free(c->argv);

    /* Unlink the client: this will close the socket, remove the I/O
     * handlers, and remove references of the client from different
     * places where active clients may be referenced. */
    unlinkClient(c);

    free(c);
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