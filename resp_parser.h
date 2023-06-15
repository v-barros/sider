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

void putText(server_resp*, const char * text);

void addReplyNull(server_resp *rp);
void addReplyBool(server_resp *rp, int b);
void addReplyVerbatim(server_resp *rp, const char *s, size_t len, const char *ext);
void addReplyBulk(server_resp *rp, const char *str);
void addReplyBulkLen(server_resp *rp, const char *s);
void addReplyBulkCString(server_resp *rp, const char *s);
void addReplyBulkCBuffer(server_resp *rp, const void *p, size_t len);
void addReplyBulkLongLong(server_resp *rp, long long ll);
void addReplyErrorObject(server_resp *rp, const char *err);
void addReplyOrErrorObject(server_resp *rp, const char *reply);
void addReplyError(server_resp *rp, const char *err);
void addReplyStatus(server_resp *rp, const char *status);
void addReplyDouble(server_resp *rp, double d);
void addReplyLongLongWithPrefix(server_resp *rp, long long ll, char prefix);
void addReplyBigNum(server_resp *rp, const char* num, size_t len);
void addReplyHumanLongDouble(server_resp *rp, long double d);
void addReplyLongLong(server_resp *rp, long long ll);
void addReplyArrayLen(server_resp *rp, long length);
void addReplyMapLen(server_resp *rp, long length);
void addReplySetLen(server_resp *rp, long length);
void addReplyAttributeLen(server_resp *rp, long length);
void addReplyPushLen(server_resp *rp, long length);
void addReplyHelp(server_resp *rp, const char **help);
void addReplyLen(server_resp *rp, const char *str,size_t len);
void addReply(server_resp *rp, const char *str);
#endif //RESP_PARSER_H_