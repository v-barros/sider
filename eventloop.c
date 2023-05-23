/*
 * eventloop.c
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
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <time.h>
#include "server.h"
#include "eventloop.h"
#include "events.h"
#define PORT 8080


// Driver function
/*

static inline void check_timeout(long now,int *checkpos);



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
*/

// set up new listening socket on fd using port
int init_conn(int fd, int port){
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

eventloop * fd init_loop(int epoll_instance_fd,int port){
    epollfd = epoll_create(EVENTS_MAX+1);
    eventloop * ev = (eventloop*) malloc (sizeof(eventloop));
    if(!ev)
        abort();
    
    int socket = init_conn(epoll_instance_fd,port);
    printf("listening socket fd [%d]\n",socket);    
    event_set(ev->events_t[EVENTS_MAX], socket, accept_con, &events_t[EVENTS_MAX],time(NULL));
    event_add(ev->evevents_t[EVENTS_MAX],epoll_instance_fd,EPOLLIN);
    ev->epollfd=ev;
}

void event_set(s_event * ev, int fd, event_handler callback, void * arg,long time_now){
    ev->fd = fd;
    ev->callback = callback;
    ev->events = 0;
    ev->arg = arg;
    ev->status = EVENT_OFF;
    ev->last_active = time_now;    
}

void event_rm(s_event * ev, int fd){
    struct epoll_event e_event = {0, {0}};

    if (ev->status == EVENT_OFF)                                        
        return ;

    e_event.data.ptr = ev;
    ev->status = EVENT_OFF;                                    
    epoll_ctl(fd, EPOLL_CTL_DEL, ev->fd, &e_event);
}

void event_add(s_event *ev, int fd, int event){
    
    struct epoll_event e_event = {0, {0}};
    int op;
    e_event.data.ptr = ev;
    e_event.events = ev->events = event;    //EPOLLIN or EPOLLOUT

    if (ev->status == EVENT_ON) {                                          
        op = EPOLL_CTL_MOD;
    } else {                   
        op = EPOLL_CTL_ADD;
        ev->status = EVENT_ON;
    }
    if (epoll_ctl(fd, op, ev->fd, &e_event) < 0)
    {                   
        printf("%s: epoll_ctl, %s\n", __func__, strerror(errno));
    }
}
