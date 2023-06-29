/*
 * commands.c
 *
 *  Created on: 2021-11-21
 *      Author: @v-barros
 */
#include "server.h"
#include "parser.h"
#include "utils.h"
#include "connection.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "resp_parser.h"
#define maxkeylength 1000
#define maxvaluelength 1000

void getCommand(client *c){
    /*
    char key[maxkeylength];
    const char * value=NULL;
    memset(key,0,maxkeylength);
    trimGetArg(args,key);
    //printf("GET key= \"%s\"",key);
    value = getValue(table,key);
   // if(value!=NULL)
    putText(reply,value);
    */
    
}

void setCommand(client *c){
    int ret;
    if(c->argc!=3)
        ret =0;


    printf("SET key= \"%s\" value= \"%s\"\n",c->argv[1],c->argv[2]);
    printf("table size before command: %d\n",server.ht_table->numberOfEntries(server.ht_table));
    if(server.ht_table->put(server.ht_table,c->argv[1],c->argv[2]));
    printf("table size after command: %d\n",server.ht_table->numberOfEntries(server.ht_table));
   /* if(put(table,key,value))
        addReplyBool(reply,1);
    else
        addReplyBool(reply,0);
    */
}

