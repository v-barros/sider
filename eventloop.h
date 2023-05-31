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
enum EVENT_STATUS{
	EVENT_ON = 1,
#define EVENT_ON EVENT_ON
    EVENT_OFF = 0
#define EVENT_OFF EVENT_OFF
};
typedef void event_handler (int,void*,long,void*);
typedef struct fired_event fired_event;
typedef struct _eventloop eventloop;

struct fired_event{
    int fd;												//socket file descriptor
    int events;											// read or write
	int status;											//whether there's a peding event or not
    long last_active;									//Record the timestamp of each event
    event_handler *callback;							//Callback function (accept_con,read_data and write_data)
    void *arg;
};

struct _eventloop{
    int epollfd;                                        //Global epoll file descriptor (returned by epoll create)
    fired_event events_t[EVENTS_MAX + 1];               //Global events table
};


/*event loop API functions*/
eventloop * init_loop(int port);
void runloop(eventloop* event_loop);
void event_set(fired_event * ev, int fd, event_handler callback, void * arg,long time_now);
void event_rm(fired_event * ev, int epfd);
void event_add(fired_event *ev, int epfd, int event);


/*event handlers (callback functions)*/

void read_data(int fd,void*arg,long time_now,void* event_loop);
void accept_con(int fd,void*arg,long time_now,void *event_loop);
void write_data(int fd,void*arg,long time_now,void *event_loop);
#endif