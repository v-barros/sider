/*
 * server.h
 *
 *  Created on: 2021-11-21
 *      Author: @v-barros
 */
#ifndef SERVER_H_
#define SERVER_H_

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "ht.h"
#include "eventloop.h"
#include "string.h"
#include "connection.h"
#define PORT 1234

#define OUTPUT_BUF_LEN 1024
#define OBJ_SHARED_BULKHDR_LEN 32
#define MAX_KEYS_BUFFER 256
#define REPLY_CHUNK_BYTES 1024

#define COMMANDS 2

#define C_OK 0
#define C_ERR -1

#define min(a, b) ((a) < (b) ? (a) : (b))

#define CLIENT_PENDING_COMMAND (1<<4)

typedef struct server_resp server_resp;
typedef struct server_str server_str;
typedef struct shared_resp shared_resp;
typedef struct client client;

typedef struct siderCommand siderCommand;
typedef void command_proc(client *);

void getCommand(client*);

void setCommand(client*);

/* A result structure for the various getkeys function calls. It lists the
 * keys as indices to the provided argv.
 */
typedef struct {
    int keysbuf[MAX_KEYS_BUFFER];       /* Pre-allocated buffer, to save heap allocations */
    int *keys;                          /* Key indices array, points to keysbuf or heap */
    int numkeys;                        /* Number of key indices return */
    int size;                           /* Available array size */
} getKeysResult;

typedef int command_get_keys(siderCommand *cmd, char **argv, int argc, getKeysResult *result);

struct client {
    int argc;               /* Num of arguments of current command. */
    int argv_len;           /* Size of argv array (may be more than argc) */
    uint64_t id;            /* Client incremental unique ID. */
    connection *conn;
    char querybuf[1024];    /* Buffer we use to store client queries*/
    char **argv;            /* Arguments of current command. */
    size_t argv_len_sum;    /* Sum of lengths of objects in argv list. */
    struct siderCommand *cmd;  /*command to be executed. */
    int multibulklen;       /* Number of multi bulk arguments left to read. */
    long bulklen;           /* Length of bulk argument in multi bulk request. */
    size_t sentlen;         /* Amount of bytes already sent in the current
                               buffer or object being sent. */
    time_t ctime;           /* Client creation time. */
    time_t lastinteraction; /* Time of the last interaction, used for timeout */
    uint64_t flags;         /* Client flags: CLIENT_* macros. */
   
    /* Response buffer */
    int bufpos;
    /* Note that 'buf' must be the last field of client struct, because memory
     * allocator may give us more memory than our apply for reducing fragments,
     * but we want to make full use of given memory, i.e. we may access the
     * memory after 'buf'. To avoid make others fields corrupt, 'buf' must be
     * the last one. */
    char buf[REPLY_CHUNK_BYTES];
};


struct siderCommand{
    char *name;
    command_proc *proc;
    command_get_keys *getKeys;
};

/*Global server*/
extern struct server_str server;

/* Global shared responses to build server responses*/
extern struct shared_resp shared;

struct shared_resp{
    String *ok,*crlf,*plus,*cone,*czero,
    *bulkhdr[OBJ_SHARED_BULKHDR_LEN],
    *mbulkhdr[OBJ_SHARED_BULKHDR_LEN];
};

struct server_resp {
    int bufpos;
    int buflen;
    char buffer[OUTPUT_BUF_LEN]; 
};

struct server_str {
    int port;
    int stat_numconnections;
    int commandsNum;
    uint64_t next_client_id;
    time_t unixtime;
    struct _eventloop* el;
    server_resp *reply;
    siderCommand *commands;
    Table * ht_table;
};

void serverConfInit();

void createSharedResps();

server_resp * create_resp();

int formatCommand(char * s);

void acceptTcpHandler(eventloop *el, int fd, void *privdata, int mask);
void readQueryFromClient(connection *conn);

/* This function is called every time, in the client structure 'c', there is
 * more query buffer to process, because we read more data from the socket
 * or because a client was blocked and later reactivated, so there could be
 * pending query buffer, already representing a full command, to process.
 * return C_ERR in case the client was freed during the processing */
int processInputBuffer(client *c);

/* If this function gets called we already read a whole
 * command, arguments are in the client argv/argc fields.
 * processCommand() execute the command or prepare the
 * server for a bulk read from the client.
 *
 * If C_OK is returned the client is still alive and valid and
 * other operations can be performed by the caller. Otherwise
 * if C_ERR is returned the client was destroyed (i.e. after QUIT). */
int processCommand(client *c);
#endif // SERVER_H_