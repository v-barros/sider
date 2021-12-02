/*
 * commands.c
 *
 *  Created on: 2021-11-21
 *      Author: @v-barros
 */
#include "commands.h"
#include "server.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#define maxkeylength 110
#define maxvaluelength 110

// n must be positive
// convert n to array of char in dest and return num of chars
// ex: 65535
// put "65535" on dest
// return 5
int toString(int n,char*dest);

char itochar(int n);

int chartoi(char c);

int stoi(char *);

int ipow(int, int);

// check if input string is a valid get command
// return -1 if false
// return key length otherwise
// ex: c = "get key" len = 7
// return 3
// ex: c = "get key key2" len = 11
// return -1
int is_valid_get(char*c, int len){
    int i=0,aux =0;

    return aux;    
}

int is_valid_set(char*c,int len){
    return -1;
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
    $1$3key$5value\r\n

    response
    $<response_len>{response}\r\n
*/

int trimGetArg(char *, char *);
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


// $0$3$key\r\n
// put key at kp and return key length
int trimGetArg(char * s,char *kp){
    
    char * aux = s;
    int keylen = stoi(aux+3);
    assert(keylen!=-1);
    aux = aux+3;

    while(*aux++!='$');
    memcpy(kp,aux,keylen);
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
    return n>=0&&n<=9 ? n+0x30 : 0x20;
}

int chartoi(char c){
    return c>=0x30 && c<=0x39 ? c - 0x30 : -1;
}
// str = "123$"
int stoi(char * str){
    int n=0,i=0,temp=0,aux[7]; 
    int j=0;
    bzero(aux,7);

    do{ 
        aux[i] = chartoi(str[i]);
    }while(str[++i]!=0x24);
 
    i--;
    temp = i;
    // aux = 1  , 2  , 3  , -1 , 0  , 0  , 0
    //       ^2 , ^1 , ^0
    while (i>=0)
    {
        n+= aux[i] * (ipow(10,temp - i));
        i--;
    }
    return n;
}

int ipow(int base, int exp)
{

    int result = 1;
    while(1)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        if (!exp)
            break;
        base *= base;
    }

    return result;
}