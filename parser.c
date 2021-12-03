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
    aux = aux+3;
    while(*aux++!='$');
    memcpy(keyp,aux,keylen);
}

// $1$3$3$foobar\r\n
void trimSetArgs(char *s, char *kp,char *vp){
    char * p,*aux;
    int klen=0,vlen=0;

    klen = stoi(s+3);
    p=s+3;
    while(*p++!='$');
    vlen = stoi(p);

    while(*p++!='$');
    
    memcpy(kp,p,klen);
    p+=klen; 
    memcpy(vp,p,vlen);
}

int stoi(char * str){
    int n=0,i=0,temp=0; 
    int j=0;
    
    while (*str!='$')
    {
        n = (n*10)+(*str - '0');
        str++;
    }
    return n;
}

