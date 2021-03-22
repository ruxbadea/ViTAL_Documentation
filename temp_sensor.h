/*******************************************************************************
 * COPYRIGHT (C) VITESCO TECHNOLOGIES
 * ALL RIGHTS RESERVED.
 *
 * The reproduction, transmission or use of this document or its
 * contents is not permitted without express written authority.
 * Offenders will be liable for damages. All rights, including rights
 * created by patent grant or registration of a utility model or design,
 * are reserved.
 *******************************************************************************/

#ifndef COMPONENTS_VITAL_BSW_HAL_TEMP_SENSOR_H
#define COMPONENTS_VITAL_BSW_HAL_TEMP_SENSOR_H

#include "global.h"

typedef struct
{
	uint8_t u8IntegralHum;
	uint8_t u8DecimalHum;
	uint8_t u8IntegralTemp;
	uint8_t u8DecimalTemp;
	uint8_t u8CheckSum;
} DHT11_struct;

/*******************************************************************************
 *  Function name    : DHT11_vRequest
 *
 *  Description      : Send pulse to activate the sensor
 *
 *  List of arguments: -
 *
 *  Return value     : -
 *
 *******************************************************************************/
void DHT11_vRequest(void);

/*******************************************************************************
 *  Function name    : DHT11_i8Response
 *
 *  Description      : See if the sensor got the pulse
 *
 *  List of arguments: -
 *
 *  Return value     : int8_t -> 0 - OK; -1 for TIMEOUT
 *
 *******************************************************************************/
int8_t DHT11_i8Response(void);

/*******************************************************************************
 *  Function name    : DHT11_i8Receive
 *
 *  Description      : Get 8 bits of data from the sensor
 *
 *  List of arguments: -
 *
 *  Return value     : int8_t -> Data received; -1 for TIMEOUT
 *
 *******************************************************************************/
int8_t DHT11_i8Receive(void);

/*******************************************************************************
 *  Function name    : DHT11_dht11Read
 *
 *  Description      : Make a full operation on the sensor
 *
 *  List of arguments: -
 *
 *  Return value     : DHT11_t -> Data from sensor
 *
 *******************************************************************************/
DHT11_struct DHT11_dht11Read(void);

/*******************************************************************************
 *  Function name    : DHT11_vTaskTempAndHumCalculate
 *
 *  Description      : Calculate and update temp and hum
 *
 *  List of arguments: -
 *
 *  Return value     : -
 *
 *******************************************************************************/
void DHT11_vTaskTempAndHumCalculate(void);

/* Errors */
#define DHT11_TIMEOUT -1
#define DHT11_OK 0
/* Time limits from datasheet + aprox 5 us */
#define DHT11_RESPONSE_START_US 45
#define DHT11_RESPONSE_LOW_US 85
#define DHT11_RESPONSE_HIGH_US 85
#define DHT11_BIT_START_US 55
#define DHT11_BIT_0_US 28
#define DHT11_BIT_1_US 75

#endif
