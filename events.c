/*
 * events.c
 *
 *  Created on: 2023-05-22
 *      Author: @v-barros
 */
#include "eventloop.h"
#include "server.h"
#define BUFFLEN 1024

void read_data(int fd,void*arg,long time_now,void *event_loop){
    registered_event *aux_ev = (registered_event*) arg;
    char buf[BUFFLEN];
    int len;
    len = read(fd,buf,sizeof(buf));
    struct _eventloop * evloop = (eventloop*) event_loop;


    if(len==0){
        //event_rm(aux_ev,evloop->epollfd);
        close(fd);
        printf("connection closed by client\n");
        return;
    }else if(len<0){
        //event_rm(aux_ev,evloop->epollfd);
        close(fd);
        printf("%s read(), error: %s\n",__func__,strerror(errno));
        return;
    }
     
    int i = formatCommand(buf);
    
   // event_create(evloop,fd,write_data,WRITABLE,NULL);
}

void write_data(int fd,void*arg,long time_now,void *event_loop){
    registered_event *aux_ev = (registered_event*) arg;
    struct _eventloop * evloop = (eventloop*) event_loop;
    if(write(fd,"OK",3)!=3){
       // event_rm(aux_ev,evloop->epollfd);
        printf("failed to send all data\n");
        close(fd); /* failed to send all data at once, close */
        //aux_ev->last_active=time_now;
        return;
    }
   // event_create(evloop,fd,read_data,READABLE,time(NULL));

}