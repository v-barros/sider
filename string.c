#include "string.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

int _sprintf_internal(String * s,const char * fmt,va_list ap);

String * _newNull(){
    String * s = (String *) calloc(1,sizeof(String));
    if(!s)
        abort();
    s->len=0;
    return s;
}
String *_new(const char * p){
    return _newl(p,strlen(p));
}

String * _newl(const char *p, int len){
    String * s = (String *) calloc(1,sizeof(String));
    if(!s)
        abort();
    s->text = (char*)calloc(1,len+1);

    if(!s->text)
        abort();
    strcpy(s->text,p);
    *(s->text+len)='\0';
    s->len = len;
    return s; 
}

char * get(String* s){
    return s->text;
}

void * set(String **s,const char * str){
    if((*s)->text)
        free((*s)->text);
    (*s)->len=0;
    s = _new(str);
}


int len(String* s){
    return s->len;
}

int _sprintf(String *s,const char* fmt,...){
    va_list ap;
    va_start(ap, fmt);


    if(!s)
        return NULL;
    if(s->len>0)
        free(s->text);
    
    size_t len = strlen(fmt);

    s->text = (char*)calloc(1,len+1);

    va_end(ap);
    return _sprintf_internal(s,fmt,ap);
}

int _sprintf_internal(String * s,const char * fmt,va_list ap){
    va_list cpy;
    char staticbuf[1024];
    size_t buflen;
    int bufstrlen;
    buflen = sizeof(staticbuf);

    
    va_copy(cpy,ap);
    bufstrlen = vsnprintf(staticbuf, buflen, fmt, cpy);
    va_end(cpy);

    if (bufstrlen < 0)
        return NULL;
    if (((size_t)bufstrlen) >= buflen) 
        return NULL;

  
  
    s->text = (char *) malloc(bufstrlen);

    memcpy(s->text,staticbuf,bufstrlen);
    return bufstrlen;
}