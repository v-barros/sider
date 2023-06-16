#include "string.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

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

/*
    change content in s.
    expects a pointer of String
    discard content in s and copy content from str

*/
void * set(String *s,const char * str){
    if(s->text)
        free(s->text);
    s->len=0;


    int len = strlen(str);
    s->text = (char*)calloc(1,len+1);
    assert(s->text);
    memcpy(s->text,str,len);
    s->text[len]='\0';
    s->len=len;
}


int len(String* s){
    return s->len;
}

/*
    Higher level function to perform sprintf-like operation with string structure
    return string lengh or NULL in case string doesn't exists.
    ex:
        _sprint(str,"Long: %ll\n",mylongvar);
    
    _sprintf will copy the result of a standard sprintf() call into s->text, discarding the current content at s->text.
*/
int _sprintf(String *s,const char* fmt,...){
    va_list ap;
    va_start(ap, fmt);

    if(!s)
        return -1;
    if(s->len>0)
        free(s->text);
    
    size_t len = strlen(fmt);
   
    va_end(ap);
    return _sprintf_internal(s,fmt,ap);
}


/*
    Lower level function to perform printf-like operation to format the string structure with the given format
    Expects va_list instead of being variadic
    return string lengh or -1.
    ex:
        _sprint(str,"Long: %ll\n",mylongvar);

    implements the internal interation with vsnprintf() using a STATIC BUFFER of length 1024, that means
    the formatted result cannot be bigger than that.
       
*/
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
        return -1;
    if (((size_t)bufstrlen) >= buflen) 
        return -1;
    
    
    s->text = (char*)calloc(1,bufstrlen+1);
    assert(s->text);
    
    memcpy(s->text,staticbuf,bufstrlen);
    s->len=bufstrlen;
    s->text[bufstrlen]='\0';
    return bufstrlen;
}