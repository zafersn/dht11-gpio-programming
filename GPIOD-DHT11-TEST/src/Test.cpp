//============================================================================
// Name        : Maax-B-C.cpp
// Author      : zafer
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <unistd.h>
#include "Maax-B-Test.h"

#include <gpiod.h>

#include <string.h>
//#include <linux/init.h>             // Macros used to mark up functions e.g., __init __exit
#include <linux/module.h>           // Core header for loading LKMs into the kernel
#include <linux/kernel.h>           // Contains types, macros, functions for the kernel

using namespace std;


#define DHT_DATA_BYTE_COUNT 5
#define DHT_BEGIN_RESPONSE_TIMEOUT_US	90  //all their sort are microsec
#define DHT_START_BIT_TIMEOUT_US	70
#define DHT_BIT_SET_DATA_DETECT_TIME_US 50
char dht11_byte[DHT_DATA_BYTE_COUNT];
uint32_t dht11_byte_t[DHT_DATA_BYTE_COUNT][8];
uint64_t start = 0, finish = 0, delta = 0;
int ReadRawDHTData();
err_ret_typ err_step;
double temperature = 0, humidity = 0;

struct timespec ts = { 0, 5000 };

void Gpio_Line_Realise(struct gpiod_line *line);
void Close_Chip(struct gpiod_chip * chipName);
int GPIO_SetOutPutPin(struct gpiod_line *line);
int GPIO_ResetOutPutPin(struct gpiod_line *line);
int GPIO_SetDirection_Request(gpio_direction_typ mode, struct gpiod_line *line,const char *consumer);
int GPIO_SetDirectionBYconfig(struct gpiod_line *line,struct gpiod_line_request_config *config);
int GPIO_SetDirection(gpio_direction_typ mode, struct gpiod_line *line);
int GPIO_IsInputPinSet(struct gpiod_line *line);
int ReadDHT_Data();
void polling_main();
void Cleanup();



int ret;
gpiodChipstruct *chip0 = NULL;
gpiodChipstruct *chip3 = NULL;
//const char *chipname3 = "gpiochip3";

struct gpiod_line *lineRed;    // Red LED  gpiochip0
struct gpiod_line *dht11;	//GPIO4_IO26	//gpiochip3
struct gpiod_line *testP; //GPIO4_IO23 //gpiochip3

unsigned int redLed_line_num = 13;
unsigned int dht11_line_num = 26;
unsigned int testP_line_num = 23;

int main() {

	polling_main();
	return 0;
}


int ReadDHT_Data()
{
		int rv = ReadRawDHTData();
		err_step.totl_measure++;
		if (rv != 0)
		{
			err_step.eror_cnt++;
			return rv;
		}

		// CONVERT AND STOREpolling_main
		humidity    = dht11_byte[0];  // bit[1] == 0;
		temperature = dht11_byte[2];  // bits[3] == 0;

		// TEST CHECKSUM
		uint8_t sum = dht11_byte[0] + dht11_byte[1] + dht11_byte[2] + dht11_byte[3]; // bits[1] && bits[3] both 0
		if (dht11_byte[4] != sum)
		{
			temperature = humidity = -1;
			return -1;
		}
		return 0;
}
/**
  * @brief  read data from dht11 sensor.
  * @param  None
  * @retval :
  *  0 : OK
  *  -1: failure
  *  -2 : timeout
  */
