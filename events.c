/*
 * events.c
 *
 *  Created on: 2023-05-22
 *      Author: @v-barros
 */
#include "eventloop.h"
#define BUFFLEN 1024
void read_data(int fd,void*arg,long time_now,void *event_loop){
    registered_event *aux_ev = (registered_event*) arg;
    char buf[BUFFLEN];
    int len;
    len = read(fd,buf,sizeof(buf));
    struct _eventloop * evloop = (eventloop*) event_loop;
   // event_rm(aux_ev,evloop->epollfd);
   // printf(" read\n");
    if(len==0){
        event_rm(aux_ev,evloop->epollfd);
        close(fd);
        printf("connection closed by client\n");
        return;
    }else if(len<0){
        event_rm(aux_ev,evloop->epollfd);
        close(fd);
        printf("%s read(), error: %s\n",__func__,strerror(errno));
        return;
    }
    event_create(evloop,fd,write_data,WRITEABLE,time(NULL));
}

void accept_con(int fd,void*arg,long time_now,void *event_loop)
{
    struct sockaddr_in cin;
    socklen_t addr_len = sizeof(cin);
    int cfd, i;
    eventloop * evloop=(eventloop*)event_loop;

    if ((cfd = accept(fd, (struct sockaddr *)&cin,&addr_len)) == -1) {
        printf("%s: accept, %s\n", __func__, strerror(errno));
        return ;
    }
    registered_event aux;
    //finds first available event on events table
   
   /* for (i = 0; i < EVENTS_MAX; i++){ 
        aux = evloop->events_t[i];
        if (aux.mask == NONE)                              
            break;       
    }
    */
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
    event_create(evloop,cfd,read_data,READABLE,time(NULL)); 
}

void write_data(int fd,void*arg,long time_now,void *event_loop){
    registered_event *aux_ev = (registered_event*) arg;
    struct _eventloop * evloop = (eventloop*) event_loop;
   // event_rm(aux_ev,evloop->epollfd);
    if(write(fd,"OK",3)!=3){
        event_rm(aux_ev,evloop->epollfd);
        printf("failed to send all data\n");
        close(fd); /* failed to send all data at once, close */
        aux_ev->last_active=time_now;
        return;
    }
    event_create(evloop,fd,read_data,READABLE,time(NULL));


}