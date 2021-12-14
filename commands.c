/*
 * commands.c
 *
 *  Created on: 2021-11-21
 *      Author: @v-barros
 */
#include "commands.h"
#include "server.h"
#include "parser.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#define maxkeylength 1000
#define maxvaluelength 1000

int is_valid_get(char*c, int len){
    return -1;    
}

int is_valid_set(char*c,int len){
    return -1;
}


/*
    $<command>$<param_len>${param}\r\n
    GET key
    $0$3$key\r\n

    $<command>$<param1_len>$<param2_len>{param1}{param2}\r\n
    SET key value
    $1$3key$5value\r\n

    response
    $<response_len>{response}\r\n
*/


void getFun(Table * table,char * args,struct serverReply * reply){
    char key[maxkeylength];
    const char * value;
    memset(key,0,maxkeylength);
    trimGetArg(args,key);
    printf("GET key= \"%s\"",key);
    value = getValue(table,key);
    printf(" value=\"%s\"", value);
    putText(reply,value);
    removeByKey(table,key);
}

void setFun(Table * table,char * args, struct serverReply * reply){
    char key[maxkeylength];
    char value[maxvaluelength];
    memset(key,0,maxkeylength);
    memset(value,0,maxkeylength);
    trimSetArgs(args,key,value);
    printf("SET key= \"%s\" value= \"%s\"",key,value);
    if(put(table,key,value))
       // putText(reply,value);
        putOk(reply);
    else
        putError(reply);
}

