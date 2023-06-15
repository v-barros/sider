#ifndef STRING_H_
#define STRING_H_

typedef struct string String;
struct string {
    char * text;
    int len;
};

String *_new(const char *);
String * _newl(const char *, int );
String * _newNull();
int _sprintf(String *,const char*,...);
char * get(String*);
void * set(String**,const char*);
int len(String*);

#endif // STRING_H_