int ReadRawDHTData()
{
	//we notified that we want to start com.
	int ret  = 0;	//struct gpiod_line_request_config config;
	err_step.step = 0;err_step.ret_val = 0;
//	memset(&err_step,0,sizeof(err_step)-(2*sizeof(uint32_t)+sizeof(uint8_t)));
	if (gpiod_line_direction(dht11) == GPIOD_LINE_DIRECTION_INPUT || gpiod_line_consumer(dht11) == NULL)
	 {
//		config.request_type = GPIOD_LINE_REQUEST_DIRECTION_OUTPUT;
//		config.consumer = GPIOD_TEST_CONSUMER;
//		ret = GPIO_SetDirectionBYconfig(dht11, &config);
		ret = GPIO_SetDirection(OUT, dht11);
	 }
	err_step.ret_val = 0;
	err_step.step = 0;
//	GPIO_ResetOutPutPin(testP);
	if(ret == EXIT_FAILURE_C)
	{
		err_step.step = 1;
		err_step.ret_val = -1;
		return -1;
	}
	if(GPIO_ResetOutPutPin(dht11) == EXIT_FAILURE_C)
	{
		err_step.step = 2;
		err_step.ret_val = -1;
		Gpio_Line_Realise(dht11);

		return -1;
	}
	usleep(18000);

	if(GPIO_SetOutPutPin(dht11) == EXIT_FAILURE_C)
	{
		err_step.step = 3;
		err_step.ret_val = -1;
		return -1;
	}

	//********************************

	/*****************low(80us) => high (80us) ***************** response */
	 GPIO_SetOutPutPin(testP);
//	 config.request_type = GPIOD_LINE_REQUEST_DIRECTION_INPUT;
//	config.consumer = GPIOD_TEST_CONSUMER;
//	if(GPIO_SetDirectionBYconfig(dht11, &config) == EXIT_FAILURE_C)
	 if(GPIO_SetDirection(IN, dht11) == EXIT_FAILURE_C)
	{
		err_step.step = 4;
		err_step.ret_val = -1;
		return -1;
	}

    GPIO_ResetOutPutPin(testP);
    start = micros();

	while(!GPIO_IsInputPinSet(dht11 ))
	{
		delta = micros()  - start;
		if(delta > DHT_BEGIN_RESPONSE_TIMEOUT_US)
		{
			err_step.step = 6;
			err_step.ret_val = -2;
			return -2;
		}
	}

//		GPIO_ResetOutPutPin(testP);
		err_step.ret_val = 10;

    start = micros();
//    GPIO_SetOutPutPin(testP);

	while (GPIO_IsInputPinSet(dht11))
	{
		delta = micros()  - start;

		if(delta > DHT_BEGIN_RESPONSE_TIMEOUT_US)
		{
			err_step.step = 8;
			err_step.ret_val = -2;
			return -2;
		}
	}

//		GPIO_ResetOutPutPin(testP);
		err_step.ret_val = 10;


		int is = 0;
	//********************************************************* Start reading data bit by low level (50us) ***************************
	for (int i = 0; i < DHT_DATA_BYTE_COUNT ; i++)
	{
		for (int J = 7; J > -1; J--)
		{
//			 GPIO_SetOutPutPin(testP);
			 int ret;
			 start = micros();
			 do
			 {
				 ret = GPIO_IsInputPinSet(dht11);
				 delta = micros() - start;
				 if(ret == -1)
				 {
					 err_step.step = 9;
					err_step.ret_val = -1;
					return -1;
				 }
				if(delta > DHT_START_BIT_TIMEOUT_US)
				{
					err_step.step = 10;
					err_step.ret_val = -2;
					err_step.timestamp[is] = delta;
					err_step.indx[is].i = i;
					err_step.indx[is++].j = J;
//					GPIO_ResetOutPutPin(testP);
					return -2;
				}
			 }while(ret == 0);


//				GPIO_ResetOutPutPin(testP);
				err_step.ret_val = 10;


//			    GPIO_SetOutPutPin(testP);

		    start = micros();
		    do
			 {
				 ret = GPIO_IsInputPinSet(dht11);
				 delta = micros() - start;
				 if(ret == -1)
				 {
					 err_step.step = 11;
					err_step.ret_val = -1;
					return -1;
				 }
				 if(delta > DHT_BEGIN_RESPONSE_TIMEOUT_US)
				{
					err_step.step = 12;
					err_step.ret_val = -2;
					err_step.timestamp[is] = delta;
					err_step.indx[is].i = i;
					err_step.indx[is++].j = J;
					return -2;
				}
			 }while(ret == 1);

					err_step.timestamp[is] = delta;
					err_step.indx[is].i = i;
					err_step.indx[is++].j = J;
//				GPIO_ResetOutPutPin(testP);
				err_step.ret_val = 10;

			(delta > DHT_BIT_SET_DATA_DETECT_TIME_US) ? bitWrite(dht11_byte[i],J,1) : bitWrite(dht11_byte[i],J,0);
		}
	}

	return 0;
}


