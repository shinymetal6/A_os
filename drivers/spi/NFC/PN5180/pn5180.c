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
 *  Created on: Jan 26, 2026
 *      Author: fil
 *      Details :
 * 				Clock Polarity (CPOL): Low
 *				Clock Phase (CPHA): 1 Edge
 *				Baud Rate: ≤ 10 MHz
https://github.com/ATrappmann/PN5180-Library/blob/master/PN5180.cpp#L281
 */

#include "main.h"
#include "../../../../kernel/A.h"
#include "../../../../kernel/A_exported_functions.h"

#ifdef A_OS_SPI_ENABLED
#ifdef NFC_ENABLED

#include "../nfc.h"
#include "pn5180.h"
#include "pn5180_defs.h"

static inline uint32_t wait_busylow(SPI_NFC_DriverStruct_t *spi_nfc_Drv)
{
	uint32_t	tout=2000;
	while(HAL_GPIO_ReadPin(spi_nfc_Drv->busy_port, spi_nfc_Drv->busy_bit) == GPIO_PIN_SET)
	{
		A_wait_uSec_NoIrq(1);
		if ( --tout == 0 )
		{
		    HAL_GPIO_WritePin(ERROR_GPIO_Port, ERROR_Pin, GPIO_PIN_SET);
			return 1;
		}
	}
	return 0;
}
uint32_t werr = 0;
uint32_t wait_high_error(void)
{
    HAL_GPIO_WritePin(ERROR_GPIO_Port, ERROR_Pin, GPIO_PIN_SET);
	werr++;
	return 1;
}

static uint32_t pn5180_send_spi(SPI_NFC_DriverStruct_t *spi_nfc_Drv,uint32_t len)
{
uint32_t	ret_val;

	if ( wait_busylow(spi_nfc_Drv) == 1)
		return 1;
    HAL_GPIO_WritePin(spi_nfc_Drv->cs_port, spi_nfc_Drv->cs_bit, GPIO_PIN_RESET);
    ret_val = HAL_SPI_Transmit(spi_nfc_Drv->bus, spi_nfc_Drv->tx_data_ptr, len, PN5180_SPI_TIMEOUT);
    if ( HAL_GPIO_ReadPin(spi_nfc_Drv->busy_port, spi_nfc_Drv->busy_bit) == GPIO_PIN_RESET)
		return wait_high_error();
    HAL_GPIO_WritePin(spi_nfc_Drv->cs_port, spi_nfc_Drv->cs_bit, GPIO_PIN_SET);
	if ( wait_busylow(spi_nfc_Drv) == 1)
		return 1;
    return ret_val;
}

static uint32_t pn5180_receive_spi(SPI_NFC_DriverStruct_t *spi_nfc_Drv,uint32_t len)
{
uint32_t	ret_val;

	if ( wait_busylow(spi_nfc_Drv) == 1)
		return 1;
    HAL_GPIO_WritePin(spi_nfc_Drv->cs_port, spi_nfc_Drv->cs_bit, GPIO_PIN_RESET);
    ret_val = HAL_SPI_Receive(spi_nfc_Drv->bus, spi_nfc_Drv->rx_data_ptr, len, PN5180_SPI_TIMEOUT);
    if ( HAL_GPIO_ReadPin(spi_nfc_Drv->busy_port, spi_nfc_Drv->busy_bit) == GPIO_PIN_RESET)
		return wait_high_error();
    HAL_GPIO_WritePin(spi_nfc_Drv->cs_port, spi_nfc_Drv->cs_bit, GPIO_PIN_SET);
	if ( wait_busylow(spi_nfc_Drv) == 1)
		return 1;
    return ret_val;
}

uint32_t pn5180_read_register(SPI_NFC_DriverStruct_t *spi_nfc_Drv,uint8_t reg)
{
	spi_nfc_Drv->tx_data_ptr[0] = PN5180_CMD_READ_REGISTER;
	spi_nfc_Drv->tx_data_ptr[1] = reg;
	pn5180_send_spi(spi_nfc_Drv, 2);
	pn5180_receive_spi(spi_nfc_Drv, 4);
	return ((spi_nfc_Drv->rx_data_ptr[3] << 24 ) | (spi_nfc_Drv->rx_data_ptr[2] << 16 ) | (spi_nfc_Drv->rx_data_ptr[1] << 8 ) | spi_nfc_Drv->rx_data_ptr[0]);
}

uint32_t pn5180_write_register(SPI_NFC_DriverStruct_t *spi_nfc_Drv,uint8_t command,uint8_t reg, uint32_t value)
{
uint8_t *p = (uint8_t*)&value;
	spi_nfc_Drv->tx_data_ptr[0] = command;
	spi_nfc_Drv->tx_data_ptr[1] = reg;
	spi_nfc_Drv->tx_data_ptr[2] = p[3];
	spi_nfc_Drv->tx_data_ptr[3] = p[2];
	spi_nfc_Drv->tx_data_ptr[4] = p[1];
	spi_nfc_Drv->tx_data_ptr[5] = p[0];
	return pn5180_send_spi(spi_nfc_Drv, 6);
}

