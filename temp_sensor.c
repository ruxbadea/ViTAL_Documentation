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

#include "BSW/HAL/Temp_Sensor/temp_sensor.h"

#include "BSW/MCAL/GPIO/gpio.h"

#include "BSW/HAL/Com/com.h"

extern COM_GET_struct g_GET_DataStructure;

static const char *TAG = "HAL TEMP SENSOR";

static uint8_t u8Counter = 0;

void DHT11_vRequest(void)
{
	/* Set the direction to output for a new communication */
	GPIO_vSetDirection(DHT11_PIN, DIR_OUTPUT);

	/* H-L-H of at least 18ms */
	GPIO_vSetLevel(DHT11_PIN, HIGH_LEVEL);
	GPIO_vSetLevel(DHT11_PIN, LOW_LEVEL);
	ets_delay_us(20000);
	/* Set as input so 10k pull-up from circuit drives pin High */
	GPIO_vSetDirection(DHT11_PIN, DIR_INPUT);
}

int8_t DHT11_i8Response(void)
{
	u8Counter = 0;
	while (GPIO_iGetLevel(DHT11_PIN))
	{
		if (u8Counter >= DHT11_RESPONSE_START_US)
		{
			return DHT11_TIMEOUT;
		}
		u8Counter++;
		ets_delay_us(1);
	}

	u8Counter = 0;
	while (GPIO_iGetLevel(DHT11_PIN) == 0)
	{
		if (u8Counter >= DHT11_RESPONSE_LOW_US)
		{
			return DHT11_TIMEOUT;
		}
		u8Counter++;
		ets_delay_us(1);
	}

	u8Counter = 0;
	while (GPIO_iGetLevel(DHT11_PIN))
	{
		if (u8Counter >= DHT11_RESPONSE_HIGH_US)
		{
			return DHT11_TIMEOUT;
		}
		u8Counter++;
		ets_delay_us(1);
	}

	return DHT11_OK;
}

int8_t DHT11_i8Receive(void)
{
	uint8_t u8Index = 0;
	/* Sensor measures data in the range of 0-50 degrees and 0-100(max) humidity
	 * so it is safe to type cast uint to int or int to uint */
	uint8_t u8Data = 0;

	for (; u8Index < 8; u8Index++)
	{
		/* Get the common 0 level for both 0 and 1 bits */
		u8Counter = 0;
		while (GPIO_iGetLevel(DHT11_PIN) == 0)
		{
			if (u8Counter >= DHT11_BIT_START_US)
			{
				return DHT11_TIMEOUT;
			}
			u8Counter++;
			ets_delay_us(1);
		}

		/* Measure the bit value */
		u8Counter = 0;
		while (GPIO_iGetLevel(DHT11_PIN))
		{
			/* Max time of bit */
			if (u8Counter >= DHT11_BIT_1_US)
			{
				{
					return DHT11_TIMEOUT;
				}
			}
			u8Counter++;
			ets_delay_us(1);
		}

		/* Time is greater than a 0 so it must be a one */
		if (u8Counter > DHT11_BIT_0_US)
		{
			u8Data = (u8Data << 1) | (0x01);
		}
		else
		{
			u8Data = (u8Data << 1);
		}
	}

	return (int8_t) u8Data;
}

DHT11_struct DHT11_dht11Read(void)
{
	DHT11_struct dht11Data;

	/* Create and reset a data array */
	int8_t i8DHT11_array[5];
	uint8_t u8Index = 0;
	for (u8Index = 0; u8Index < 5; u8Index++)
	{
		i8DHT11_array[u8Index] = 0;
	}

	DHT11_vRequest();

	if (DHT11_i8Response() != DHT11_OK)
	{
		ESP_LOGI(TAG, "Response TIMEOUT!");
	}

	for (u8Index = 0; u8Index < 5; u8Index++)
	{
		i8DHT11_array[u8Index] = DHT11_i8Receive();

		if (i8DHT11_array[u8Index] == DHT11_TIMEOUT)
		{
			ESP_LOGI(TAG, "Data TIMEOUT!");
		}
	}

	/* Assign received data to structure */
	dht11Data.u8IntegralHum = i8DHT11_array[0];
	dht11Data.u8DecimalHum = i8DHT11_array[1];
	dht11Data.u8IntegralTemp = i8DHT11_array[2];
	dht11Data.u8DecimalTemp = i8DHT11_array[3];
	dht11Data.u8CheckSum = i8DHT11_array[4];

	if ((dht11Data.u8IntegralHum + dht11Data.u8DecimalHum
			+ dht11Data.u8IntegralTemp + dht11Data.u8DecimalTemp)
			!= dht11Data.u8CheckSum)
	{
		ESP_LOGI(TAG, "ERROR at checksum");
		dht11Data.u8CheckSum = 0;
	}

	return dht11Data;
}

void DHT11_vTaskTempAndHumCalculate(void)
{
	DHT11_struct dht11Data = DHT11_dht11Read();

	if ((dht11Data.u8IntegralTemp != (uint8_t) DHT11_TIMEOUT)
			&& (dht11Data.u8IntegralHum != (uint8_t) DHT11_TIMEOUT)
			&& (dht11Data.u8CheckSum != 0))
	{
		g_GET_DataStructure.u8Temperature = dht11Data.u8IntegralTemp;
		g_GET_DataStructure.u8Humidity = dht11Data.u8IntegralHum;
	}
	else
	{
		ESP_LOGI(TAG, "ERROR at COM");
	}

	ESP_LOGI(TAG, "Hum Int: %d", dht11Data.u8IntegralHum);
	ESP_LOGI(TAG, "Hum Dec: %d", dht11Data.u8DecimalHum);
	ESP_LOGI(TAG, "Temp Int: %d", dht11Data.u8IntegralTemp);
	ESP_LOGI(TAG, "Temp Dec: %d", dht11Data.u8DecimalTemp);
	ESP_LOGI(TAG, "Checksum: %d", dht11Data.u8CheckSum);
}
