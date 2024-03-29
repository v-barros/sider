/*
 * eventloop.h
 *
 *  Created on: 2023-05-22
 *      Author: @v-barros
 */
#ifndef EVENTLOOP_H_
#define EVENTLOOP_H_
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
#define EVENTS_MAX 1024

/* Events masks, we use it to define wheater  */
#define NONE 0 /* no events registered*/
#define READABLE 1 /* fire when we got a readable event in the fd*/
#define WRITABLE 2 /* fire when we got a writable event in the fd*/

typedef struct registered_event registered_event;
typedef struct _eventloop eventloop;
typedef void event_handler (eventloop *el,int fd,void* clientData,int mask);

struct registered_event{
	int fd;                                             //fd used for both socket handling and indexing in registered_events array
    int mask;											//mask is set using READABLE,WRITABLE and NONE. We me marge READABLE and WRITABLE (using OR '|') when we want to monitor the fd for both ops
    event_handler *read_event_handler;					//Callback function (accept_con,read_data)
    event_handler *write_event_handler;					//Callback function (write_data)
    void * clientData;
}; 

struct _eventloop{
    int epollfd;                                        //Global epoll file descriptor (returned by epoll create)
    int maxfd;                                          //highest file descriptor currently registered
    int setsize;                                        //max number of file descriptors tracked
    int socketfd;                                       //socket file descriptor created for the LISTENING port
    registered_event events_t[EVENTS_MAX + 1];          //Global registered events table
    struct epoll_event fired_events_t[EVENTS_MAX+1];    //aux epoll_event vector to store fired events returned in epoll_wait
};

/*event loop API functions*/

eventloop * init_loop(int port);
void runloop(eventloop* event_loop);
int event_create(eventloop *event_loop,int event_fd, event_handler callback,int mask,void * clientData);
void event_close(eventloop* eventLoop, int event_fd,int mask);
void event_add(eventloop* eventLoop, int event_fd,int mask);

/*event handlers (callback functions)*/

int accept_con(int fd);
#endif