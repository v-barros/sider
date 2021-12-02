/*
 * parser.h
 *
 *  Created on: 2021-12-02
 *      Author: @v-barros
 */
#ifndef PARSER_H_
#define PARSER_H_

// src = $0$3$key\r\n
// put key at keyp and return key length
int trimGetArg(char * src, char *keyp);

// str = "123$"
int stoi(char * str);

void trimSetArgs(char*, char *,char *);

#endif // PARSER_H_