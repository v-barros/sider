/*
 * server.c
 *
 *  Created on: 2021-11-21
 *      Author: @v-barros
 */
#include "server.h"
#include "ht.h"
#include "commands.h"
#include "utils.h"
#include "assert.h"
#define replies 2
#define REPLYMAX 1024
//#define PORT 8000

/*Global server*/
struct server_str server;

/* Global shared responses to build server responses*/
struct shared_resp shared;

int formatCommand(char * s);

int reply(const char * buff, int len);

void createSharedResps(){
    /*
    For Simple Strings, the first byte of the reply is "+"
    For Errors, the first byte of the reply is "-"
    For Integers, the first byte of the reply is ":"
    For Bulk Strings, the first byte of the reply is "$"
    For Arrays, the first byte of the reply is "*"
    */
    shared.crlf=_new("\r\n");
    shared.ok=_new("+OK\r\n");
    shared.plus=_new("+");
}

void serverConfInit(){
    server.port=PORT;
    server.reply=create_resp();
    eventloop* ev_loop= init_loop(server.port);
    server.ev = ev_loop;
    printf("epoll file descriptor [%d]\n ",ev_loop->epollfd);
    printf("server running!\n using port [%d]\n", server.port);
}

int reply(const char * buff, int len){
    return 1;
}

int formatCommand(char * s){
    int i=0;
    for(;i<commands;i++){
        if(strncmp(s+1,userCommandTable[i].name,1) ==0)
        {
            return i;
        }
    }    
    return -1;
}

server_resp * create_resp(){
    server_resp * sr = (server_resp *) malloc(sizeof(server_resp));
    assert(sr!=NULL);
    sr->text = (char *) malloc(REPLYMAX);
    assert(sr->text!=NULL);
    sr->len=0;
    return sr;
}

int main(void){
    createSharedResps();
    serverConfInit();

    return 0;
}