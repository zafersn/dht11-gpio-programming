//============================================================================
// Name        : Maax-B-Test.cpp
// Author      : ZAFER
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "fcntl.h"  //define O_WRONLY and O_RDONLY
#include "Maax-B-Test.h"
#include "stdint.h"

//Chip reset pin: P1_16
#define SYSFS_GPIO_EXPORT          "/sys/class/gpio/export"
#define SYSFS_GPIO_RST_PIN_VAL      "13"
#define SYSFS_GPIO_RST_DIR          "/sys/class/gpio/gpio13/direction"
#define SYSFS_GPIO_RST_DIR_VAL      "OUT"
#define SYSFS_GPIO_RST_VAL          "/sys/class/gpio/gpio13/value"
#define SYSFS_GPIO_UNEXPORT          "/sys/class/gpio/unexport"

#define SYSFS_GPIO_RST_VAL_H        "1"
#define SYSFS_GPIO_RST_VAL_L        "0"

#define LED_USR_VAL "/sys/class/leds/usr_led/brightness"

double temperature = 0, humidity = 0;
int readValPin = -1;
int ReadDHT_Data(int fd);
int Init_DHT11(int fd);


int GPIO_ExportPin(int fd, const void* pinNumber )
{

	fd = open(SYSFS_GPIO_EXPORT, O_WRONLY);
	if(fd < 0)
	{
		printf("ERR: Radio hard reset pin%s open error.\n",(char *)pinNumber);
		return EXIT_FAILURE;
	}


	write(fd, pinNumber ,sizeof(pinNumber));

	close(fd);
	return EXIT_SUCCESS;

}
int GPIO_SetDirection(int fd, const void* pinDir, const void* gpioPinNum)
{
	//Set the port direction / sys / class / GPIO / gpio48 echo out > direction
	char gpio_path[255];
	sprintf(gpio_path,"/sys/class/gpio/gpio%s/direction",(char *)gpioPinNum);
	//printf("s: %s\n",gpio_path);
	do
	{
		fd = open(gpio_path, O_WRONLY);

		if(fd < 0)
		{
			printf("ERR: Radio hard reset pin%s direction open error.\n",(char *)gpioPinNum);
			return EXIT_FAILURE;
		}
	}while(fd == -1);
	write(fd, pinDir, sizeof(pinDir));
	close(fd);
	return EXIT_SUCCESS;
}
int GPIO_SetValue(int fd, const void* gpioPinNum, const void* pinVal,int isUserLed)
{
	//Output reset signal: pull up > 100ns
	///sys/class/gpio/gpio13/value
	char gpio_path[255];
	if(isUserLed)
		sprintf(gpio_path,"/sys/class/leds/%s/brightness",(char *)gpioPinNum);
	else
		sprintf(gpio_path,"/sys/class/gpio/gpio%s/value",(char *)gpioPinNum);
	fd = open(gpio_path, O_RDWR);
	if(fd < 0)

	{
		printf("ERR: Radio hard reset pin%s value open error.\n",(char *)gpioPinNum);
		return EXIT_FAILURE;
	}
	write(fd, pinVal, sizeof(pinVal));
	close(fd);
	return EXIT_SUCCESS;
}
int GPIO_ResetOutPutPin(int fd, const void* gpioPinNum)
{
	char gpio_path[255];
	sprintf(gpio_path,"/sys/class/gpio/gpio%s/value",(char *)gpioPinNum);
	fd = open(gpio_path, O_RDWR);
	if(fd < 0)

	{
		printf("ERR: Radio hard reset pin%s value open error.\n",(char *)gpioPinNum);
		return EXIT_FAILURE;
	}
	write(fd, LOW, sizeof(LOW));
	close(fd);
	return EXIT_SUCCESS;
}
int GPIO_SetOutPutPin(int fd, const void* gpioPinNum)
{
	char gpio_path[255];
	sprintf(gpio_path,"/sys/class/gpio/gpio%s/value",(char *)gpioPinNum);
	fd = open(gpio_path, O_RDWR);
	if(fd < 0)

	{
		printf("ERR: Radio hard reset pin%s value open error.\n",(char *)gpioPinNum);
		return EXIT_FAILURE;
	}
	write(fd, HIGH, sizeof(HIGH));
	close(fd);
	return EXIT_SUCCESS;
}
int GPIO_IsInputPinSet(int fd, const void* gpioPinNum,bool *retVal)
{
	  char path[64];

	    char value_str[3];

	    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/value",(char *) gpioPinNum);

	    fd = open(path, O_RDONLY);

	    if (fd < 0) {
	    	printf("Failed to open gpio%s value for reading!\n",(char *)gpioPinNum);
	    	//close(fd);
	        return EXIT_FAILURE;

	    }

	    if (read(fd, value_str, 3) < 0) {

	    	printf("Failed to read value!\n");

	        return EXIT_FAILURE;

	    }
	    //close(fd);
	    readValPin = *retVal = atoi(value_str);
	    return EXIT_SUCCESS;
}
int UNexport_GPIO_Pin(int fd, const void* pinNumber )
{
	fd = open(SYSFS_GPIO_UNEXPORT, O_WRONLY);
	if(fd == -1)
	{
		printf("ERR: Radio hard reset pin%s open error.\n",(char *)pinNumber);
		return EXIT_FAILURE;
	}
	else
	{
		return EXIT_SUCCESS;
	}

	write(fd, pinNumber ,sizeof(pinNumber));

	close(fd);
}

