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
 *  Created on: Jul 30, 2025
 *      Author: fil
 */

#ifndef SRC_LORA_1262_H_
#define SRC_LORA_1262_H_

#include "SX1262_Definitions.h"
/*
typedef struct{
	// peripheral config
	SPI_HandleTypeDef		SPI;
	GPIO_TypeDef*			Reset_Port;
	uint16_t				Reset_Pin;
	GPIO_TypeDef*			NSS_Port;
	uint16_t				NSS_Pin;
	GPIO_TypeDef*			Busy_Port;
	uint16_t				Busy_Pin;

	SX1262_STATE			State;

	uint8_t					TX_Buf[300]; // internal buffer for SPI commands
	uint8_t					RX_Buf[300]; // internal buffer for SPI commands

	uint8_t					Packet_Buf[SX126X_MAX_PACKET_LENGTH];

	void 					(*RX_Callback)(uint8_t*, uint8_t);




} LoRa_1262_DriverStruct_t;
*/
//LoRa_1262_DriverStruct_t *LoRa_1262_Get_st(void);

extern	void		LoRa_1262_CSLow(void);
extern	void		LoRa_1262_CSHigh(void);

extern	void		LoRa_1262_BusyWait(void);
extern	uint32_t	LoRa_1262_IsBusy(void);

extern	void		LoRa_1262_HandleCallback(uint16_t GPIO_Pin);
extern	void		LoRa_1262_Set_Command(uint8_t *cmnd_, uint8_t *ans_, uint16_t Len,uint32_t Time_out ,uint16_t Delay);
extern	void		LoRa_1262_Transmit(uint8_t*, uint8_t);
extern	void		LoRa_1262_Init(void);
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

#endif /* SRC_LORA_1262_H_ */
