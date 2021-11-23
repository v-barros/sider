#ifndef COMMANDS_H_
#define COMMANDS_H_

#define commands 2

typedef int fun(char * args);

struct userCommand{
    char *name;
    fun * execFunction;
};
int getFun(char * args);

int setFun(char * args);

static const struct userCommand userCommandTable[commands]={
    {
        "get",
        getFun
    },
    {
        "set",
        setFun
    }
};



#endif // COMMANDS_H_