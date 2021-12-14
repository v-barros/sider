/*
 * utils.c
 *
 *  Created on: 2021-12-02
 *      Author: @v-barros
 */
#include "utils.h"

char itochar(int n){
    return n>=0&&n<=9 ? n+0x30 : 0x20;
}

int chartoi(char c){
    return c>=0x30 && c<=0x39 ? c - 0x30 : -1;
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

int toString(int n,char *out){
    int len=0;
    int aux=0;
    char temp[11];

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