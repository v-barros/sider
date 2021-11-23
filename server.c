/*
 * server.c
 *
 *  Created on: 2021-11-21
 *      Author: @v-barros
 */
#include "server.h"
#include "ht.h"
#include "commands.h"
#define replies 2


struct serverReply {
    char * text;   
};

struct serverReply serverReplyTable[replies] ={
    {
        "KO",
    },
    {
        "OK",
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
        reply =0;
        // read the message from client and copy it in buffer
        read(sockfd, buff, sizeof(buff));
        // print buffer which contains the client contents
        printf("\nFrom client: %s\t", buff);
        n = formatCommand(buff);
        if(n!=-1){
            if(userCommandTable[n].execFunction(buff))
                reply =1;            
         }
        bzero(buff, MAX);
        memcpy(buff,serverReplyTable[reply].text,3);
        
        write(sockfd, buff, sizeof(buff));
        // if msg contains "Exit" then server exit and chat ended.
        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
    }
}

int formatCommand(char * s){
    int i=0;
    for(;i<commands;i++){
        if(strncmp(s,userCommandTable[i].name,3) ==0)
        {
            return i;
        }
    }    
    return -1;
}

