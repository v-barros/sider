/*
 * parser.c
 *
 *  Created on: 2021-12-02
 *      Author: @v-barros
 */

#include "parser.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

int trimGetArg(char * src,char *keyp){
    
    char * aux = src;
    int keylen = stoi(aux+3);
    assert(keylen!=-1);
    aux = aux+3;

    while(*aux++!='$');
    memcpy(keyp,aux,keylen);
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