void GPIO_ProgrammingBYSysFile()
{
	int fd = -1,fd_l = -1,fd_d = -1;
		if(GPIO_ExportPin(fd, GPIO13) == EXIT_SUCCESS)
		{
			GPIO_SetDirection(fd, OUT,GPIO13);
		}
		if(GPIO_ExportPin(fd, GPIO4_IO23) == EXIT_SUCCESS)
		{
			GPIO_SetDirection(fd, OUT,GPIO4_IO23);
		}
		if(Init_DHT11(fd_d) == EXIT_FAILURE)
		{
			printf("ERR: dht11 could not be imported.\n");
			return ;
		}

	    while(1)
	    {

	    	GPIO_SetValue(fd, GPIO13, HIGH,0);
	    	GPIO_SetValue(fd_l, USR_LED, HIGH,1);
	        usleep(1000000);
	        GPIO_SetValue(fd, GPIO13, LOW,0);
	        GPIO_SetValue(fd_l, USR_LED, LOW,1);

	        usleep(1000000);
	        ReadDHT_Data(fd_d);

	    }
}

int main()
{
	GPIO_ProgrammingBYSysFile();
    	printf("ERR: program was stopped\n");

    return 0;
}

int Init_DHT11(int fd)
{
	int ret = GPIO_ExportPin(fd, GPIODHT11);
	if(ret == EXIT_SUCCESS)
	{
		ret = GPIO_SetDirection(fd, OUT,GPIODHT11);
		return ret;
	}
	return EXIT_FAILURE;
}
#define DHT_DATA_BYTE_COUNT 5
#define DHT_BEGIN_RESPONSE_TIMEOUT_US	290  //all their sort are microsec
#define DHT_START_BIT_TIMEOUT_US	60
#define DHT_BIT_SET_DATA_DETECT_TIME_US 50
char dht11_byte[DHT_DATA_BYTE_COUNT];
uint32_t dht11_byte_t[DHT_DATA_BYTE_COUNT][8];
int start = 0, finish = 0, delta = 0;
int ReadRawDHTData(int fd);
err_ret_typ err_step;
bool isGpio_set = false;

