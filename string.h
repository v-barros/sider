#ifndef STRING_H_
#define STRING_H_

typedef struct string String;


String *_new(char *);
String * _newl(char *, int );
char * get(String*);
void * set(String*);
int len(String*);

#endif // STRING_H_