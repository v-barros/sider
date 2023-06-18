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
#include "connection.h"

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