#include "eventloop.h"

static inline void check_timeout(long now,int *checkpos,eventloop* event_loop);

eventloop * init_loop(int port){
    int epollfd = epoll_create(EVENTS_MAX+1);
    eventloop * ev = (eventloop*) malloc (sizeof(eventloop));
    if(!ev)
        abort();
    ev->epollfd=epollfd;
    int socket = init_conn(port);
    printf("listening socket fd [%d]\n",socket);    
    event_set(&ev->events_t[EVENTS_MAX], socket, accept_con, &ev->events_t[EVENTS_MAX],time(NULL));
    event_add(&ev->events_t[EVENTS_MAX],ev->epollfd,EPOLLIN);
    
    return ev;
}

void event_set(fired_event * ev, int fd, event_handler callback, void * arg,long time_now){
    ev->fd = fd;
    ev->callback = callback;
    ev->events = 0;
    ev->arg = arg;
    ev->status = EVENT_OFF;
    ev->last_active = time_now;    
}

void event_rm(fired_event * ev, int fd){
    struct epoll_event e_event = {0, {0}};

    if (ev->status == EVENT_OFF)                                        
        return ;

    e_event.data.ptr = ev;
    ev->status = EVENT_OFF;                                    
    epoll_ctl(fd, EPOLL_CTL_DEL, ev->fd, &e_event);
}

void event_add(fired_event *ev, int fd, int event){
    
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

void runloop(eventloop* event_loop){
           
    int checkpos = 0, i;
    long now;
    struct epoll_event aux_events[EVENTS_MAX+1];             
    while (1) {
        now=time(NULL);
       
        check_timeout(now,&checkpos,event_loop);

        int number_of_events = epoll_wait(event_loop->epollfd, aux_events, EVENTS_MAX+1, 20);
        if (number_of_events < 0) {
            printf("epoll_wait error, exit\n");
            break;
        }
        
        for (i = 0; i < number_of_events; i++) {
            fired_event *ev = (fired_event*)aux_events[i].data.ptr;  
            if ((aux_events[i].events & EPOLLIN) && (ev->events & EPOLLIN))
                ev->callback(ev->fd, ev->arg,now,event_loop);
        }
    }
    return 0;
}

static inline void check_timeout(long now,int *checkpos,eventloop* event_loop){
    int i;
    int ck = *checkpos;
    for (i = 0; i < 50; i++, ck++) {
            ck%=EVENTS_MAX;
            if (event_loop->events_t[ck].status == EVENT_OFF){
                continue;
            }        
            long duration = now - event_loop->events_t[ck].last_active;       
            if (duration >= 30L) {
                close(event_loop->events_t[ck].fd);                           
                printf("[fd=%d] timeout\n", event_loop->events_t[ck].fd);
                event_rm(&event_loop->events_t[ck],event_loop->epollfd);                   
            }
        
    }
    *checkpos=ck;
}