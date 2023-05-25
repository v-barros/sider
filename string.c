#include "string.h"
#include <stdlib.h>
#include <string.h>

struct string {
    char * text;
    int len;
};

String *_new(char * p){
    return _newl(p,strlen(p));
}

String * _newl(char *p, int len){
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
void * set(String*s){
}
int len(String* s){
    return s->len;
}