/*
 * commands.h
 *
 *  Created on: 2021-11-21
 *      Author: @v-barros
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_
#define COMMANDS 2
#include "ht.h"
#include "server.h"

typedef struct siderCommand siderCommand;
typedef void command_proc(Table *,char * ,server_resp*);
typedef int command_get_keys(siderCommand *cmd, char **argv, int argc, getKeysResult *result);

struct siderCommand{
    char *name;
    command_proc *proc;
    command_get_keys *getKeys;
};
void getCommand(Table * table,char * args,server_resp*);

void setCommand(Table * table,char * args,server_resp*);

static const struct siderCommand userCommandTable[COMMANDS]={
    {
        "get",
        getCommand
    },
    {
        "set",
        setCommand
    }
};

#endif // COMMANDS_H_