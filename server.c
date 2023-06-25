/*
 * server.c
 *
 *  Created on: 2021-11-21
 *      Author: @v-barros
 */
#include <stddef.h>
#include "connection.h"
#include "ht.h"
#include "utils.h"
#include <assert.h>
#include "server.h"
#define replies 2
//#define PORT 8000

/*Global server*/
struct server_str server;

/* Global shared responses to build server responses*/
struct shared_resp shared;

static const struct siderCommand userCommandTable[COMMANDS]={
    {
        "get",
        getCommand
    },
    {
        "set",
        setCommand
    }
};


int processCommand(client *c){


     /* Now lookup the command and check ASAP about trivial error conditions
     * such as wrong arity, bad command name and so forth. */
   // c->cmd = c->lastcmd = lookupCommand(c->argv,c->argc);
    if (!c->cmd) {
        printf("unknown command '%s'",(char*)c->argv[0]);
        
        return C_OK;
    }
    /*
    int is_read_command = (c->cmd->flags & CMD_READONLY) ||
                           (c->cmd->proc == execCommand && (c->mstate.cmd_flags & CMD_READONLY));
    int is_write_command = (c->cmd->flags & CMD_WRITE) ||
                           (c->cmd->proc == execCommand && (c->mstate.cmd_flags & CMD_WRITE)); 
    */

    /* Exec the command */
    //call(c,CMD_CALL_FULL);

    return C_OK;
}

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
    shared.cone=_new(":1\r\n");
    shared.czero=_new(":0\r\n");
    int j;
    for (j = 0; j < OBJ_SHARED_BULKHDR_LEN; j++) {

        shared.mbulkhdr[j] = _newNull();
        shared.bulkhdr[j] = _newNull();
        _sprintf(shared.mbulkhdr[j],"*%d\r\n",j);
        _sprintf(shared.bulkhdr[j],"$%d\r\n",j);
    //    printf("%s\t%d\n",get(shared.bulkhdr[j]),len(shared.bulkhdr[j]));
       // printf("real length [%ld]\n", strlen(get(shared.bulkhdr[j])));
    }

}

void serverConfInit(){
    server.port=PORT;
    server.reply=create_resp();
    eventloop* ev_loop= init_loop(server.port);
    server.next_client_id =0;
    server.stat_numconnections=0;
    server.unixtime=time(NULL);
    server.el = ev_loop;

    printf("epoll file descriptor [%d]\n ",ev_loop->epollfd);
    printf("server running!\n using port [%d]\n", server.port);
}

int reply(const char * buff, int len){
    return 1;
}

int formatCommand(char * s){
    int i=0;
    for(;i<COMMANDS;i++){
        if(strncmp(s+1,userCommandTable[i].name,3) ==0)
        {
            return i;
        }
    }    
    return -1;
}

server_resp * create_resp(){
    server_resp * sr = (server_resp *) malloc(sizeof(server_resp));
    assert(sr!=NULL);
    sr->bufpos=0;
    sr->buflen=OUTPUT_BUF_LEN;
    return sr;
}

int main(void){
    createSharedResps();
    serverConfInit();
    runloop(server.el);
    return 0;
}