void Cleanup()
{
	Gpio_Line_Realise(lineRed);
	Gpio_Line_Realise(testP);
	Gpio_Line_Realise(dht11);
	Close_Chip(chip0);
	Close_Chip(chip3);
}
int GPIO_ResetOutPutPin(struct gpiod_line *line)
{
	return gpiod_line_set_value(line, 0);
}
int GPIO_SetOutPutPin(struct gpiod_line *line)
{
	return gpiod_line_set_value(line, 1);
}
int GPIO_SetDirection_Request(gpio_direction_typ mode, struct gpiod_line *line,const char *consumer)
{
	switch (mode)
	{
	case IN:
		return  gpiod_line_request_input(line, consumer);
		break;
	case OUT:
		return  gpiod_line_request_output(line, consumer, 1);
		break;
	}
	return -1;
}
int GPIO_SetDirectionBYconfig(struct gpiod_line *line,struct gpiod_line_request_config *config)
{
	if(gpiod_line_is_used(line))
	{
		Gpio_Line_Realise(line);
	}
	return gpiod_line_request(line, config, 1);
}
int GPIO_SetDirection(gpio_direction_typ mode, struct gpiod_line *line)
{
	if(gpiod_line_is_used(line))
	{
		Gpio_Line_Realise(line);
	}
	return GPIO_SetDirection_Request(mode, line, GPIOD_TEST_CONSUMER);

//	switch (mode)
//	{
//	case IN:
//		return  gpiod_line_set_direction_input(line);
//		break;
//	case OUT:
//		return  gpiod_line_set_direction_output(line, 0);
//		break;
//	}
	return -1;
}

int GPIO_IsInputPinSet(struct gpiod_line *line)
{
	return gpiod_line_get_value(line);
}
void Close_Chip(struct gpiod_chip * chipName)
{
	gpiod_chip_unref(chipName);
	 // gpiod_chip_close(chipName);

}
void Gpio_Line_Realise(struct gpiod_line *line)
{
	gpiod_line_release(line);
}

void polling_main()
{
	/*
		 * GPIO4_IO23
		 * GPIO4_IO26
		 *
		 * */


		  // Open GPIO chip
			chip0 = gpiod_chip_open("/dev/gpiochip0");
			chip3 = gpiod_chip_open("/dev/gpiochip3");


		   // Open GPIO lines
		   lineRed = gpiod_chip_get_line(chip0, redLed_line_num);
		   dht11 = gpiod_chip_get_line(chip3, dht11_line_num);
		   testP = gpiod_chip_get_line(chip3, testP_line_num);
		    if(!lineRed)
		    {
		    	printf("errLine");
		    	Close_Chip(chip0);
		    }
		    if(!dht11)
			{
				printf("errLine");
				Close_Chip(chip3);
			}
			ret = gpiod_line_request_output(lineRed, "ex1", 0);
			if(ret != 0)
			{
				Gpio_Line_Realise(lineRed);
				printf("err request");
			}
			ret = gpiod_line_request_output(dht11, GPIOD_TEST_CONSUMER, 1);
			if(ret != 0)
			{
				Gpio_Line_Realise(dht11);
				printf("err request");
			}
		//	GPIO_SetOutPutPin(dht11);
			ret = GPIO_SetDirection_Request(OUT, testP, "EX3");//gpiod_line_request_output(testP, "ex3", 0);
			if(ret != 0)
			{
				Gpio_Line_Realise(testP);
				printf("err request");
			}
			bool val = 0;
			while(1)
			{
//				ret = gpiod_line_set_value(lineRed, val);
	//			gpiod_line_set_value(dht11, val);
	//			gpiod_line_set_value(testP, val);
//				if (ret < 0) {
//					perror("Set line output failed\n");
//					Gpio_Line_Realise(lineRed);
//				}
//				printf("Output %u on line #%u\n", val, redLed_line_num);
				sleep(1);
//				val = !val;

				ReadDHT_Data();
				err_step.percentage_Of_corraption = err_step.eror_cnt * 100 / err_step.totl_measure;

			}
}

