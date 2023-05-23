/*
 * events.h
 *
 *  Created on: 2023-05-22
 *      Author: @v-barros
 */
#ifndef EVENTS_H_
#define EVENTS_H_
#define BUFFLEN 1024
#define EVENTS_MAX 1024 
#include <stdio.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "eventloop.h"
#include "server.h"

enum EVENT_STATUS{
	EVENT_ON = 1,
#define EVENT_ON EVENT_ON
    EVENT_OFF = 0
#define EVENT_OFF EVENT_OFF
};

static void read_data(int fd,void*arg,long time_now,eventloop* ev);
static void accept_con(int fd,void*arg,long time_now,eventloop *ev);
typedef void event_handler (int,void*,long);

typedef struct s_event {
    int fd;												//socket file descriptor
    int events;											// read or write
	int status;											//whether there's a peding event or not
    long last_active;									//Record the timestamp of each event
    event_handler *callback;							//Callback function (accept_con,read_data and write_data)
    void *arg;
}s_event;

#endif