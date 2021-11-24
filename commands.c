#include "commands.h"
#include "server.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define maxkeylength 100
#define maxvaluelength 100

char itochar(int n);

int is_valid_get(char*c, int len){
    int i=0,aux =0;
    char * key;
    if(len>maxkeylength+4)
        return -1;

    while (i<3){
        if(c[i]!=tolower(userCommandTable[0].name[i]))
            return -1;
        i++;
    }
    if(c[i++]!=' ')
        return -1;
    key =c+4;
        
    aux = len - 4; // 3 
    //g e t - f o o - b a r
    //  0 1 2 3 4 5 6 7 8 9 10
    i = 0;
    while(i<aux){
        if(*key+i == ' ')
            return -1;
        i++;
    }
    return aux;    
}

int is_valid_set(char*c,int len){
    return -1;
}


int encode_get(char *src,int len,char * dest){
    dest[0]='$';
    dest[1]='0';
    dest[2]='$';   
    dest[3]=itochar(len);
    memcpy(dest+4,src+4,len);    
}
char itochar(int n){
    if(n>=0 && n<=9)
        return n+0x30;
    return 0x30;
}
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

void trimGetArg(char *, char *);
void trimSetArgs(char*, char *,char *);

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
    *(aux+len)='\0';
    printf("key %s len %d", aux,len);
    memcpy(kp,aux,len);
    
    aux = p;
    len = 0;
    while(*p++!='\0'){
        len++;
    }
    *(aux+len)='\0';
    printf("value %s ", aux);
    memcpy(vp,aux,len);
}