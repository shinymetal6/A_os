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
 *      Details :
 * 				Clock Polarity (CPOL): Low
 *				Clock Phase (CPHA): 1 Edge
 *				Baud Rate: ≤ 10 MHz
 */

#include "main.h"
#include "../../../../kernel/A.h"
#include "../../../../kernel/A_exported_functions.h"

#ifdef A_OS_SPI_ENABLED
#ifdef NFC_ENABLED

#include "../nfc.h"
#include "pn5180.h"
#include "pn5180_defs.h"

static uint32_t pn5180_send_spi(SPI_NFC_DriverStruct_t *spi_nfc_Drv,uint8_t len)
{
    HAL_GPIO_WritePin(spi_nfc_Drv->cs_port, spi_nfc_Drv->cs_bit, GPIO_PIN_RESET);
    return HAL_SPI_Transmit(spi_nfc_Drv->bus, spi_nfc_Drv->tx_data_ptr, len, PN5180_SPI_TIMEOUT);
}

static uint32_t pn5180_send_receive_spi(SPI_NFC_DriverStruct_t *spi_nfc_Drv,uint16_t len)
{
    HAL_GPIO_WritePin(spi_nfc_Drv->cs_port, spi_nfc_Drv->cs_bit, GPIO_PIN_RESET);
	return HAL_SPI_TransmitReceive(spi_nfc_Drv->bus, spi_nfc_Drv->tx_data_ptr, spi_nfc_Drv->rx_data_ptr, len, PN5180_SPI_TIMEOUT);
}

