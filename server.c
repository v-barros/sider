/*
 * server.c
 *
 *  Created on: 2021-11-21
 *      Author: @v-barros
 */
#include "server.h"
#include "ht.h"
#define commands 2
typedef int fun(char * args);

struct userCommand{
    char *name;
    fun * execFunction;
};

struct userCommand userCommandTable[2]={
    {
        "get",
        getFun
    },
    {
        "set",
        setFun
    }
};

int formatCommand(char * s);
Table* table;
// Function designed for chat between client and server.
void run(int sockfd)
{
    table = createTable();
    char buff[MAX];
    int n, reply;
    for (;;) {
        bzero(buff, MAX);
   
        // read the message from client and copy it in buffer
        read(sockfd, buff, sizeof(buff));
        // print buffer which contains the client contents
        printf("From client: %s\t To client : ", buff);
        bzero(buff, MAX);
        memcpy(buff,"0\0",2);
        n = formatCommand(buff);
        if(n){
            if(userCommandTable[n].execFunction(buff))
                memcpy(buff,"1\0",2);                
        }
        
        write(sockfd, buff, sizeof(buff));
        // if msg contains "Exit" then server exit and chat ended.
        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
    }
}

int formatCommand(char * s){
    if(strlen(s)!=3) return 0;
    int i=0;
    for(;i<commands;i++)
        if(!strncmp(s,userCommandTable[i].name,3))
            return 1;
    return 0;
}

int getFun(char * args){
    char *p = args;
    while(*p++!=' ');
    printf("%s",p);
    return 0x31;
}

int setFun(char * args){
    return 0x31;
}