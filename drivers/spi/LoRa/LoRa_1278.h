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
 * LoRa_1278.h
 *
 *  Created on: Jul 31, 2025
 *      Author: fil
 */

#ifndef DRIVERS_SPI_LORA_LORA_1278_H_
#define DRIVERS_SPI_LORA_LORA_1278_H_

#include "SX1278_Definitions.h"

extern	uint32_t LoRa_1278_init(void);
extern	void LoRa_1278_reset(void);
extern	void LoRa_1278_setModeStandby(void);
extern	void LoRa_1278_setModeReceive(void);
extern	uint8_t LoRa_1278_getstatus(void);
extern	void LoRa_1278_setFrequency( uint32_t frequency);
extern	void LoRa_1278_HandleCallback(uint16_t GPIO_Pin);
extern	void LoRa_1278_Transmit(uint8_t* data, uint8_t length);
extern	uint32_t LoRa_1278_getRSSI(void);

extern	uint8_t LoRa_1278_read(uint8_t address);
extern	void LoRa_1278_write( uint8_t address, uint8_t value);

/*
extern	void LoRa_1278_readReg(uint8_t* address, uint16_t r_length, uint8_t* output, uint16_t w_length);
extern	void LoRa_1278_writeReg(uint8_t* address, uint16_t r_length, uint8_t* values, uint16_t w_length);
extern	void LoRa_1278_BurstWrite(uint8_t address, uint8_t *value, uint8_t length);

extern	void LoRa_1278_gotoMode(int mode);
extern	uint8_t LoRa_1278_isvalid(void);

extern	void LoRa_1278_setLowDaraRateOptimization(uint8_t value);
extern	void LoRa_1278_setAutoLDO(void);
extern	void LoRa_1278_setFrequency(int freq);
extern	void LoRa_1278_setSpreadingFactor(int SP);
extern	void LoRa_1278_setPower(uint8_t power);
extern	void LoRa_1278_setOCP(uint8_t current);
extern	void LoRa_1278_setTOMsb_setCRCon(void);
extern	void LoRa_1278_setSyncWord(uint8_t syncword);
extern	void LoRa_1278_startReceiving(void);
*/

#endif /* DRIVERS_SPI_LORA_LORA_1278_H_ */
