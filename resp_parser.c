/*
 * resp_parser.h
 *
 *  Created on: 2023-05-28
 *      Author: @v-barros
 */
#include "resp_parser.h"

void putError(server_resp *rp){
    char * s = "$2$KO\r\n";
    memcpy(rp->buffer,s,7);
    rp->bufpos = 7;
}

//$3$val\r\n
void putText(server_resp *rp, const char * text){
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

void addReplyNull(client *c){
    addReplyLen(c,"_\r\n",3);
}

void addReplyBool(client *c, int b){
    addReply(c, b ? shared.cone->text : shared.czero->text);
}
void addReplyVerbatim(client *c, const char *s, size_t len, const char *ext);

void addReplyBulk(client *c, const char *str){
    addReplyBulkLen(c,str);
    addReply(c,str);
    addReply(c,shared.crlf->text);
}

void addReplyBulkLen(client *c, const char *s) {
    size_t len = strlen(s);

    addReplyLongLongWithPrefix(c,len,'$');
}
void addReplyBulkCString(client *c, const char *s);
void addReplyBulkCBuffer(client *c, const void *p, size_t len);
void addReplyBulkLongLong(client *c, long long ll);
void addReplyErrorObject(client *c, const char *err);
void addReplyOrErrorObject(client *c, const char *reply);
void addReplyError(client *c, const char *err);
void addReplyStatus(client *c, const char *status);
void addReplyDouble(client *c, double d);

/* Add a long long as integer reply or bulk len / multi bulk count.
 * Basically this is used to output <prefix><long long><crlf>. */
void addReplyLongLongWithPrefix(client *c, long long ll, char prefix){
    char buf[128];
    int len;

    /* Things like $3\r\n or *2\r\n are emitted very often by the protocol
     * so we have a few shared objects to use if the integer is small
     * like it is most of the times. */
    if (prefix == '*' && ll < OBJ_SHARED_BULKHDR_LEN && ll >= 0) {
        addReply(c,shared.mbulkhdr[ll]->text);
        return;
    } else if (prefix == '$' && ll < OBJ_SHARED_BULKHDR_LEN && ll >= 0) {
        addReply(c,shared.bulkhdr[ll]->text);
        return;
    }

    buf[0] = prefix;
    len = ll2string(buf+1,sizeof(buf)-1,ll);
    buf[len+1] = '\r';
    buf[len+2] = '\n';
    addReplyLen(c,buf,len+3);
}
void addReplyBigNum(client *c, const char* num, size_t len);
void addReplyHumanLongDouble(client *c, long double d);
void addReplyLongLong(client *c, long long ll);
void addReplyArrayLen(client *c, long length);
void addReplyMapLen(client *c, long length);
void addReplySetLen(client *c, long length);
void addReplyAttributeLen(client *c, long length);
void addReplyPushLen(client *c, long length);
void addReplyHelp(client *c, const char **help);

size_t _addReplyToBuffer(client *c, const char *s, size_t len) {
    size_t available = c->buf_size - c->bufpos;

    size_t reply_len = len > available ? available : len;
    
    memcpy(c->buf+c->bufpos,s,reply_len);
    c->bufpos+=reply_len;
    return reply_len;
}

/* Add the 'str' string representation to the server output buffer. */
void addReplyLen(client *c, const char *str,size_t len) {
   _addReplyToBuffer(c,str,len);
}

void addReply(client *c, const char *str) {
   _addReplyToBuffer(c,str,strlen(str));
}