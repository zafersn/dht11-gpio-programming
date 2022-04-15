/*
 * Maax-B-Test.h
 *
 *  Created on: 22 Dec 2021
 *      Author: ubu
 */

#ifndef MAAX_B_TEST_H_
#define MAAX_B_TEST_H_

#include <time.h>

#define IN   "in"
#define OUT  "out"
#define HIGH   "1"
#define LOW  "0"
#define GPIO13  "13"
#define GPIODHT11  "122"
#define GPIO4_IO23  "119"

#define USR_LED "usr_led"

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

typedef struct _err_ret
{
	int step;
	int ret_val;
}err_ret_typ;
static inline int millis()
{
int iTime;
struct timespec res;

    clock_gettime(CLOCK_MONOTONIC, &res);
    iTime = 1000*res.tv_sec + res.tv_nsec/1000000;

    return iTime;
} /* millis() */

static inline int micros()
{
int iTime;
struct timespec res;

    clock_gettime(CLOCK_MONOTONIC, &res);
    iTime = 1000000*res.tv_sec + res.tv_nsec/1000;

    return iTime-60;
} /* millis() */

#endif /* MAAX_B_TEST_H_ */
