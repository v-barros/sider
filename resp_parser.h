/*
 * resp_parser.h
 *
 *  Created on: 2023-05-28
 *      Author: @v-barros
 */
#ifndef RESP_PARSER_H_
#define RESP_PARSER_H_
#include "server.h"


void addReplyNull(client *c);
void addReplyBool(client *c, int b);
void addReplyVerbatim(client *c, const char *s, size_t len, const char *ext);
void addReplyBulk(client *c, const char *str);
void addReplyBulkLen(client *c, const char *s);
void addReplyBulkCString(client *c, const char *s);
void addReplyBulkCBuffer(client *c, const void *p, size_t len);
void addReplyBulkLongLong(client *c, long long ll);
void addReplyErrorObject(client *c, const char *err);
void addReplyOrErrorObject(client *c, const char *reply);
void addReplyError(client *c, const char *err);
void addReplyStatus(client *c, const char *status);
void addReplyDouble(client *c, double d);
void addReplyLongLongWithPrefix(client *c, long long ll, char prefix);
void addReplyBigNum(client *c, const char* num, size_t len);
void addReplyHumanLongDouble(client *c, long double d);
void addReplyLongLong(client *c, long long ll);
void addReplyArrayLen(client *c, long length);
void addReplyMapLen(client *c, long length);
void addReplySetLen(client *c, long length);
void addReplyAttributeLen(client *c, long length);
void addReplyPushLen(client *c, long length);
void addReplyHelp(client *c, const char **help);
void addReplyLen(client *c, const char *str,size_t len);
void addReply(client *c, const char *str);
#endif //RESP_PARSER_H_