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
#define MAX 1024
#define PORT 8080
#define SA struct sockaddr
   
typedef struct serverReply {
    char * text; 
    int len;
} serverReply;

typedef struct serverstr {
    int port;
    eventloop ev;
    serverReply reply;
} serverstr;

void putError(serverReply *);

void putOk(serverReply*);

void putText(serverReply*, const char * text);

// Function designed for chat between client and server.
void run(int sockfd);

#endif // SERVER_H_