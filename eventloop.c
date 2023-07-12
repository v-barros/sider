/*
 * eventloop.h
 *
 *  Created on: 2023-05-22
 *      Author: @v-barros
 */
#include "eventloop.h"
#include "connection.h"
#include "server.h"

#define TIMEOUT 5

static inline void check_timeout(long now,int *checkpos,eventloop* event_loop);
void set_mask(registered_event* ev,int mask);
void event_delete(eventloop* eventLoop, int event_fd,int delmask);

eventloop * init_loop(int port){
    int epollfd = epoll_create(EVENTS_MAX+1);
    eventloop * ev = (eventloop*) calloc (1,sizeof(eventloop));
    if(!ev)
        abort();
    ev->epollfd=epollfd;
    int socket = init_conn(port);
    printf("listening socket fd [%d]\n",socket);   

    ev->socketfd=socket;
    ev->setsize=EVENTS_MAX;
    
    int i;
    for(i=0;i<EVENTS_MAX;i++){
        set_mask(&ev->events_t[i],NONE);
    }
    /* Create an event handler for accepting new connections in TCP sockets */
    event_create(ev,socket,acceptTcpHandler,READABLE,NULL);
    
    
    return ev;
}

void set_mask(registered_event* ev,int mask){
    ev->mask=mask;
}

int event_create(eventloop *event_loop,int event_fd, event_handler callback,int mask,void * clientData){
    registered_event *re = &event_loop->events_t[event_fd];
    re->fd=event_fd;
  //  printf("%d - %s\n", __LINE__,__func__);
    if (event_fd >= event_loop->setsize) {
        return C_ERR;
    }
  //  printf("%d - %s\n", __LINE__,__func__);
    event_add(event_loop,re->fd,mask);

    re->mask |=mask;
    if(mask & READABLE) 
        re->read_event_handler=callback;
    if(mask & WRITABLE) 
        re->write_event_handler=callback;
  
    re->clientData = clientData;

    if (event_fd > event_loop->maxfd)
        event_loop->maxfd=event_fd;
    
    return C_OK;
}

void event_close(eventloop* eventLoop, int event_fd,int mask){
    if (event_fd >= eventLoop->setsize) return;
    registered_event *fe = &eventLoop->events_t[event_fd];
    if (fe->mask == NONE) return;

    event_delete(eventLoop, event_fd, mask);
    
    fe->mask = fe->mask & (~mask);
    if (event_fd == eventLoop->maxfd && fe->mask == NONE) {
        /* Update the max fd */
        int j;

        for (j = eventLoop->maxfd-1; j >= 0; j--)
            if (eventLoop->events_t[j].mask != NONE) break;
        eventLoop->maxfd = j;
    }

}

void event_delete(eventloop* eventLoop, int event_fd,int delmask){
    struct epoll_event ee = {0};
    int mask = eventLoop->events_t[event_fd].mask & (~delmask);

    ee.events = 0;
    if (mask & READABLE) ee.events |= EPOLLIN;
    if (mask & WRITABLE) ee.events |= EPOLLOUT;
    ee.data.fd = event_fd;
    if (mask != NONE) {
        epoll_ctl(eventLoop->epollfd,EPOLL_CTL_MOD,event_fd,&ee);
   //      printf("event RM OK [fd=%d], op=%d, events[%0X]\n", ee.data.fd, EPOLL_CTL_MOD, ee.events);
    } else {

        epoll_ctl(eventLoop->epollfd,EPOLL_CTL_DEL,event_fd,&ee);
    //    printf("event RM OK [fd=%d], op=%d, events[%0X]\n", ee.data.fd, EPOLL_CTL_DEL, ee.events);
    }  

}

void event_add(eventloop* eventLoop, int event_fd,int mask){
    struct epoll_event e_event = {0, {0}};
    int op = eventLoop->events_t[event_fd].mask == NONE ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
   
    e_event.events = 0;

    mask |= eventLoop->events_t[event_fd].mask; /*merge old events*/

    if(mask & WRITABLE)
        e_event.events |= EPOLLOUT;    //EPOLLIN or EPOLLOUT
    if(mask & READABLE)
        e_event.events |= EPOLLIN;    //EPOLLIN or EPOLLOUT

    e_event.data.fd=event_fd;
   // printf("event add OK [fd=%d], op=%d, events[%0X]\n", e_event.data.fd, op, e_event.events);

    if (epoll_ctl(eventLoop->epollfd, op, e_event.data.fd, &e_event) < 0)
    {                   
        printf("%s: epoll_ctl, %s\n", __func__, strerror(errno));
    }
}

void runloop(eventloop* event_loop){
           
    int checkpos = 0, i,firedfd,firedmask,mask;
    long now;
    registered_event *rv;
    struct epoll_event *fe;
  

    while (1) {
        //now=time(NULL);
       
        //check_timeout(now,&checkpos,event_loop);

        int number_of_events = epoll_wait(event_loop->epollfd, event_loop->fired_events_t, EVENTS_MAX+1, 20);
        if (number_of_events < 0) {
            printf("epoll_wait error, exit\n");
            break;
        }

        for (i = 0; i < number_of_events; i++) {
            mask = 0;
            fe = &event_loop->fired_events_t[i];
            firedfd = fe->data.fd;
            firedmask = fe->events;

            rv =  &event_loop->events_t[firedfd];

            if (firedmask & EPOLLIN) mask |= READABLE;
            if (firedmask & EPOLLOUT) mask |= WRITABLE;
          //  printf("%d - %s\n",__LINE__,__func__);            
            if(mask & READABLE && rv->mask & READABLE)
                rv->read_event_handler(event_loop,rv->fd,rv->clientData,READABLE);
            if(mask & WRITABLE && rv->mask & WRITABLE)
                rv->write_event_handler(event_loop,rv->fd,rv->clientData,WRITABLE);
            }
    }
}

static inline void check_timeout(long now,int *checkpos,eventloop* event_loop){
    int i,socket_pos;
    int ck = *checkpos;
    int timeout=TIMEOUT;
    socket_pos=event_loop->socketfd;
    for (i = 0; i < 50; i++, ck++) {
            ck%=EVENTS_MAX;
            
            if (event_loop->events_t[ck].mask == NONE || ck==socket_pos){
                continue;
            }
            long duration = now - 1;       
            if (duration >= timeout) {
                close(event_loop->events_t[ck].fd);                           
            //    printf("[fd=%d] timeout\n", event_loop->events_t[ck].fd);
                //event_rm(&event_loop->events_t[ck],event_loop->epollfd);                   
            }
        
    }
    *checkpos=ck;
}