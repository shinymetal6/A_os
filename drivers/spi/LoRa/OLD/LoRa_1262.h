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
 * LoRa_1262.h
 *
 *  Created on: Jul 31, 2025
 *      Author: fil
 */

#ifndef DRIVERS_SPI_LORA_LORA_1262_H_
#define DRIVERS_SPI_LORA_LORA_1262_H_

#include "SX1262_Definitions.h"

extern	void		LoRa_1262_CSLow(void);
extern	void		LoRa_1262_CSHigh(void);

extern	void		LoRa_1262_BusyWait(void);
extern	uint32_t	LoRa_1262_IsBusy(void);

extern	void		LoRa_1262_HandleCallback(uint16_t GPIO_Pin);
extern	void		LoRa_1262_Set_Command(uint8_t *cmnd_, uint8_t *ans_, uint16_t Len,uint32_t Time_out ,uint16_t Delay);
extern	void		LoRa_1262_Transmit(uint8_t*, uint8_t);
extern	uint32_t	LoRa_1262_Init(void);
extern	void		LoRa_1262_setRX(void);
extern	uint8_t 	LoRa_1262_Check_Correct(void);
extern	uint8_t 	LoRa_1262_waitForRadioCommandCompletion(uint32_t timeout);
extern	void		LoRa_1262_setModeStandby(void);
extern	void		LoRa_1262_setModeReceive(void);
extern	uint8_t 	LoRa_1262_getstatus(void);
extern	uint16_t 	LoRa_1262_getdeverr(void);
extern	uint16_t 	LoRa_1262_cleardeverr(void);
extern	void		LoRa_1262_Radio_essental_Config(void);
extern	void		LoRa_1262_SetFrequency(uint32_t frequency);
extern	uint32_t 	LoRa_1262_getRSSI(void);
extern	void 		LoRa_1262_WriteRegisters(uint16_t addr, uint8_t *buffer, uint8_t size);
extern	void 		LoRa_1262_ReadRegisters( uint16_t addr, uint8_t *buffer, uint8_t size );
extern	uint8_t 	LoRa_1262_ReadSingleRegister( uint16_t addr );
extern	void 		LoRa_1262_WriteSingleRegister(uint16_t addr, uint8_t data);




#endif /* DRIVERS_SPI_LORA_LORA_1262_H_ */
