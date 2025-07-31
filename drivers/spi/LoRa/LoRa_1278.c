/* 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Project : A_os
*/
/*
 * LoRa_1278.c
 *
 *  Created on: Jul 31, 2025
 *      Author: fil
 *      Rework of https://github.com/SMotlaq/LoRa/tree/master
 *      Credits : https://github.com/SMotlaq/LoRa/tree/master
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"

#ifdef A_OS_SPI_ENABLED

#include "LoRa.h"
#include "LoRa_1278.h"
#include <string.h>

extern	LORA_Drv_TypeDef			*lora_Drv;
void LoRa_1278_CSLow(void)
{
	HAL_GPIO_WritePin(lora_Drv->CS_port, (int)lora_Drv->CS_bit, GPIO_PIN_RESET);
}

void LoRa_1278_CSHigh(void)
{
	HAL_GPIO_WritePin(lora_Drv->CS_port, (int)lora_Drv->CS_bit, GPIO_PIN_SET);
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_1278_1278_reset

		description : reset module

		arguments   :
			LoRa* LoRa --> LoRa object handler

		returns     : Nothing
\* ----------------------------------------------------------------------------- */
void LoRa_1278_reset(void)
{
	LoRa_1278_CSHigh();
	HAL_GPIO_WritePin(lora_Drv->RESET_port,lora_Drv->RESET_bit, GPIO_PIN_RESET);
	HAL_Delay(50);
	HAL_GPIO_WritePin(lora_Drv->RESET_port, lora_Drv->RESET_bit, GPIO_PIN_SET);
	HAL_Delay(100);
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_gotoMode

		description : set LoRa Op mode

		arguments   :
			LoRa* LoRa    --> LoRa object handler
			mode	        --> select from defined modes

		returns     : Nothing
\* ----------------------------------------------------------------------------- */
void LoRa_1278_gotoMode(int mode)
{
	uint8_t    read;
	uint8_t    data;

	read = LoRa_1278_read( RegOpMode);
	data = read;

	if(mode == SLEEP_MODE){
		data = (read & 0xF8) | 0x00;
		lora_Drv->current_mode = SLEEP_MODE;
	}else if (mode == STNBY_MODE){
		data = (read & 0xF8) | 0x01;
		lora_Drv->current_mode = STNBY_MODE;
	}else if (mode == TRANSMIT_MODE){
		data = (read & 0xF8) | 0x03;
		lora_Drv->current_mode = TRANSMIT_MODE;
	}else if (mode == RXCONTIN_MODE){
		data = (read & 0xF8) | 0x05;
		lora_Drv->current_mode = RXCONTIN_MODE;
	}else if (mode == RXSINGLE_MODE){
		data = (read & 0xF8) | 0x06;
		lora_Drv->current_mode = RXSINGLE_MODE;
	}

	LoRa_1278_write( RegOpMode, data);
	//HAL_Delay(10);
}
void LoRa_1278_setModeStandby(void)
{
	LoRa_1278_gotoMode(STNBY_MODE);
}
void LoRa_1278_setModeReceive(void)
{
	LoRa_1278_gotoMode(RXCONTIN_MODE);
}

uint8_t LoRa_1278_getstatus(void)
{
	return 0;
}
/* ----------------------------------------------------------------------------- *\
		name        : LoRa_1278_readReg

		description : read a register(s) by an address and a length,
									then store value(s) at outpur array.
		arguments   :
			LoRa* LoRa        --> LoRa object handler
			uint8_t* address  -->	pointer to the beginning of address array
			uint16_t r_length -->	detemines number of addresse bytes that
														you want to send
			uint8_t* output		--> pointer to the beginning of output array
			uint16_t w_length	--> detemines number of bytes that you want to read

		returns     : Nothing
\* ----------------------------------------------------------------------------- */
void LoRa_1278_readReg( uint8_t* address, uint16_t r_length, uint8_t* output, uint16_t w_length)
{
	LoRa_1278_CSLow();
	HAL_SPI_Transmit(lora_Drv->spi, address, r_length, TRANSMIT_TIMEOUT);
	while (HAL_SPI_GetState(lora_Drv->spi) != HAL_SPI_STATE_READY)
		;
	HAL_SPI_Receive(lora_Drv->spi, output, w_length, RECEIVE_TIMEOUT);
	while (HAL_SPI_GetState(lora_Drv->spi) != HAL_SPI_STATE_READY)
		;
	LoRa_1278_CSHigh();
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_1278_writeReg

		description : write a value(s) in a register(s) by an address

		arguments   :
			LoRa* LoRa        --> LoRa object handler
			uint8_t* address  -->	pointer to the beginning of address array
			uint16_t r_length -->	detemines number of addresse bytes that
														you want to send
			uint8_t* output		--> pointer to the beginning of values array
			uint16_t w_length	--> detemines number of bytes that you want to send

		returns     : Nothing
\* ----------------------------------------------------------------------------- */
void LoRa_1278_writeReg(uint8_t* address, uint16_t r_length, uint8_t* values, uint16_t w_length)
{
	LoRa_1278_CSLow();
	HAL_SPI_Transmit(lora_Drv->spi, address, r_length, TRANSMIT_TIMEOUT);
	while (HAL_SPI_GetState(lora_Drv->spi) != HAL_SPI_STATE_READY)
		;
	HAL_SPI_Transmit(lora_Drv->spi, values, w_length, TRANSMIT_TIMEOUT);
	while (HAL_SPI_GetState(lora_Drv->spi) != HAL_SPI_STATE_READY)
		;
	LoRa_1278_CSHigh();
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_1278_setLowDaraRateOptimization

		description : set the LowDataRateOptimization flag. Is is mandated for when the symbol length exceeds 16ms.

		arguments   :
			LoRa*	LoRa         --> LoRa object handler
			uint8_t	value        --> 0 to disable, otherwise to enable

		returns     : Nothing
\* ----------------------------------------------------------------------------- */
void LoRa_1278_setLowDaraRateOptimization( uint8_t value){
	uint8_t	data;
	uint8_t	read;

	read = LoRa_1278_read(RegModemConfig3);

	if(value)
		data = read | 0x08;
	else
		data = read & 0xF7;

	LoRa_1278_write( RegModemConfig3, data);
	HAL_Delay(10);
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_1278_setAutoLDO

		description : set the LowDataRateOptimization flag automatically based on the symbol length.

		arguments   :
			LoRa*	LoRa         --> LoRa object handler

		returns     : Nothing
\* ----------------------------------------------------------------------------- */
void LoRa_1278_setAutoLDO(void)
{
	double BW[] = {7.8, 10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125.0, 250.0, 500.0};

	LoRa_1278_setLowDaraRateOptimization( (long)((1 << lora_Drv->spredingFactor) / ((double)BW[lora_Drv->bandWidth])) > 16.0);
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_1278_setFrequency

		description : set carrier frequency e.g 433 MHz

		arguments   :
			LoRa* LoRa        --> LoRa object handler
			int   freq        --> desired frequency in MHz unit, e.g 434

		returns     : Nothing
\* ----------------------------------------------------------------------------- */
void LoRa_1278_setFrequency( uint32_t frequency)
{
	uint8_t  data;
	uint32_t F;
	F = (frequency * 524288)>>5;

	// write Msb:
	data = F >> 16;
	LoRa_1278_write( RegFrMsb, data);
	HAL_Delay(5);

	// write Mid:
	data = F >> 8;
	LoRa_1278_write( RegFrMid, data);
	HAL_Delay(5);

	// write Lsb:
	data = F >> 0;
	LoRa_1278_write( RegFrLsb, data);
	HAL_Delay(5);
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_1278_setSpreadingFactor

		description : set spreading factor, from 7 to 12.

		arguments   :
			LoRa* LoRa        --> LoRa object handler
			int   SP          --> desired spreading factor e.g 7

		returns     : Nothing
\* ----------------------------------------------------------------------------- */
void LoRa_1278_setSpreadingFactor(int SF)
{
	uint8_t	data;
	uint8_t	read;

	if(SF>12)
		SF = 12;
	if(SF<7)
		SF = 7;

	read = LoRa_1278_read(RegModemConfig2);
	HAL_Delay(10);

	data = (SF << 4) + (read & 0x0F);
	LoRa_1278_write( RegModemConfig2, data);
	HAL_Delay(10);

	LoRa_1278_setAutoLDO();
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_1278_setPower

		description : set power gain.

		arguments   :
			LoRa* LoRa        --> LoRa object handler
			int   power       --> desired power like POWER_17db

		returns     : Nothing
\* ----------------------------------------------------------------------------- */
void LoRa_1278_setPower( uint8_t power)
{
	LoRa_1278_write( RegPaConfig, power);
	HAL_Delay(10);
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_1278_setOCP

		description : set maximum allowed current.

		arguments   :
			LoRa* LoRa        --> LoRa object handler
			int   current     --> desired max currnet in mA, e.g 120

		returns     : Nothing
\* ----------------------------------------------------------------------------- */
void LoRa_1278_setOCP( uint8_t current)
{
	uint8_t	OcpTrim = 0;

	if(current<45)
		current = 45;
	if(current>240)
		current = 240;

	if(current <= 120)
		OcpTrim = (current - 45)/5;
	else if(current <= 240)
		OcpTrim = (current + 30)/10;

	OcpTrim = OcpTrim + (1 << 5);
	LoRa_1278_write( RegOcp, OcpTrim);
	HAL_Delay(10);
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_1278_setTOMsb_setCRCon

		description : set timeout msb to 0xFF + set CRC enable.

		arguments   :
			LoRa* LoRa        --> LoRa object handler

		returns     : Nothing
\* ----------------------------------------------------------------------------- */
void LoRa_1278_setTOMsb_setCRCon(void){
	uint8_t read, data;

	read = LoRa_1278_read( RegModemConfig2);

	data = read | 0x07;
	LoRa_1278_write( RegModemConfig2, data);\
	HAL_Delay(10);
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_1278_setTOMsb_setCRCon

		description : set timeout msb to 0xFF + set CRC enable.

		arguments   :
			LoRa* LoRa        --> LoRa object handler

		returns     : Nothing
\* ----------------------------------------------------------------------------- */
void LoRa_1278_setSyncWord( uint8_t syncword){
	LoRa_1278_write( RegSyncWord, syncword);
	HAL_Delay(10);
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_1278_read

		description : read a register by an address

		arguments   :
			LoRa*   LoRa        --> LoRa object handler
			uint8_t address     -->	address of the register e.g 0x1D

		returns     : register value
\* ----------------------------------------------------------------------------- */
uint8_t LoRa_1278_read( uint8_t address)
{
	uint8_t read_data;
	uint8_t data_addr;

	data_addr = address & 0x7F;
	LoRa_1278_readReg(&data_addr, 1, &read_data, 1);
	//HAL_Delay(5);

	return read_data;
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_1278_write

		description : write a value in a register by an address

		arguments   :
			LoRa*   LoRa        --> LoRa object handler
			uint8_t address     -->	address of the register e.g 0x1D
			uint8_t value       --> value that you want to write

		returns     : Nothing
\* ----------------------------------------------------------------------------- */
void LoRa_1278_write( uint8_t address, uint8_t value)
{
	uint8_t data;
	uint8_t addr;

	addr = address | 0x80;
	data = value;
	LoRa_1278_writeReg(&addr, 1, &data, 1);
	//HAL_Delay(5);
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_1278_BurstWrite

		description : write a set of values in a register by an address respectively

		arguments   :
			LoRa*   LoRa        --> LoRa object handler
			uint8_t address     -->	address of the register e.g 0x1D
			uint8_t *value      --> address of values that you want to write

		returns     : Nothing
\* ----------------------------------------------------------------------------- */
void LoRa_1278_BurstWrite(uint8_t address, uint8_t *value, uint8_t length){
	uint8_t addr;
	addr = address | 0x80;

	//NSS = 1
	LoRa_1278_CSLow();

	HAL_SPI_Transmit(lora_Drv->spi, &addr, 1, TRANSMIT_TIMEOUT);
	while (HAL_SPI_GetState(lora_Drv->spi) != HAL_SPI_STATE_READY)
		;
	//Write data in FiFo
	HAL_SPI_Transmit(lora_Drv->spi, value, length, TRANSMIT_TIMEOUT);
	while (HAL_SPI_GetState(lora_Drv->spi) != HAL_SPI_STATE_READY)
		;
	//NSS = 0
	//HAL_Delay(5);
	LoRa_1278_CSHigh();
}
/* ----------------------------------------------------------------------------- *\
		name        : LoRa_1278_isvalid

		description : check the LoRa instruct values

		arguments   :
			LoRa* LoRa --> LoRa object handler

		returns     : returns 1 if all of the values were given, otherwise returns 0
\* ----------------------------------------------------------------------------- */
uint8_t LoRa_1278_isvalid(void)
{

	return 1;
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_1278_transmit

		description : Transmit data

		arguments   :
			LoRa*    LoRa     --> LoRa object handler
			uint8_t  data			--> A pointer to the data you wanna send
			uint8_t	 length   --> Size of your data in Bytes
			uint16_t timeOut	--> Timeout in milliseconds
		returns     : 1 in case of success, 0 in case of timeout
\* ----------------------------------------------------------------------------- */
void LoRa_1278_Transmit( uint8_t* data, uint8_t length)
{
	uint8_t read;
	uint16_t timeout = 100;

	int mode = lora_Drv->current_mode;
	LoRa_1278_gotoMode(STNBY_MODE);
	read = LoRa_1278_read(RegFiFoTxBaseAddr);
	LoRa_1278_write( RegFiFoAddPtr, read);
	LoRa_1278_write( RegPayloadLength, length);
	LoRa_1278_BurstWrite(RegFiFo, data, length);
	LoRa_1278_gotoMode( TRANSMIT_MODE);
	while(timeout != 0)
	{
		read = LoRa_1278_read( RegIrqFlags);
		if((read & 0x08)!=0){
			LoRa_1278_write( RegIrqFlags, 0xFF);
			LoRa_1278_gotoMode( mode);
			return;
		}
		HAL_Delay(1);
		timeout--;
	}
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_1278_startReceiving

		description : Start receiving continuously

		arguments   :
			LoRa*    LoRa     --> LoRa object handler

		returns     : Nothing
\* ----------------------------------------------------------------------------- */
void LoRa_1278_startReceiving(void)
{
	LoRa_1278_gotoMode( RXCONTIN_MODE);
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_1278_Receive

		description : Read received data from module

		arguments   :
			LoRa*    LoRa     --> LoRa object handler
			uint8_t  data			--> A pointer to the array that you want to write bytes in it
			uint8_t	 length   --> Determines how many bytes you want to read

		returns     : The number of bytes received
\* ----------------------------------------------------------------------------- */
void LoRa_1278_HandleCallback(uint16_t GPIO_Pin)
{
uint8_t read;

	LoRa_1278_gotoMode( STNBY_MODE);
	read = LoRa_1278_read( RegIrqFlags);
	if((read & 0x40) != 0)
	{
		LoRa_1278_write( RegIrqFlags, 0xFF);
		lora_Drv->rx_payloadLen = LoRa_1278_read( RegRxNbBytes);
		read = LoRa_1278_read( RegFiFoRxCurrentAddr);
		LoRa_1278_write( RegFiFoAddPtr, read);

		for(int i=0; i<lora_Drv->rx_payloadLen; i++)
			lora_Drv->RX_Buf[i] = LoRa_1278_read( RegFiFo);
	}
	LoRa_1278_gotoMode( RXCONTIN_MODE);
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_1278_getRSSI

		description : initialize and set the right setting according to LoRa sruct vars

		arguments   :
			LoRa* LoRa        --> LoRa object handler

		returns     : Returns the RSSI value of last received packet.
\* ----------------------------------------------------------------------------- */
uint32_t LoRa_1278_getRSSI(void)
{
	uint8_t read;
	read = LoRa_1278_read( RegPktRssiValue);
	return -164 + read;
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_1278_init

		description : initialize and set the right setting according to LoRa sruct vars

		arguments   :
			LoRa* LoRa        --> LoRa object handler

		returns     : Nothing
\* ----------------------------------------------------------------------------- */
uint32_t LoRa_1278_init(void)
{
	uint8_t    data;
	uint8_t    read;

	if(LoRa_1278_isvalid()){
		// goto sleep mode:
			LoRa_1278_gotoMode( SLEEP_MODE);
			HAL_Delay(10);

		// turn on LoRa mode:
			read = LoRa_1278_read( RegOpMode);
			HAL_Delay(10);
			data = read | 0x80;
			LoRa_1278_write( RegOpMode, data);
			HAL_Delay(100);

		// set frequency:
			LoRa_1278_setFrequency( lora_Drv->frequency);

		// set output power gain:
			LoRa_1278_setPower( lora_Drv->power);

		// set over current protection:
			LoRa_1278_setOCP( lora_Drv->overCurrentProtection);

		// set LNA gain:
			LoRa_1278_write( RegLna, 0x23);

		// set spreading factor, CRC on, and Timeout Msb:
			LoRa_1278_setTOMsb_setCRCon();
			LoRa_1278_setSpreadingFactor( lora_Drv->spredingFactor);

		// set Timeout Lsb:
			LoRa_1278_write( RegSymbTimeoutL, 0xFF);

		// set bandwidth, coding rate and expilicit mode:
			// 8 bit RegModemConfig --> | X | X | X | X | X | X | X | X |
			//       bits represent --> |   bandwidth   |     CR    |I/E|
			data = 0;
			data = (lora_Drv->bandWidth << 4) + (lora_Drv->crcRate << 1);
			LoRa_1278_write( RegModemConfig1, data);
			LoRa_1278_setAutoLDO();

		// set preamble:
			LoRa_1278_write( RegPreambleMsb, lora_Drv->preamble >> 8);
			LoRa_1278_write( RegPreambleLsb, lora_Drv->preamble >> 0);

		// DIO mapping:   --> DIO: RxDone
			read = LoRa_1278_read( RegDioMapping1);
			data = read | 0x3F;
			LoRa_1278_write( RegDioMapping1, data);

		// goto standby mode:
			LoRa_1278_gotoMode( STNBY_MODE);
			lora_Drv->current_mode = STNBY_MODE;
			HAL_Delay(10);

			read = LoRa_1278_read( RegVersion);
			if(read == 0x12)
				return LORA_OK;
			else
				return LORA_NOT_FOUND;
	}
	else {
		return LORA_UNAVAILABLE;
	}
}

#endif // #ifdef A_OS_SPI_ENABLED
