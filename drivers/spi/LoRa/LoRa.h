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
 * LoRa.h
 *
 *  Created on: Jul 30, 2025
 *      Author: fil
 */

#ifndef DRIVERS_SPI_LORA_LORA_H_
#define DRIVERS_SPI_LORA_LORA_H_
#ifdef LORA_ENABLED

#define	LORA_MAX_PACKET_LENGTH	255
typedef struct
{
	uint8_t				status;
	uint8_t				flags;
	uint8_t				handle;
	uint32_t 			wakeup_id;
	SPI_HandleTypeDef	*spi;
	uint32_t 			spi_timeout_ms;
	uint32_t 			device_id;
	uint16_t			CS_bit;
	GPIO_TypeDef	 	*CS_port;
	uint16_t			RESET_bit;
	GPIO_TypeDef	 	*RESET_port;
	uint16_t			IRQ_bit;
	GPIO_TypeDef	 	*IRQ_port;
	IRQn_Type		 	IRQ_number;
	uint16_t			RADIO_BUSY_bit;
	GPIO_TypeDef	 	*RADIO_BUSY_port;
	uint32_t			current_mode;
	uint8_t				*TX_Buf;		// internal tx buffer
	uint8_t				tx_payloadLen; 	// tx len
	uint8_t				*RX_Buf;		// internal rx buffer
	uint8_t				rx_payloadLen; 	// rx len
	uint8_t				Packet_Buf[LORA_MAX_PACKET_LENGTH];
	uint32_t			(*LoRa_Init)(void);
	void 				(*LoRa_HandleCallback)(uint16_t GPIO_Pin);
	void 				(*LoRa_Transmit)(uint8_t*, uint8_t);
	void 				(*LoRa_setModeStandby)(void);
	void 				(*LoRa_setModeReceive)(uint32_t timeoutMs);
	uint8_t				(*LoRa_getstatus)(void);
	void 				(*LoRa_SetFrequency)(uint32_t frequency);
	uint32_t			(*LoRa_GetRSSI)(void);
	void 				(*LoRa_WriteReg)(uint16_t addr, uint8_t data);
	void				(*LoRa_ReadReg)(uint16_t addr);
	void				(*LoRa_WriteRegisters)( uint16_t addr, uint8_t *buffer, uint8_t size );
	void 				(*LoRa_ReadRegisters)( uint16_t addr, uint8_t *buffer, uint8_t size );
	void				(*LoRa_WriteSingleRegister)( uint16_t addr, uint8_t data );
	uint8_t 			(*LoRa_ReadSingleRegister) ( uint16_t addr );
/*
	void 				(*LoRa_setRX)(void);
	uint8_t				(*LoRa_Check_Correct)(void);
	uint16_t			(*LoRa_getdeverr)(void);
	uint16_t			(*LoRa_cleardeverr)(void);
	void 				(*LoRa_Set_Command)(uint8_t *cmnd_, uint8_t *ans_, uint16_t Len,uint32_t Time_out ,uint16_t Delay);
*/
	uint32_t		frequency;
	uint8_t			spredingFactor;
	uint8_t			bandWidth;
	uint8_t			crcRate;
	uint16_t		preamble;
	uint8_t			power;
	uint8_t			overCurrentProtection;

}LORA_Drv_TypeDef;

#define	LORA_DRIVER_STATUS_IN_USE		0x80
#define	LORA_DRIVER_STATUS_INITIALIZED	0x40
#define	LORA_DRIVER_STATUS_INITPEND	0x20
#define	LORA_DRIVER_STATUS_REQUESTED	0x10
#define	LORA_DRIVER_STATUS_FAILED		0x01
#define	LORA_DRIVER_STATUS_UNUSED		0

#define	ID_SX1261	0x1261
#define	ID_SX1262	0x1262
#define	ID_SX1278	0x1278

extern	uint32_t	LoRa_register(LORA_Drv_TypeDef *driver_private_data);
extern	void LoRa_Init(void);
extern	void LoRa_Tx(uint8_t *buffer, uint8_t size);
extern	void LoRa_SetModeReceive(uint32_t timeoutMs);
extern	uint32_t LoRa_GetRSSI(void);
extern	void LoRa_SetFrequency(uint32_t frequency);
extern	void LoRa_SetModeStandby(void);

#endif // #ifdef LORA_ENABLED

#endif /* DRIVERS_SPI_LORA_LORA_H_ */
