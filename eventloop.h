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
#include "server.h"
#include "connection.h"
#define PORT 8080
#define EVENTS_MAX 1024
#define NONE 0
#define READABLE 1
#define WRITEABLE 2

typedef void event_handler (int,void*,long,void*);
typedef struct fired_event fired_event;
typedef struct registered_event registered_event;
typedef struct _eventloop eventloop;

struct registered_event{
	int fd;                                             //fd used for both socket handling and indexing in registered_events array
    int mask;											//event mask might be WRITEABLE, READABLE or NONE for unmonitored events
    long last_active;									//Record the timestamp of each event
    event_handler *read_event_handler;					//Callback function (accept_con,read_data)
    event_handler *write_event_handler;					//Callback function (write_data)
}; 

/* struct used to pass the minimum information
   required to interact with the epoll instance.
   */
struct fired_event{
    int fd;                                             //fd used for both socket handling and indexing in registered_events array
	int mask;											//event mask might be writeable, readable or none for unmonitored events
};

struct _eventloop{
    int epollfd;                                        //Global epoll file descriptor (returned by epoll create)
    int maxfd;                                          /* highest file descriptor currently registered */
    int setsize;                                        /* max number of file descriptors tracked */
    int socketfd;
    registered_event events_t[EVENTS_MAX + 1];          //Global events table
    struct epoll_event fired_events_t[EVENTS_MAX+1]; 
};

/*event loop API functions*/

eventloop * init_loop(int port);
void runloop(eventloop* event_loop);
void event_create(eventloop *event_loop,int event_fd, event_handler callback,int mask,long time_now);
void event_rm(registered_event * ev, int epfd);
void event_add(int event_fd,eventloop* eventLoop, int mask);

/*event handlers (callback functions)*/

void read_data(int fd,void*arg,long time_now,void* event_loop);
void accept_con(int fd,void*arg,long time_now,void *event_loop);
void write_data(int fd,void*arg,long time_now,void *event_loop);
#endif