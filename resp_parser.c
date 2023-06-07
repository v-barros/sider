/*
 * resp_parser.h
 *
 *  Created on: 2023-05-28
 *      Author: @v-barros
 */
#include "resp_parser.h"

void putError(server_resp * rp){
    char * s = "$2$KO\r\n";
    memcpy(rp->text,s,7);
    rp->len = 7;
}

void putOk(server_resp* rp){
    char * s = "$2$OK\r\n";
    memcpy(rp->text,s,8);
    rp->len = 8;
}
//$3$val\r\n
void putText(server_resp*rp, const char * text){
    if(!text){
        *(rp->text)='$';
        *(rp->text+1)='1';
        *(rp->text+2)='$';
        *(rp->text+3)='\r';
        *(rp->text+4)='\n';
        rp->len = 5;
        return;
    }
    int aux,len = strlen(text);
    *(rp->text)='$';
    aux = toString(len,rp->text+1);
    rp->text[aux+1]='$';

    memcpy(rp->text+aux+2,text,len);

    rp->text[aux+2+len]='\r';
    rp->text[aux+3+len]='\n';
    rp->len = aux + 5+len;
}
