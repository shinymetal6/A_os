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
 * nfc.h
 *
 *  Created on: Jan 12, 2026
 *      Author: fil
 */

#ifndef DRIVERS_SPI_NFC_NFC_H_
#define DRIVERS_SPI_NFC_NFC_H_

#ifdef NFC_ENABLED

#include "../spi.h"
#include <string.h>

typedef struct
{
	/* driver header */
	uint8_t				status;
	uint8_t				flags;
	uint8_t 			process;
	SPI_HandleTypeDef 	*bus;
	GPIO_TypeDef	 	*cs_port;
	uint16_t			cs_bit;
	uint32_t 			wakeup_id;
	uint32_t			*next_drv;
	/* driver proprietary data */
#ifdef A_OS_TIMERS_ENABLED
	TIM_HandleTypeDef	*backlight_timer;
	uint32_t			backlight_timer_channel;
#endif
	GPIO_TypeDef	 	*reset_port;
	uint16_t			reset_bit;
	uint16_t			reset_time;
	GPIO_TypeDef	 	*busy_port;
	uint16_t			busy_bit;
	GPIO_TypeDef	 	*irq_port;
	uint32_t 			spi_timeout_ms;
	uint32_t 			spi_transfer_result;
	uint16_t			irq_bit;
	IRQn_Type		 	IRQ_number;
#ifdef HAL_GPIO_MODULE_ENABLED
	GPIO_Interrupt_DriverStruct_t	*nfc_irq_driver;
	uint32_t			(*nfc_irq_callback)(uint16_t GPIO_Pin,uint32_t *nfc_struct);
#endif
	uint32_t			(*nfc_ISO14443_init)(uint32_t *nfc_struct);
	uint8_t				(*nfc_ISO14443_send_REQA)(uint32_t *nfc_struct);
	uint8_t				(*nfc_send_ISO14443_AntiCollision)(uint32_t *nfc_struct);
	uint8_t				(*nfc_ISO14443_Authenticate)(uint32_t *nfc_struct,uint8_t *Key,uint8_t KeyType, uint8_t BlockNo);
	uint8_t				(*nfc_ISO14443_BlockRead)(uint32_t *nfc_struct, uint8_t BlockNo,uint8_t *Block);
	uint8_t				(*nfc_ISO14443_BlockWrite)(uint32_t *nfc_struct, uint8_t BlockNo,uint8_t *Block);
	uint32_t			(*nfc_hw_data)(uint32_t *nfc_struct,uint32_t param1, uint8_t *param2, uint32_t param3);
	uint32_t			(*nfc_reset)(uint32_t *nfc_struct);
	uint32_t			(*nfc_rf_on)(uint32_t *nfc_struct);
	uint32_t			(*nfc_rf_off)(uint32_t *nfc_struct);
	uint8_t 			*tx_data_ptr;
	uint8_t 			*rx_data_ptr;
	uint16_t			nfc_model;
	uint32_t			iso_card;
	uint8_t 			ATQA;
	uint8_t 			SAK;
	uint8_t 			UID[8];
	uint8_t 			UID_len;
	uint8_t 			BCC_UID;
	uint8_t 			KEY[6];
	uint8_t				dma_timeout;
	uint32_t			time_start;
	uint32_t			op_time;
}SPI_NFC_DriverStruct_t;

#define	NFC_IS_PN5180			0x5180
#define	SPI_NFC_DMA_TIMEOUT		250
#define	PN5180_SPI_TIMEOUT		250
#define	NFC_CARD_IS_14443		0x14443
#define	NFC_14443_LEVEL2AC_FLAG	0x88

#define	NFC_CARD_IS_15693		0x15693
#define	NFC_DEFAULT_RESET_TIME	100

extern uint32_t	spi_nfc_register(SPI_NFC_DriverStruct_t *spi_nfc_Drv);
extern uint32_t	spi_nfc_reset(SPI_NFC_DriverStruct_t *spi_nfc_Drv);
extern uint32_t	spi_nfc_rf_on(SPI_NFC_DriverStruct_t *spi_nfc_Drv);
extern uint32_t	spi_nfc_rf_off(SPI_NFC_DriverStruct_t *spi_nfc_Drv);

extern uint32_t	spi_nfc_ISO14443_init(SPI_NFC_DriverStruct_t *spi_nfc_Drv);
extern uint8_t	spi_nfc_ISO14443_send_REQA(SPI_NFC_DriverStruct_t *spi_nfc_Drv);
extern uint32_t	spi_nfc_send_ISO14443_AntiCollision(SPI_NFC_DriverStruct_t *spi_nfc_Drv);
extern uint32_t	spi_ISO14443_Authenticate(SPI_NFC_DriverStruct_t *spi_nfc_Drv,uint8_t *Key,uint8_t KeyType, uint8_t BlockNo);
extern uint32_t	spi_ISO14443_BlockRead(SPI_NFC_DriverStruct_t *spi_nfc_Drv, uint8_t BlockNo,uint8_t *Block);
extern uint32_t	spi_ISO14443_BlockWrite(SPI_NFC_DriverStruct_t *spi_nfc_Drv, uint8_t BlockNo,uint8_t *Block);

extern uint32_t	spi_nfc_get_hwdata(SPI_NFC_DriverStruct_t *spi_nfc_Drv,uint32_t addr, uint8_t *buffer, uint32_t len);

#include "iso14443.h"
#include "crypto1.h"
#include "PN5180/pn5180.h"

#endif // #ifdef NFC_ENABLED

#endif /* DRIVERS_SPI_NFC_NFC_H_ */
