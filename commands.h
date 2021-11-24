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

int encode_get(char *src,int len,char * dest);

/*
    $<command>$<param_len>{param}\r\n
    GET key
    $0$3key\r\n

    $<command>$<param1_len>$<param2_len>{param1}{param2}\r\n
    SET key value
    $1$3$5keyvalue\r\n

    response
    $<response_len>{response}\r\n
*/

static const struct userCommand userCommandTable[commands]={
    {
        "get",
        getFun,
        is_valid_get
    },
    {
        "set",
        setFun,
        is_valid_set
    }
};

#endif // COMMANDS_H_