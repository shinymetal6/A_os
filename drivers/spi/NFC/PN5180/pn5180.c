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
 * pn5180.c
 *
 *  Created on: Jan 12, 2026
 *      Author: fil
 */

#include "main.h"
#include "../../../../kernel/A.h"
#include "../../../../kernel/A_exported_functions.h"

#ifdef A_OS_SPI_ENABLED
#ifdef NFC_ENABLED

#include "pn5180.h"
#include "../nfc.h"

GPIO_TypeDef	*PN5180_cs_port;
uint16_t		PN5180_cs_bit;
GPIO_TypeDef	*PN5180_reset_port;
uint16_t		PN5180_reset_bit;
uint16_t		PN5180_reset_time;
SPI_HandleTypeDef 	*PN5180_spi_port;
uint8_t			*PN5180_flags;
uint8_t			*PN5180_dma_timeout;

uint32_t pn5180_init(void)
{
	return 0;
}
uint32_t pn5180_reset(void)
{
    HAL_GPIO_WritePin(PN5180_reset_port, PN5180_reset_bit, GPIO_PIN_RESET);
    task_delay(50);
    HAL_GPIO_WritePin(PN5180_reset_port, PN5180_reset_bit, GPIO_PIN_SET);
	return 0;
}
uint32_t pn5180_write_register(uint8_t address,uint8_t byte0,uint8_t byte1,uint8_t byte2,uint8_t byte3)
{
uint8_t nfc_spi_buf[5] = {address,byte0,byte1,byte2,byte3};

    HAL_GPIO_WritePin(PN5180_cs_port, PN5180_cs_bit, GPIO_PIN_RESET);
    HAL_SPI_Transmit(PN5180_spi_port, nfc_spi_buf, 5, PN5180_SPI_TIMEOUT);
	return 0;
}
uint32_t pn5180_send_data(uint8_t NumberOfValidBitsInLastByte,uint8_t *tx_data_ptr)
{
	return 0;
}
uint32_t pn5180_read_data(uint8_t *rx_data_ptr)
{
	return 0;
}
uint32_t pn5180_load_rf_config(void)
{
	return 0;
}
uint32_t pn5180_rf_on(void)
{
	return 0;
}
uint32_t pn5180_rf_off(void)
{
	return 0;
}

#endif // #ifdef NFC_ENABLED
#endif // #ifdef A_OS_SPI_ENABLED

