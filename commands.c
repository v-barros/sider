#include "commands.h"
#include <stdio.h>
#include <string.h>
#define maxkeylength 100

void trimGetArg(char *, char *);
void trimSetArgs(char*, char *,char *);

int getFun(char * args){
    char key[maxkeylength];
    trimGetArg(args,key);
    printf("\nGET key= \"%s\"",key);
    return 1;
}

int setFun(char * args){
    char key[maxkeylength];
    char value[maxkeylength];
    trimSetArgs(args,key,value);
    printf("SET key= \"%s\" value= \"%s\"",key,value);
    return 1;
}
void trimGetArg(char * s,char *kp){
    char * p = s;
    while(*p++!=' ');
    memcpy(kp,p,strlen(p));
}

void trimSetArgs(char *s, char *kp,char *vp){
    char * p = s,*aux;
    int len=0;
    while(*p++!=' ');
    aux = p;
    while(*p++!=' '){
        len++;
    }
    memcpy(kp,aux,len);
    aux = p;
    len = 0;
    while(*p++!='\0'){
        len++;
    }
    memcpy(vp,aux,len);
}