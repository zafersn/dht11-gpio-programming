/*
 * Maax-B-Test.h
 *
 *  Created on: 22 Dec 2021
 *      Author: ubu
 */

#ifndef MAAX_B_TEST_H_
#define MAAX_B_TEST_H_
#include <stdint.h>
#include <time.h>
#include <glib.h>


//#define IN   "in"
//#define OUT  "out"
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

#define EXIT_FAILURE_C -1
#define EXIT_SUCCESS_C  0

#define GPIOD_TEST_CONSUMER "gpiod-test"

typedef struct _bitData
{
	int i;
	int j;
}Bit_d_typ;
typedef struct _err_ret
{
	int step;
	int ret_val;
	uint32_t timestamp[40];
	Bit_d_typ indx[40];
	uint32_t eror_cnt;
	uint32_t totl_measure;
	uint8_t percentage_Of_corraption;
}err_ret_typ;

typedef enum
{
	IN = 0,
	OUT,
}gpio_direction_typ;

struct gpiod_test_event_thread {
	GThread *id;
	GMutex lock;
	GCond cond;
	gboolean should_stop;
	guint chip_index;
	guint line_offset;
	guint period_ms;
};
typedef struct gpiod_test_event_thread GpiodTestEventThread;
typedef struct gpiod_chip gpiodChipstruct;
//
//static inline uint64_t millis()
//{
//	uint64_t iTime;
//struct timespec res;
//
//    clock_gettime(CLOCK_MONOTONIC, &res);
//    iTime =  res.tv_nsec/1000000;
//
//    return iTime;
//} /* millis() */
/**
 * @return milliseconds
 */
static inline uint32_t millis() {
  struct timespec spec;
  if (clock_gettime(CLOCK_MONOTONIC, &spec) == -1) { /* 1 is CLOCK_MONOTONIC */
	  printf("abort\n");
    abort();
  }

  return  spec.tv_nsec / 1e6;
}


static inline unsigned int micros()
{
unsigned int iTime;
struct timespec res;

    clock_gettime(CLOCK_MONOTONIC_RAW, &res);
    iTime = res.tv_nsec/1000;

    return iTime-60;
} /* millis() */

int EventGpiod();



/* Helpers for triggering line events in a separate thread. */

#endif /* MAAX_B_TEST_H_ */
