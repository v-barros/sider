/*
 * resp_parser.h
 *
 *  Created on: 2023-05-28
 *      Author: @v-barros
 */
#include "resp_parser.h"

void putError(server_resp * rp){
    char * s = "$2$KO\r\n";
    memcpy(rp->buffer,s,7);
    rp->bufpos = 7;
}

//$3$val\r\n
void putText(server_resp*rp, const char * text){
    if(!text){
        *(rp->buffer)='$';
        *(rp->buffer+1)='1';
        *(rp->buffer+2)='$';
        *(rp->buffer+3)='\r';
        *(rp->buffer+4)='\n';
        rp->bufpos = 5;
        return;
    }
    int aux,len = strlen(text);
    *(rp->buffer)='$';
    aux = toString(len,rp->buffer+1);
    rp->buffer[aux+1]='$';

    memcpy(rp->buffer+aux+2,text,len);

    rp->buffer[aux+2+len]='\r';
    rp->buffer[aux+3+len]='\n';
    rp->bufpos = aux + 5+len;
}

void addReplyNull(server_resp *rp){
    addReplyLen(rp,"_\r\n",3);
}

void addReplyBool(server_resp *rp, int b){
    addReply(rp, b ? shared.cone->text : shared.czero->text);
}
void addReplyVerbatim(server_resp *rp, const char *s, size_t len, const char *ext);

void addReplyBulk(server_resp *rp, const char *str){
    addReplyBulkLen(rp,str);
    addReply(rp,str);
    addReply(rp,shared.crlf->text);
}

void addReplyBulkLen(server_resp *rp, const char *s) {
    size_t len = strlen(s);

    addReplyLongLongWithPrefix(rp,len,'$');
}
void addReplyBulkCString(server_resp *rp, const char *s);
void addReplyBulkCBuffer(server_resp *rp, const void *p, size_t len);
void addReplyBulkLongLong(server_resp *rp, long long ll);
void addReplyErrorObject(server_resp *rp, const char *err);
void addReplyOrErrorObject(server_resp *rp, const char *reply);
void addReplyError(server_resp *rp, const char *err);
void addReplyStatus(server_resp *rp, const char *status);
void addReplyDouble(server_resp *rp, double d);
void addReplyLongLongWithPrefix(server_resp *rp, long long ll, char prefix){
    return;
}
void addReplyBigNum(server_resp *rp, const char* num, size_t len);
void addReplyHumanLongDouble(server_resp *rp, long double d);
void addReplyLongLong(server_resp *rp, long long ll);
void addReplyArrayLen(server_resp *rp, long length);
void addReplyMapLen(server_resp *rp, long length);
void addReplySetLen(server_resp *rp, long length);
void addReplyAttributeLen(server_resp *rp, long length);
void addReplyPushLen(server_resp *rp, long length);
void addReplyHelp(server_resp *rp, const char **help);

size_t _addReplyToBuffer(server_resp *rp, const char *s, size_t len) {
    size_t available = rp->buflen - rp->bufpos;

    size_t reply_len = len > available ? available : len;
    
    memcpy(rp->buffer+rp->bufpos,s,reply_len);
    rp->bufpos+=reply_len;
    return reply_len;
}

/* Add the 'str' string representation to the server output buffer. */
void addReplyLen(server_resp *rp, const char *str,size_t len) {
   _addReplyToBuffer(rp,str,len);
}

void addReply(server_resp *rp, const char *str) {
   _addReplyToBuffer(rp,str,strlen(str));
}