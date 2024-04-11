/*
Library:				DHT22 - AM2302 Temperature and Humidity Sensor
Written by:				LF
Date Written:			21/11/2018
Last modified:		    -/-
Description:			This is an STM32 device driver library for the DHT22 Temperature and Humidity Sensor, using STM HAL libraries

References:				This library was written based on the DHT22 datasheet
						- https://cdn-shop.adafruit.com/datasheets/Digital+humidity+and+temperature+sensor+AM2302.pdf
										
*/

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"

#define	DHT22_ENABLE
#ifdef DHT22_ENABLE

#include "dht22.h"

extern	void DWT_Delay_us(uint32_t au32_microseconds);

//Bit fields manipulations
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

//1. One wire data line
static GPIO_TypeDef* oneWire_PORT;
static uint16_t oneWire_PIN;
static uint8_t oneWirePin_Idx;

//*** Functions prototypes ***//
//OneWire Initialise
void DHT22_Init(GPIO_TypeDef* DataPort, uint16_t DataPin)
{
	oneWire_PORT = DataPort;
	oneWire_PIN = DataPin;
	for(uint8_t i=0; i<16; i++)
	{
		if(DataPin & (1 << i))
		{
			oneWirePin_Idx = i;
			break;
		}
	}
}

//Change pin mode
static void ONE_WIRE_PinMode(OnePinMode_Typedef mode)
{
#ifdef OLD
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = oneWire_PIN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	
	if(mode == ONE_OUTPUT)
	{
//		oneWire_PORT->MODER &= ~(3UL << 2*oneWirePin_Idx);  //Reset State
//		oneWire_PORT->MODER |= (0x01 << 2*oneWirePin_Idx); //Output Mode
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	}
	else if(mode == ONE_INPUT)
	{
//		oneWire_PORT->MODER &= ~(3UL << 2*oneWirePin_Idx);  //Input Mode
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;

	}
	
	HAL_GPIO_Init(oneWire_PORT, &GPIO_InitStruct);
#else
	if(mode == ONE_OUTPUT)
		oneWire_PORT->MODER &= ~((1<<oneWire_PIN) | (1<<(oneWire_PIN+1)));  // pin Port as input (00 shifted for bit)
	else if(mode == ONE_INPUT)
	{
		oneWire_PORT->MODER &= ~((1<<oneWire_PIN) | (1<<(oneWire_PIN+1)));	// pin Port as input (00 shifted for bit)
		oneWire_PORT->MODER |= (1<<oneWire_PIN);  							// pin Port as output (01 shifted for bit)
	}
	else
		return;
#endif

}	
//One Wire pin HIGH/LOW Write
static void ONE_WIRE_Pin_Write(bool state)
{
	if(state)
		HAL_GPIO_WritePin(oneWire_PORT, oneWire_PIN, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(oneWire_PORT, oneWire_PIN, GPIO_PIN_RESET);
}
static bool ONE_WIRE_Pin_Read(void)
{
	return HAL_GPIO_ReadPin(oneWire_PORT, oneWire_PIN) & 0x01;
}

//DHT Begin function
static void DHT22_StartAcquisition(void)
{
	//Change data pin mode to OUTPUT
	ONE_WIRE_PinMode(ONE_OUTPUT);
	//Put pin LOW
	ONE_WIRE_Pin_Write(0);
	//500uSec delay
	//DelayMicroSeconds(500);
	DWT_Delay_us(500);
	//Bring pin HIGH
	ONE_WIRE_Pin_Write(1);
	//30 uSec delay
	//DelayMicroSeconds(30);
	DWT_Delay_us(30);
	//Set pin as input
	ONE_WIRE_PinMode(ONE_INPUT);
}
//Read 5 bytes
uint32_t	dht_timeout;
static void DHT22_ReadRaw(uint8_t *data)
{
	uint32_t rawBits = 0UL;
	uint8_t checksumBits=0;
	
	//DelayMicroSeconds(40);
	DWT_Delay_us(40);
	dht_timeout = 0;
	while(!ONE_WIRE_Pin_Read() )
	{
		DWT_Delay_us(10);
		dht_timeout++;
		if ( dht_timeout > 100)
			return ;
	}
	while(ONE_WIRE_Pin_Read())
	{
		DWT_Delay_us(10);
		dht_timeout++;
		if ( dht_timeout > 100)
			return ;
	}
	for(int8_t i=31; i>=0; i--)
	{
		while(!ONE_WIRE_Pin_Read())
		{
			DWT_Delay_us(10);
			dht_timeout++;
			if ( dht_timeout > 100)
				return ;
		}
		//DelayMicroSeconds(40);
		DWT_Delay_us(40);

		if(ONE_WIRE_Pin_Read())
		{
			rawBits |= (1UL << i);
		}
		while(ONE_WIRE_Pin_Read());
	}
	
	for(int8_t i=7; i>=0; i--)
	{
		while(!ONE_WIRE_Pin_Read())
		{
			DWT_Delay_us(10);
			dht_timeout++;
			if ( dht_timeout > 100)
				return ;
		}
		//DelayMicroSeconds(40);
		DWT_Delay_us(40);
		if(ONE_WIRE_Pin_Read())
		{
			checksumBits |= (1UL << i);
		}
		while(ONE_WIRE_Pin_Read());
	}
	
	
	//Copy raw data to array of bytes
	data[0] = (rawBits>>24)&0xFF;
	data[1] = (rawBits>>16)&0xFF;
	data[2] = (rawBits>>8)&0xFF;
	data[3] = (rawBits>>0)&0xFF;
	data[4] = (checksumBits)&0xFF;
}

//Get Temperature and Humidity data
uint8_t DHT22_GetTemp_Humidity(float *Temp, float *Humidity)
{
	uint8_t dataArray[6], myChecksum;
	uint16_t Temp16, Humid16;
	//Implement Start data Aqcuisition routine
	DHT22_StartAcquisition();
	//Aqcuire raw data
	DHT22_ReadRaw(dataArray);
	//calculate checksum
	myChecksum = 0;
	for(uint8_t k=0; k<4; k++) 
	{
		myChecksum += dataArray[k];
	}
	if(myChecksum == dataArray[4])
	{
		Temp16 = (dataArray[2] <<8) | dataArray[3];
		Humid16 = (dataArray[0] <<8) | dataArray[1];
		
		*Temp = Temp16/10.0f;
		*Humidity = Humid16/10.0f;
		return 1;
	}
	return 0;
}

#endif // #ifdef DHT22_ENABLE