uint32_t pn5180_activate_read(uint32_t *spi_nfc_driver32)
{
SPI_NFC_DriverStruct_t *spi_nfc_Drv = (SPI_NFC_DriverStruct_t *)spi_nfc_driver32;

	if (spi_nfc_Drv->iso_card == CARD_IS_14443)
	{
		//1: sendSPI(0x11, 0x00, 0x80);
		spi_nfc_Drv->tx_data_ptr[0] = CMD_LOAD_RF_CONFIG;
		spi_nfc_Drv->tx_data_ptr[1] = 0x00;
		spi_nfc_Drv->tx_data_ptr[2] = 0x80;
		pn5180_send_spi(spi_nfc_Drv,3);
		//2: sendSPI(0x16, 0x00);
		spi_nfc_Drv->tx_data_ptr[0] = CMD_RF_ON;
		spi_nfc_Drv->tx_data_ptr[1] = 0x00;
		pn5180_send_spi(spi_nfc_Drv,2);
		//3: sendSPI(0x02, 0x19, 0xFE, 0xFF, 0xFF, 0xFF);
		spi_nfc_Drv->tx_data_ptr[0] = CMD_WRITE_REGISTER_AND_MASK;
		spi_nfc_Drv->tx_data_ptr[1] = REG_CRC_TX_CONFIG;
		spi_nfc_Drv->tx_data_ptr[2] = 0xfe;
		spi_nfc_Drv->tx_data_ptr[3] = 0xff;
		spi_nfc_Drv->tx_data_ptr[4] = 0xff;
		spi_nfc_Drv->tx_data_ptr[5] = 0xff;
		pn5180_send_spi(spi_nfc_Drv,6);
		//4: sendSPI(0x02, 0x12, 0xFE, 0xFF, 0xFF, 0xFF);
		spi_nfc_Drv->tx_data_ptr[1] = REG_CRC_RX_CONFIG;
		pn5180_send_spi(spi_nfc_Drv,6);
		//5: sendSPI(0x00, 0x03, 0xFF, 0xFF, 0x0F, 0x00);
		spi_nfc_Drv->tx_data_ptr[0] = CMD_WRITE_REGISTER;
		spi_nfc_Drv->tx_data_ptr[1] = REG_IRQ_CLEAR;
		spi_nfc_Drv->tx_data_ptr[2] = 0xff;
		spi_nfc_Drv->tx_data_ptr[3] = 0xff;
		spi_nfc_Drv->tx_data_ptr[4] = 0x0f;
		spi_nfc_Drv->tx_data_ptr[5] = 0x00;
		pn5180_send_spi(spi_nfc_Drv,6);
		//6: sendSPI(0x02, 0x00, 0xF8, 0xFF, 0xFF, 0xFF);
		spi_nfc_Drv->tx_data_ptr[0] = CMD_WRITE_REGISTER_AND_MASK;
		spi_nfc_Drv->tx_data_ptr[1] = REG_SYSTEM_CONFIG;
		spi_nfc_Drv->tx_data_ptr[2] = 0xf8;
		spi_nfc_Drv->tx_data_ptr[3] = 0xff;
		spi_nfc_Drv->tx_data_ptr[4] = 0xff;
		spi_nfc_Drv->tx_data_ptr[5] = 0xff;
		pn5180_send_spi(spi_nfc_Drv,6);
		//7: sendSPI(0x01, 0x00, 0x03, 0x00, 0x00, 0x00);
		spi_nfc_Drv->tx_data_ptr[0] = CMD_WRITE_REGISTER_OR_MASK;
		spi_nfc_Drv->tx_data_ptr[1] = REG_SYSTEM_CONFIG;
		spi_nfc_Drv->tx_data_ptr[2] = 0x03;
		spi_nfc_Drv->tx_data_ptr[3] = 0x00;
		spi_nfc_Drv->tx_data_ptr[4] = 0x00;
		spi_nfc_Drv->tx_data_ptr[5] = 0x00;
		pn5180_send_spi(spi_nfc_Drv,6);
		//8: sendSPI(0x09, 0x07, 0x26);
		spi_nfc_Drv->tx_data_ptr[0] = CMD_SEND_DATA;
		spi_nfc_Drv->tx_data_ptr[1] = REG_PADOUT;
		spi_nfc_Drv->tx_data_ptr[2] = CMD_NFC_PAYLOAD;
		pn5180_send_spi(spi_nfc_Drv,3);
		//9: waitForCardResponse();
		//10: sendSPI(0x0A, 0x00);
		spi_nfc_Drv->tx_data_ptr[0] = CMD_READ_DATA;
		spi_nfc_Drv->tx_data_ptr[1] = REG_PADOUT;
		pn5180_send_receive_spi(spi_nfc_Drv,508);
		//11: sendSPI(0x17, 0x00);
		spi_nfc_Drv->tx_data_ptr[0] = CMD_RF_OFF;
		spi_nfc_Drv->tx_data_ptr[1] = 0x00;
		pn5180_send_spi(spi_nfc_Drv,2);
		return 0;
	}
	if (spi_nfc_Drv->iso_card == CARD_IS_15693)
	{

	}
	return 1;
}

uint32_t pn5180_get_data(uint32_t *spi_nfc_driver32)
{
SPI_NFC_DriverStruct_t *spi_nfc_Drv = (SPI_NFC_DriverStruct_t *)spi_nfc_driver32;
	spi_nfc_Drv->tx_data_ptr[0] = CMD_READ_DATA;
	spi_nfc_Drv->tx_data_ptr[1] = REG_PADOUT;
	return pn5180_send_receive_spi(spi_nfc_Drv,508);
}

uint32_t pn5180_reset(uint32_t *spi_nfc_driver32)
{
SPI_NFC_DriverStruct_t *spi_nfc_Drv = (SPI_NFC_DriverStruct_t *)spi_nfc_driver32;
    HAL_GPIO_WritePin(spi_nfc_Drv->reset_port, spi_nfc_Drv->reset_bit, GPIO_PIN_RESET);
    task_delay(50);
    HAL_GPIO_WritePin(spi_nfc_Drv->reset_port, spi_nfc_Drv->reset_bit, GPIO_PIN_SET);
	return 0;
}


#endif // #ifdef NFC_ENABLED
#endif // #ifdef A_OS_SPI_ENABLED