int ReadDHT_Data(int fd)
{
		int rv = ReadRawDHTData(fd);
		if (rv != 0)
		{
			return rv;
		}

		// CONVERT AND STORE
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
int ReadRawDHTData(int fd)
{
	//we notified that we want to start com.

	int ret = GPIO_SetDirection(fd, OUT,GPIODHT11);
	err_step.ret_val = 0;
	err_step.step = 0;
	GPIO_ResetOutPutPin(fd, GPIO4_IO23);
	if(ret == EXIT_FAILURE)
	{
		err_step.step = 1;
		err_step.ret_val = -1;
		return -1;
	}
	if(GPIO_ResetOutPutPin(fd, GPIODHT11))
	{
		err_step.step = 2;
		err_step.ret_val = -1;
		return -1;
	}
	usleep(18000);

	if(GPIO_SetOutPutPin(fd, GPIODHT11))
	{
		err_step.step = 3;
		err_step.ret_val = -1;
		return -1;
	}
	usleep(40);
	//********************************

	/*****************low(80us) => high (80us) ***************** response */

	if(GPIO_SetDirection(fd, IN,GPIODHT11))
	{
		err_step.step = 4;
		err_step.ret_val = -1;
		return -1;
	}

	isGpio_set = false;
    GPIO_SetOutPutPin(fd, GPIO4_IO23);
    start = micros();
    do
    {
    	if(GPIO_IsInputPinSet(fd, GPIODHT11,&isGpio_set) )
    	{
    		err_step.step = 5;
			err_step.ret_val = -1;
    		return -1;
    	}
		delta = micros()  - start;
		if(isGpio_set)
		{
			GPIO_ResetOutPutPin(fd, GPIO4_IO23);
			err_step.ret_val = 10;
		}
		if(delta > DHT_BEGIN_RESPONSE_TIMEOUT_US)
		{
			err_step.step = 6;
			err_step.ret_val = -2;
			return -2;
		}
    }
	while(!isGpio_set);

    start = micros();
    GPIO_SetOutPutPin(fd, GPIO4_IO23);
	do
	{
		if(GPIO_IsInputPinSet(fd, GPIODHT11,&isGpio_set))
		{
			err_step.step = 7;
			err_step.ret_val = -1;
			return -1;
		}
		delta = micros()  - start;
		if(!isGpio_set)
		{
			GPIO_ResetOutPutPin(fd, GPIO4_IO23);
			err_step.ret_val = 10;
		}
		if(delta > DHT_BEGIN_RESPONSE_TIMEOUT_US)
		{
			err_step.step = 8;
			err_step.ret_val = -2;
			return -2;
		}
	}
   	while(isGpio_set);


	//********************************************************* Start reading data bit by low level (50us) ***************************
	for (int i = 0; i < DHT_DATA_BYTE_COUNT ; i++)
	{
		for (int J = 7; J > -1; J--)
		{
			 start = micros();
			 GPIO_SetOutPutPin(fd, GPIO4_IO23);
			do
			{
				if(GPIO_IsInputPinSet(fd, GPIODHT11,&isGpio_set))
				{
					err_step.step = 9;
					err_step.ret_val = -1;
					return -1;
				}
				delta = micros() - start;
				if(isGpio_set)
				{
					GPIO_ResetOutPutPin(fd, GPIO4_IO23);
					err_step.ret_val = 10;
				}
				if(delta > DHT_START_BIT_TIMEOUT_US)
				{
					err_step.step = 10;
					err_step.ret_val = -2;
					return -2;
				}
			}
			while(!isGpio_set);

		    start = micros();
		    GPIO_SetOutPutPin(fd, GPIO4_IO23);
			do
			{
				if(GPIO_IsInputPinSet(fd, GPIODHT11,&isGpio_set))
				{
					err_step.step = 11;
					err_step.ret_val = -1;
					return -1;
				}
				delta = micros() - start;
				if(!isGpio_set)
				{
					GPIO_ResetOutPutPin(fd, GPIO4_IO23);
					err_step.ret_val = 10;
				}
				if(delta > DHT_BEGIN_RESPONSE_TIMEOUT_US)
				{
					err_step.step = 12;
					err_step.ret_val = -2;
					return -2;
				}
			}
			while(isGpio_set);

			(delta > DHT_BIT_SET_DATA_DETECT_TIME_US) ? bitWrite(dht11_byte[i],J,1) : bitWrite(dht11_byte[i],J,0);
		}
	}

	return 0;
}
