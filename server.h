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
#include "eventloop.h"
#include "string.h"

#define PORT 1234
#define OUTPUT_BUF_LEN 16*1024
#define OBJ_SHARED_BULKHDR_LEN 32
#define MAX_KEYS_BUFFER 256

typedef struct server_resp server_resp;
typedef struct server_str server_str;
typedef struct shared_resp shared_resp;

/* A result structure for the various getkeys function calls. It lists the
 * keys as indices to the provided argv.
 */
typedef struct {
    int keysbuf[MAX_KEYS_BUFFER];       /* Pre-allocated buffer, to save heap allocations */
    int *keys;                          /* Key indices array, points to keysbuf or heap */
    int numkeys;                        /* Number of key indices return */
    int size;                           /* Available array size */
} getKeysResult;

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
    struct _eventloop* ev;
    server_resp *reply;
};

void serverConfInit();

void createSharedResps();

server_resp * create_resp();

int formatCommand(char * s);

#endif // SERVER_H_