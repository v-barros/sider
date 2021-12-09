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

int formatCommand(char * s);
Table* table;
// Function designed for chat between client and server.
void run(int sockfd)
{
    table = createTable();
    char buff[MAX];
    int n;
    struct serverReply reply={"KO",2};
    for (;;) {
        putError(&reply);
        
        memset(buff,0,sizeof(buff));
        
        // read the message from client and copy it in buffer
        read(sockfd, buff, sizeof(buff));
        // print buffer which contains the client contents
        printf("\nFrom client: %s\t", buff);
        n = formatCommand(buff);
        if(n!=-1){
            userCommandTable[n].execFunction(table,buff,&reply);            
         }
        memset(buff,0,sizeof(buff));
        memcpy(buff,reply.text,reply.len);
        
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
        if(strncmp(s+1,userCommandTable[i].name,1) ==0)
        {
            return i;
        }
    }    
    return -1;
}

void putError(struct serverReply * rp){
    rp->text = "KO";
    rp->len = 2;
}

void putOk(struct serverReply* rp){
    rp->text = "OK";
    rp->len = 2;
}

void putText(struct serverReply*rp, const char * text){
    rp->len = strlen(text);
    rp->text = text; 
}