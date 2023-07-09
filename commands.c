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
    
    const char * resp;
    int ret;
    if(c->argc!=2)
        ret=0;
    printf("GET key= \"%s\"\n",c->argv[1]);
    resp=server.ht_table->getValue(server.ht_table,c->argv[1]);
    if(resp==NULL){
        addReply(c,shared.null->text);
        printf("No key\n");
    }else{
        addReply(c,resp);
        addReply(c,shared.crlf->text);
        printf("value = \"%s\"\n",resp);
    }
    
}

void setCommand(client *c){
    int ret;
    if(c->argc!=3)
       addReply(c,"syntax error");

    printf("SET key= \"%s\" value= \"%s\"\n",c->argv[1],c->argv[2]);
    printf("table size before command: %d\n",server.ht_table->numberOfEntries(server.ht_table));
    if(server.ht_table->put(server.ht_table,c->argv[1],c->argv[2]));
    printf("table size after command: %d\n",server.ht_table->numberOfEntries(server.ht_table));
    addReply(c, shared.ok->text);
}

