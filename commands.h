/*
 * commands.h
 *
 *  Created on: 2021-11-21
 *      Author: @v-barros
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_
#define commands 2
#include "ht.h"
#include "server.h"

typedef void fun(Table * table,char * args,struct serverReply*);
typedef int valid(char * c,int len);

struct userCommand{
    char *name;
    fun * execFunction;
    valid * is_valid;
};
void getFun(Table * table,char * args,struct serverReply*);

void setFun(Table * table,char * args,struct serverReply*);

int is_valid_get(char*c,int len);

int is_valid_set(char*c,int len);

/*

    response
    $<response_len>{response}\r\n
*/

static const struct userCommand userCommandTable[commands]={
    {
        "0",
        getFun,
        is_valid_get
    },
    {
        "1",
        setFun,
        is_valid_set
    }
};

#endif // COMMANDS_H_