static uint32_t pn5180_send_receive_spi(SPI_NFC_DriverStruct_t *spi_nfc_Drv,uint32_t len)
{
uint32_t	rx_len;
	pn5180_send_spi(spi_nfc_Drv, len);
	task_delay(PN5180_SEND_RECEIVE_DELAY);
	rx_len = pn5180_read_register(spi_nfc_Drv,PN5180_REG_RX_STATUS) & 0x1ff;
	if ( rx_len )
	{
		//task_delay(PN5180_SEND_RECEIVE_DELAY);
		spi_nfc_Drv->tx_data_ptr[0] = PN5180_CMD_READ_DATA;
		spi_nfc_Drv->tx_data_ptr[1] = 0;
		pn5180_send_spi(spi_nfc_Drv, 2);
		pn5180_receive_spi(spi_nfc_Drv, rx_len);
		return rx_len;
	}
	return 0;
}


uint32_t pn5180_set_rf_on(uint32_t *spi_nfc_driver32)
{
SPI_NFC_DriverStruct_t *spi_nfc_Drv = (SPI_NFC_DriverStruct_t *)spi_nfc_driver32;
	spi_nfc_Drv->tx_data_ptr[0] = PN5180_CMD_RF_ON;
	spi_nfc_Drv->tx_data_ptr[1] = 0;
	pn5180_send_spi(spi_nfc_Drv, 2);
	return 0;
}

uint32_t pn5180_set_rf_off(uint32_t *spi_nfc_driver32)
{
SPI_NFC_DriverStruct_t *spi_nfc_Drv = (SPI_NFC_DriverStruct_t *)spi_nfc_driver32;
	spi_nfc_Drv->tx_data_ptr[0] = PN5180_CMD_RF_OFF;
	spi_nfc_Drv->tx_data_ptr[1] = 0;
	pn5180_send_spi(spi_nfc_Drv, 2);
	return 0;
}

void pn5180_load_rf_config(SPI_NFC_DriverStruct_t *spi_nfc_Drv)
{
	spi_nfc_Drv->tx_data_ptr[0] = PN5180_CMD_LOAD_RF_CONFIG;
	spi_nfc_Drv->tx_data_ptr[1] = 0x00;
	spi_nfc_Drv->tx_data_ptr[2] = 0x80;
	pn5180_send_spi(spi_nfc_Drv,3);
}

uint32_t pn5180_readEEprom(uint32_t *spi_nfc_driver32,uint32_t addr, uint8_t *buffer, uint32_t len)
{
SPI_NFC_DriverStruct_t *spi_nfc_Drv = (SPI_NFC_DriverStruct_t *)spi_nfc_driver32;
	if ((addr > 254) || ((addr+len) > 254))
		return 1;
	spi_nfc_Drv->tx_data_ptr[0] = PN5180_CMD_READ_EEPROM;
	spi_nfc_Drv->tx_data_ptr[1] = addr;
	spi_nfc_Drv->tx_data_ptr[2] = len;
	return pn5180_send_receive_spi(spi_nfc_Drv,3);
}

uint32_t pn5180_reset(uint32_t *spi_nfc_driver32)
{
SPI_NFC_DriverStruct_t *spi_nfc_Drv = (SPI_NFC_DriverStruct_t *)spi_nfc_driver32;
    HAL_GPIO_WritePin(spi_nfc_Drv->reset_port, spi_nfc_Drv->reset_bit, GPIO_PIN_RESET);
    task_delay(spi_nfc_Drv->reset_time);
    HAL_GPIO_WritePin(spi_nfc_Drv->reset_port, spi_nfc_Drv->reset_bit, GPIO_PIN_SET);
    task_delay(spi_nfc_Drv->reset_time);
	if ( wait_busylow(spi_nfc_Drv) == 1)
		return 1;
	pn5180_set_rf_off(spi_nfc_driver32);

	return 0;
}

