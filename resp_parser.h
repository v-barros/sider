/*
 * resp_parser.h
 *
 *  Created on: 2023-05-28
 *      Author: @v-barros
 */
#ifndef RESP_PARSER_H_
#define RESP_PARSER_H_
#include "server.h"
void putError(server_resp *);

void putOk(server_resp*);

void putText(server_resp*, const char * text);

#endif //RESP_PARSER_H_