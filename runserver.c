/*
 * runserver.c
 *
 *  Created on: 2021-11-21
 *      Author: @v-barros
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <string.h>
#include <assert.h>
#include "server.h"
#include "eventloop.h"
#include "events.h"
#define PORT 8080

// Driver function
static inline void check_timeout(long now,int *checkpos);
int epollfd;
s_event events_t[EVENTS_MAX + 1];


int main()
{
    int port = PORT;
    epollfd = epoll_create(EVENTS_MAX+1);          
    assert (epollfd> 0);
    printf("epoll file descriptor [%d]\n ",epollfd);
    init_loop(epollfd, port);
    struct epoll_event events[EVENTS_MAX+1];             
    printf("server running!\n using port [%d]\n", port);

    int checkpos = 0, i;
    long now;
    while (1) {
        now=time(NULL);
       
        check_timeout(now,&checkpos);

        int number_of_events = epoll_wait(epollfd, events, EVENTS_MAX+1, 20);
        if (number_of_events < 0) {
            printf("epoll_wait error, exit\n");
            break;
        }
        
        for (i = 0; i < number_of_events; i++) {
            s_event *ev = (s_event*)events[i].data.ptr;  
            if ((events[i].events & EPOLLIN) && (ev->events & EPOLLIN))
                ev->callback(ev->fd, ev->arg,now);
        }
    }
    return 0;
}

static inline void check_timeout(long now,int *checkpos){
    int i;
    int ck = *checkpos;
    for (i = 0; i < 50; i++, ck++) {
            ck%=EVENTS_MAX;
            if (events_t[ck].status == EVENT_OFF){
                continue;
            }        
            long duration = now - events_t[ck].last_active;       
            if (duration >= 30L) {
                close(events_t[ck].fd);                           
                printf("[fd=%d] timeout\n", events_t[ck].fd);
                event_rm(&events_t[ck],epollfd);                   
            }
        
    }
    *checkpos=ck;
}

// int main()
// {
//     int sockfd, connfd, len;
//     struct sockaddr_in servaddr, cli;
   
//     // socket create and verification
//     sockfd = socket(AF_INET, SOCK_STREAM, 0);
//     if (sockfd == -1) {
//         printf("socket creation failed...\n");
//         exit(0);
//     }
//     else
//         printf("Socket successfully created..\n");
//     memset(&servaddr,0,sizeof(servaddr));
   
//     // assign IP, PORT
//     servaddr.sin_family = AF_INET;
//     servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
//     servaddr.sin_port = htons(PORT);
   
//     // Binding newly created socket to given IP and verification
//     if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
//         printf("socket bind failed...\n");
//         exit(0);
//     }
//     else
//         printf("Socket successfully binded..\n");
   
//     // Now server is ready to listen and verification
//     if ((listen(sockfd, 5)) != 0) {
//         printf("Listen failed...\n");
//         exit(0);
//     }
//     else
//         printf("Server listening..\n");
//     len = sizeof(cli);
   
//     // Accept the data packet from client and verification
//     connfd = accept(sockfd, (SA*)&cli, &len);
//     if (connfd < 0) {
//         printf("server accept failed...\n");
//         exit(0);
//     }
//     else
//         printf("server accept the client...\n");
//     int num;
//    // scanf("%d",&num);
//     // Function for chatting between client and server
//     run(connfd);
   
//     // After chatting close the socket
//     close(connfd);
//    // scanf("%d",&num);
//     close(sockfd);
// }