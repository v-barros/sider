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
#define MAX 80
#define PORT 8080
#define SA struct sockaddr
   
int getFun(char * args);

int setFun(char * args);

// Function designed for chat between client and server.
void run(int sockfd);

#endif // SERVER_H_