#include "eventloop.h"
#define TIMEOUT 5
static inline void check_timeout(long now,int *checkpos,eventloop* event_loop);
void set_mask(registered_event* ev,int mask);

eventloop * init_loop(int port){
    int epollfd = epoll_create(EVENTS_MAX+1);
    eventloop * ev = (eventloop*) malloc (sizeof(eventloop));
    if(!ev)
        abort();
    ev->epollfd=epollfd;
    int socket = init_conn(port);
    printf("listening socket fd [%d]\n",socket);   
    int i;
    for(i=0;i<EVENTS_MAX;i++){
        set_mask(&ev->events_t[i],NONE);
    }
    event_create(ev,socket,accept_con,READABLE,time(NULL));
    ev->socketfd=socket;
    ev->setsize=EVENTS_MAX;
    return ev;
}

void set_mask(registered_event* ev,int mask){
    ev->mask=mask;
}

void event_create(eventloop *event_loop,int event_fd, event_handler callback,int mask,long time_now){
    registered_event *ev = &event_loop->events_t[event_fd];
    ev->fd=event_fd;
    ev->last_active=time_now;
    
    event_add(ev->fd,event_loop,mask);
    ev->mask=mask;
    if(mask & READABLE) 
        ev->read_event_handler=callback;
    if(mask & WRITEABLE) 
        ev->write_event_handler=callback;

    if (event_fd > event_loop->maxfd)
        event_loop->maxfd=event_fd;
}

void event_rm(registered_event * ev, int epfd){
    struct epoll_event e_event = {0, {0}};
    
    if (ev->mask == NONE)                                        
        return ;
    
    e_event.data.fd=ev->fd;
    if(ev->mask & WRITEABLE)
        e_event.events = EPOLLOUT;    //EPOLLIN or EPOLLOUT
    if(ev->mask & READABLE)
        e_event.events = EPOLLIN;    //EPOLLIN or EPOLLOUT

    ev->mask=NONE;
    printf("event RM OK [fd=%d], op=%d, events[%0X]\n", e_event.data.fd, EPOLL_CTL_DEL, e_event.events);

    if(epoll_ctl(epfd, EPOLL_CTL_DEL, ev->fd, &e_event)==0)
        close(ev->fd);


}

void event_add(int event_fd,eventloop* eventLoop, int mask){
    struct epoll_event e_event = {0, {0}};
    int op = eventLoop->events_t[event_fd].mask == NONE ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
    if(mask & WRITEABLE)
        e_event.events = EPOLLOUT;    //EPOLLIN or EPOLLOUT
    if(mask & READABLE)
        e_event.events = EPOLLIN;    //EPOLLIN or EPOLLOUT

    e_event.data.fd=event_fd;
    printf("event add OK [fd=%d], op=%d, events[%0X]\n", e_event.data.fd, op, e_event.events);

    if (epoll_ctl(eventLoop->epollfd, op, e_event.data.fd, &e_event) < 0)
    {                   
        printf("%s: epoll_ctl, %s\n", __func__, strerror(errno));
    }
}

void runloop(eventloop* event_loop){
           
    int checkpos = 0, i;
    long now;
    registered_event *ev;           
    int triggeredfd;
    int mask;
    while (1) {
        now=time(NULL);
       
        check_timeout(now,&checkpos,event_loop);

        int number_of_events = epoll_wait(event_loop->epollfd, event_loop->fired_events_t, EVENTS_MAX+1, 20);
        if (number_of_events < 0) {
            printf("epoll_wait error, exit\n");
            break;
        }
        

        for (i = 0; i < number_of_events; i++) {
            triggeredfd = event_loop->fired_events_t[i].data.fd;
            ev =  &event_loop->events_t[triggeredfd];
            mask= event_loop->fired_events_t[i].events;

            if(mask==EPOLLIN)
                ev->read_event_handler(ev->fd,ev,now,event_loop);
            if(mask==EPOLLOUT)
                ev->write_event_handler(ev->fd,ev,now,event_loop);
          //  ev = (fired_event*)aux_events[i].data.ptr;  
         //   if ((aux_events[i].events & EPOLLIN) && (ev->events & EPOLLIN))
               // ev->callback(ev->fd, ev->arg,now,event_loop);
          //  if ((aux_events[i].events & EPOLLOUT) && (ev->events & EPOLLOUT))
              //  ev->callback(ev->fd, ev->arg,now,event_loop);
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
            long duration = now - event_loop->events_t[ck].last_active;       
            if (duration >= timeout) {
                close(event_loop->events_t[ck].fd);                           
                printf("[fd=%d] timeout\n", event_loop->events_t[ck].fd);
                event_rm(&event_loop->events_t[ck],event_loop->epollfd);                   
            }
        
    }
    *checkpos=ck;
}