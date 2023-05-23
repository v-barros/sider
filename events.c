/*
 * events.c
 *
 *  Created on: 2023-05-22
 *      Author: @v-barros
 */
#include "events.h"

static void read_data(int fd,void*arg,long time_now,eventloop *ev){
    s_event *aux_ev = (s_event*) arg;
    char buf[BUFFLEN];
    int len;
    len = read(fd,buf,sizeof(buf));

    if(len==0){
        event_rm(aux_ev,epollfd);
        close(fd);
        printf("connection closed by client\n");
        return;
    }else if(len<0){
        event_rm(aux_ev,epollfd);
        close(fd);
        printf("%s read(), error: %s\n",__func__,strerror(errno));
        return;
    }
    
    //At this point, the read data would be processed

    if(write(fd,"OK",3)!=3){
        event_rm(aux_ev,epollfd);
        printf("failed to send all data\n");
	    close(fd); /* failed to send all data at once, close */
        return;
    }
    ev->last_active=time_now;
}

static void accept_con(int fd,void*arg,long time_now,eventloop *ev)
{
    struct sockaddr_in cin;
    socklen_t addr_len = sizeof(cin);
    int cfd, i;

    if ((cfd = accept(fd, (struct sockaddr *)&cin,&addr_len)) == -1) {
        printf("%s: accept, %s\n", __func__, strerror(errno));
        return ;
    }
    //finds first available event on events table
    for (i = 0; i < EVENTS_MAX; i++) 
        if (ev->events_t[i].status == EVENT_OFF)                              
            break;       

    if (i == EVENTS_MAX) {
        printf("%s: connection overflow [%d]\n", __func__, EVENTS_MAX);
        return;
    }
    int flag = 0;
    
    // set socket to NON BLOCKING
    if ((flag = fcntl(cfd, F_SETFL, O_NONBLOCK)) < 0) {
        printf("%s: fcntl(), %s\n", __func__, strerror(errno));
        return;
    }

    // registers read_data() as the event_handler callback function 
    event_set(&ev->events_t[i],cfd,read_data,&ev->events_t[i],time_now);  

    // adds the EPOLLIN event as the interested event for the newly createad connection (events_t[i]->fd)
    event_add(&ev->events_t[i],ev->epollfd, EPOLLIN);    
}