uint32_t pn5180_ISO14443_init(uint32_t *spi_nfc_driver32)
{
SPI_NFC_DriverStruct_t *spi_nfc_Drv = (SPI_NFC_DriverStruct_t *)spi_nfc_driver32;
	pn5180_load_rf_config(spi_nfc_Drv);
	pn5180_write_register(spi_nfc_Drv,PN5180_CMD_WRITE_REGISTER_OR_MASK,PN5180_REG_SYSTEM_CONFIG, PN5180_SYSTEM_CONFIG_MFC_CRYPTO_ON_MASK);
	pn5180_write_register(spi_nfc_Drv,PN5180_CMD_WRITE_REGISTER_AND_MASK,PN5180_REG_SYSTEM_CONFIG, ~PN5180_SYSTEM_CONFIG_MFC_CRYPTO_ON_MASK);
	pn5180_write_register(spi_nfc_Drv,PN5180_CMD_WRITE_REGISTER_AND_MASK,PN5180_REG_CRC_TX_CONFIG, 0xfeffffff);
	pn5180_write_register(spi_nfc_Drv,PN5180_CMD_WRITE_REGISTER_AND_MASK,PN5180_REG_CRC_RX_CONFIG, 0xfeffffff);
	pn5180_write_register(spi_nfc_Drv,PN5180_CMD_WRITE_REGISTER,PN5180_REG_IRQ_CLEAR, 0xffff0f00);
	pn5180_write_register(spi_nfc_Drv,PN5180_CMD_WRITE_REGISTER_AND_MASK,PN5180_REG_SYSTEM_CONFIG, 0xf8ffffff);
	pn5180_write_register(spi_nfc_Drv,PN5180_CMD_WRITE_REGISTER_OR_MASK,PN5180_REG_SYSTEM_CONFIG,  0x03000000);
	return 0;
}

uint8_t pn5180_send_ISO14443_REQA(uint32_t *spi_nfc_driver32)
{
SPI_NFC_DriverStruct_t *spi_nfc_Drv = (SPI_NFC_DriverStruct_t *)spi_nfc_driver32;
uint32_t	rx_len = 0;
	/* Sends WAKE-UP(Wake-UP command,Type A) command */
	spi_nfc_Drv->tx_data_ptr[0] = PN5180_CMD_SEND_DATA;
	spi_nfc_Drv->tx_data_ptr[1] = 0X07;
	spi_nfc_Drv->tx_data_ptr[2] = PN5180_CMD_NFC_WUPA;
	spi_nfc_Drv->tx_data_ptr[2] = PN5180_CMD_NFC_REQA;
	rx_len = pn5180_send_receive_spi(spi_nfc_Drv,3);
	if ( rx_len )
		return spi_nfc_Drv->rx_data_ptr[0];
	return 0;
}

uint32_t irqstatus;
uint8_t SAK;
uint8_t UID[4];

uint8_t pn5180_send_ISO14443_AntiColl1(uint32_t *spi_nfc_driver32)
{
SPI_NFC_DriverStruct_t *spi_nfc_Drv = (SPI_NFC_DriverStruct_t *)spi_nfc_driver32;
uint32_t rx_len;
	/* Sends Anticollision1 command */
	SAK=0;
	pn5180_write_register(spi_nfc_Drv,PN5180_CMD_WRITE_REGISTER,PN5180_REG_IRQ_CLEAR, 0xffff0f00);

	spi_nfc_Drv->tx_data_ptr[0] = PN5180_CMD_SEND_DATA;
	spi_nfc_Drv->tx_data_ptr[1] = 0x00;
	spi_nfc_Drv->tx_data_ptr[2] = 0x93;
	spi_nfc_Drv->tx_data_ptr[3] = 0x20;
	pn5180_send_receive_spi(spi_nfc_Drv,4);
	pn5180_write_register(spi_nfc_Drv,PN5180_CMD_WRITE_REGISTER_OR_MASK,PN5180_REG_CRC_TX_CONFIG,  0x01000000);
	pn5180_write_register(spi_nfc_Drv,PN5180_CMD_WRITE_REGISTER_OR_MASK,PN5180_REG_CRC_RX_CONFIG,  0x01000000);

	UID[0] = spi_nfc_Drv->rx_data_ptr[0];
	UID[1] = spi_nfc_Drv->rx_data_ptr[1];
	UID[2] = spi_nfc_Drv->rx_data_ptr[2];
	UID[3] = spi_nfc_Drv->rx_data_ptr[3];
	spi_nfc_Drv->tx_data_ptr[0] = PN5180_CMD_SEND_DATA;
	spi_nfc_Drv->tx_data_ptr[1] = 0x00;
	spi_nfc_Drv->tx_data_ptr[2] = 0x93;
	spi_nfc_Drv->tx_data_ptr[3] = 0x70;
	spi_nfc_Drv->tx_data_ptr[4] = UID[0];
	spi_nfc_Drv->tx_data_ptr[5] = UID[1];
	spi_nfc_Drv->tx_data_ptr[6] = UID[2];
	spi_nfc_Drv->tx_data_ptr[7] = UID[3];
	spi_nfc_Drv->tx_data_ptr[8] = UID[0] ^ UID[1] ^ UID[2] ^ UID[3];
	rx_len = pn5180_send_receive_spi(spi_nfc_Drv,9);
	if ( rx_len )
		SAK = spi_nfc_Drv->rx_data_ptr[0];
	return SAK;
}

#endif // #ifdef NFC_ENABLED
#endif // #ifdef A_OS_SPI_ENABLED
