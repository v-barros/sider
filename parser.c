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
    int keylen = stoi(src+3);
    src+=3;
    while(*src++!='$');
    memcpy(keyp,src,keylen);
    return keylen;
}

void trimSetArgs(char *s, char *kp,char *vp){
    char * p;
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
    int n=0; 
    
    while (*str!='$')
    {
        n = (n*10)+(*str - '0');
        str++;
    }
    return n;
}

