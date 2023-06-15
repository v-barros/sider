#ifndef STRING_H_
#define STRING_H_

typedef struct string String;
struct string {
    char * text;
    int len;
};

String *_new(char *);
String * _newl(char *, int );
char * get(String*);
void * set(String*);
int len(String*);

#endif // STRING_H_