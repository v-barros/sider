/*
 * commands.c
 *
 *  Created on: 2021-11-21
 *      Author: @v-barros
 */
#include "server.h"
#include "parser.h"
#include "utils.h"
#include "commands.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "resp_parser.h"

#define maxkeylength 1000
#define maxvaluelength 1000

void getCommand(Table * table,char * args,server_resp * reply){
    char key[maxkeylength];
    const char * value=NULL;
    memset(key,0,maxkeylength);
    trimGetArg(args,key);
    //printf("GET key= \"%s\"",key);
    value = getValue(table,key);
   // if(value!=NULL)
    putText(reply,value);
    
}

void setCommand(Table * table,char * args,server_resp * reply){
    char key[maxkeylength];
    char value[maxvaluelength];
    memset(key,0,maxkeylength);
    memset(value,0,maxkeylength);
    trimSetArgs(args,key,value);
    printf("SET key= \"%s\" value= \"%s\"",key,value);
    if(put(table,key,value))
        addReplyBool(reply,1);
    else
        addReplyBool(reply,0);

}

