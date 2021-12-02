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

#define maxkeylength 110
#define maxvaluelength 110

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
    char * value;
    memset(key,0,maxkeylength);
    trimGetArg(args,key);
    printf("GET key= \"%s\"",key);
  //  value = getValue(table,key);
  //  printf(" value=\"%s\"", value);
  //  putText(reply,key);
    putOk(reply);
}

void setFun(Table * table,char * args, struct serverReply * reply){
    char key[maxkeylength];
    char value[maxvaluelength];
    trimSetArgs(args,key,value);
    printf("SET key= \"%s\" value= \"%s\"",key,value);
    //if(put(table,key,value))
        putOk(reply);
    //else
    //    putError(reply);
}

