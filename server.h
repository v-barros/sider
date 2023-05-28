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
#define MAX 1024
#define PORT 8080

typedef struct server_resp server_resp;
typedef struct server_str server_str;
typedef struct shared_resp shared_resp;

/*Global server*/
extern struct server_str server;

/* Global shared responses to build server responses*/
extern struct shared_resp shared;

struct shared_resp{
    String *ok,*crlf,*plus;
};

struct server_resp {
    char * text; 
    int len;
};

struct server_str {
    int port;
    struct _eventloop* ev;
    server_resp *reply;
};

void serverConfInit();

void createSharedResps();

server_resp * create_resp();

#endif // SERVER_H_