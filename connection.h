/*
 * connection.h
 *
 *  Created on: 2023-05-23
 *      Author: @v-barros
 */
#ifndef CONNECTION_H_
#define CONNECTION_H_


// set up new NON BLOCKING listening socket using port and return socketfd
int init_conn(int port);

#endif // CONNECTION_H_