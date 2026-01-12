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
 * pn5180.h
 *
 *  Created on: Jan 12, 2026
 *      Author: fil
 */

#ifndef DRIVERS_SPI_NFC_PN5180_PN5180_H_
#define DRIVERS_SPI_NFC_PN5180_PN5180_H_

extern	GPIO_TypeDef	*PN5180_cs_port;
extern	uint16_t		PN5180_cs_bit;
extern	GPIO_TypeDef	*PN5180_reset_port;
extern	uint16_t		PN5180_reset_bit;
extern	uint16_t		PN5180_reset_time;
extern	SPI_HandleTypeDef 	*PN5180_spi_port;
extern	uint8_t			*PN5180_flags;
extern	uint8_t			*PN5180_dma_timeout;


extern	uint32_t		pn5180_init(void);
extern	uint32_t		pn5180_reset(void);
extern	uint32_t		pn5180_write_register(uint8_t address,uint8_t byte0,uint8_t byte1,uint8_t byte2,uint8_t byte3);
extern	uint32_t		pn5180_send_data(uint8_t NumberOfValidBitsInLastByte,uint8_t *tx_data_ptr);
extern	uint32_t		pn5180_read_data(uint8_t *rx_data_ptr);
extern	uint32_t		pn5180_load_rf_config(void);
extern	uint32_t		pn5180_rf_on(void);
extern	uint32_t		pn5180_rf_off(void);
#endif /* DRIVERS_SPI_NFC_PN5180_PN5180_H_ */
