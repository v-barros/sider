/*
 * utils.h
 *
 *  Created on: 2021-12-02
 *      Author: @v-barros
 */
#ifndef UTILS_H_
#define UTILS_H_
#include <stddef.h>
#include <limits.h>
#include <stdint.h>

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


/* Convert a long long into a string. Returns the number of
 * characters needed to represent the number.
 * If the buffer is not big enough to store the string, 0 is returned. */
int ll2string(char *dst, size_t dstlen, long long svalue);

/* Convert a unsigned long long into a string. Returns the number of
 * characters needed to represent the number.
 * If the buffer is not big enough to store the string, 0 is returned.
 *
 * Based on the following article (that apparently does not provide a
 * novel approach but only publicizes an already used technique):
 *
 * https://www.facebook.com/notes/facebook-engineering/three-optimization-tips-for-c/10151361643253920 */
 int ull2string(char *dst, size_t dstlen, unsigned long long value);

/* Return the number of digits of 'v' when converted to string in radix 10.
 * See ll2string() for more information. */
uint32_t digits10(uint64_t v);
#endif // UTILS_H_