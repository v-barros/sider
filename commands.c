#include "commands.h"
#include "server.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define maxkeylength 100
#define maxvaluelength 100

// n must be positive
// convert n to array of char in dest and return num of chars
// ex: 65535
// put "65535" on dest
// return 5
int toString(int n,char*dest);

char itochar(int n);

// check if input string is a valid get command
// return -1 if false
// return key length otherwise
// ex: c = "get key" len = 7
// return 3
// ex: c = "get key key2" len = 11
// return -1
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
    int n = toString(len,dest+3);
    dest[3+n]='$';
    memcpy(dest+4+n,src+4,len);
    dest[4+n+len]='\r';
    dest[5+n+len]='\n';
    return 1;
}
int toString(int n,char *out){
    int len=0;
    int aux=0;
    char temp[10];

    while(n){
        temp[len++] =n%10;
        n=n/10;
    }
    aux = len;
    while(aux){
        out[len-aux]=itochar(temp[aux-1]);
        aux--;
    }
    return len;
}
/*
    $<command>$<param_len>${param}\r\n
    GET key
    $0$3$key\r\n

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

char itochar(int n){
    if(n>=0&&n<=9)
        return n+0x30;
    return 0x20;
}