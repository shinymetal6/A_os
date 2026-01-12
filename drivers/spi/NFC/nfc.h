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

#include "../spi.h"
#include <string.h>
#include "PN5180/pn5180.h"

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
	uint32_t			(*nfc_init)(void);
	uint32_t			(*nfc_reset)(void);
	uint32_t			(*nfc_write_register)	(uint8_t address,uint8_t byte0,uint8_t byte1,uint8_t byte2,uint8_t byte3);
	uint32_t			(*nfc_send_data)	(uint8_t NumberOfValidBitsInLastByte,uint8_t *tx_data_ptr);
	uint32_t			(*nfc_read_data)	(uint8_t *rx_data_ptr);
	uint32_t			(*nfc_load_rf_config)	(void);
	uint32_t			(*nfc_rf_on)	(void);
	uint32_t			(*nfc_rf_off)	(void);
	uint8_t 			*tx_data_ptr;
	uint8_t 			*rx_data_ptr;
	uint16_t			nfc_model;
	uint8_t				dma_timeout;
	uint32_t			time_start;
	uint32_t			op_time;
}SPI_NFC_DriverStruct_t;

#define	NFC_IS_PN5180		0x5180
#define	SPI_NFC_DMA_TIMEOUT	250
#define	PN5180_SPI_TIMEOUT	250

#endif /* DRIVERS_SPI_NFC_NFC_H_ */
