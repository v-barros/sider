/*
 * utils.h
 *
 *  Created on: 2021-12-02
 *      Author: @v-barros
 */
#ifndef UTILS_H_
#define UTILS_H_


// return ascii character of n
// n must be in 0-9, otherwise return a space character (0x20)
char itochar(int n);

//convert c(ascii) to int equivalent
//c must be in 0-9 (0x30-0x39), otherwise return -1
int chartoi(char c);

// naive implementation of integers power function
int ipow(int base, int exp);

// n must be positive
// convert n to array of char in dest and return num of chars
// ex: 65535
// put "65535" on dest
// return 5
int toString(int n,char*dest);

#endif // UTILS_H_