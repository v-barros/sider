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

int formatCommand(char * s);

serverReply * create_reply();

Table* table;
// Function designed for chat between client and server.
void run(int sockfd)
{
    table = createTable();
    char buff[MAX];
    int n;
    serverReply * reply= create_reply();
    for (;;) {
        putError(reply);
        
        memset(buff,0,sizeof(buff));
        
        // read the message from client and copy it in buffer
        read(sockfd, buff, sizeof(buff));
        // print buffer which contains the client contents
        printf("\nFrom client: %s\t", buff);
        n = formatCommand(buff);
        if(n!=-1){
            userCommandTable[n].execFunction(table,buff,reply);            
         }
        printf("\nResult: %s\n",reply->text);
        memset(buff,0,sizeof(buff));
        memcpy(buff,reply->text,reply->len);
        write(sockfd, buff, reply->len);
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

serverReply * create_reply(){
    serverReply * sr = (serverReply *) malloc(sizeof(serverReply));
    assert(sr!=NULL);
    sr->text = (char *) malloc(1024);
    assert(sr->text!=NULL);
    sr->len=0;
    return sr;
}

void putError(serverReply * rp){
    char * s = "$2$KO\r\n";
    memcpy(rp->text,s,7);
    rp->len = 7;
}

void putOk(serverReply* rp){
    char * s = "$2$OK\r\n";
    memcpy(rp->text,s,7);
    rp->len = 7;
}
//$3$val\r\n
void putText(serverReply*rp, const char * text){
    if(!text){
        *(rp->text)='$';
        *(rp->text+1)='-';
        *(rp->text+2)='1';
        *(rp->text+3)='$';
        *(rp->text+4)='\r';
        *(rp->text+5)='\n';
        rp->len = 6;
        return;
    }
    int aux,len = strlen(text);
    *(rp->text)='$';
    aux = toString(len,rp->text+1);
    rp->text[aux+1]='$';

    memcpy(rp->text+aux+2,text,len);

    rp->text[aux+2+len]='\r';
    rp->text[aux+3+len]='\n';
    rp->len = aux + 4+len;
}