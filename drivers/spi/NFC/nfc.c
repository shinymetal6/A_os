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
 * nfc.c
 *
 *  Created on: Jan 12, 2026
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"

#ifdef A_OS_SPI_ENABLED
#ifdef NFC_ENABLED
#include "nfc.h"

ITCM_AREA_CODE uint32_t	spi_nfc_init(SPI_NFC_DriverStruct_t *spi_nfc_Drv)
{
	if ( spi_nfc_Drv->nfc_init != NULL )
		return spi_nfc_Drv->nfc_init();
	return 1;
}

ITCM_AREA_CODE uint32_t	spi_nfc_reset(SPI_NFC_DriverStruct_t *spi_nfc_Drv)
{
	if ( spi_nfc_Drv->nfc_reset != NULL )
		return spi_nfc_Drv->nfc_reset();
	return 1;
}

ITCM_AREA_CODE uint32_t	spi_nfc_write_register(SPI_NFC_DriverStruct_t *spi_nfc_Drv,uint8_t address,uint8_t byte0,uint8_t byte1,uint8_t byte2,uint8_t byte3)
{
	if ( spi_nfc_Drv->nfc_write_register != NULL )
		return spi_nfc_Drv->nfc_write_register( address, byte0, byte1, byte2, byte3);
	return 1;
}

ITCM_AREA_CODE uint32_t	spi_nfc_send_data(SPI_NFC_DriverStruct_t *spi_nfc_Drv,uint8_t NumberOfValidBitsInLastByte,uint8_t *tx_data_ptr)
{
	if ( spi_nfc_Drv->nfc_send_data != NULL )
	{
		if ( tx_data_ptr != NULL )
		{
			spi_nfc_Drv->tx_data_ptr = tx_data_ptr;
			return spi_nfc_Drv->nfc_send_data( NumberOfValidBitsInLastByte,spi_nfc_Drv->tx_data_ptr);
		}
	}
	return 1;
}

ITCM_AREA_CODE uint32_t	spi_nfc_read_data(SPI_NFC_DriverStruct_t *spi_nfc_Drv,uint8_t *rx_data_ptr)
{
	if ( spi_nfc_Drv->nfc_read_data != NULL )
	{
		if ( spi_nfc_Drv->rx_data_ptr != NULL )
		{
			spi_nfc_Drv->rx_data_ptr = rx_data_ptr;
			return spi_nfc_Drv->nfc_read_data(spi_nfc_Drv->rx_data_ptr);
		}
	}
	return 1;
}

ITCM_AREA_CODE uint32_t	spi_nfc_load_rf_config(SPI_NFC_DriverStruct_t *spi_nfc_Drv)
{
	if ( spi_nfc_Drv->nfc_load_rf_config != NULL )
		return spi_nfc_Drv->nfc_load_rf_config();
	return 1;
}

ITCM_AREA_CODE uint32_t	spi_nfc_rf_on(SPI_NFC_DriverStruct_t *spi_nfc_Drv)
{
	if ( spi_nfc_Drv->nfc_rf_on != NULL )
		return spi_nfc_Drv->nfc_rf_on();
	return 1;
}

ITCM_AREA_CODE uint32_t	spi_nfc_rf_off(SPI_NFC_DriverStruct_t *spi_nfc_Drv)
{
	if ( spi_nfc_Drv->nfc_rf_off != NULL )
		return spi_nfc_Drv->nfc_rf_off();
	return 1;
}


ITCM_AREA_CODE uint32_t	spi_nfc_register(SPI_NFC_DriverStruct_t *spi_nfc_Drv)
{
SPI_NFC_DriverStruct_t *eptr, *pre_eptr;

	if ( spi_nfc_Drv->cs_port == NULL )
		return DRIVER_REQUEST_FAILED;
	if ( spi_nfc_Drv->reset_port == NULL )
		return DRIVER_REQUEST_FAILED;

	if ( spi_nfc_Drv->bus == NULL )
		return DRIVER_REQUEST_FAILED;

	if ( spi_drv_ptr == NULL)
	{
		spi_drv_ptr = (SPI_DriverStruct_t *)spi_nfc_Drv;
		spi_nfc_Drv->next_drv = NULL;
	}
	else
	{
		eptr = pre_eptr = (SPI_NFC_DriverStruct_t *)spi_drv_ptr;
		while(eptr->next_drv != NULL)
		{
			pre_eptr = eptr;
			eptr = (SPI_NFC_DriverStruct_t *)eptr->next_drv;
		}
		pre_eptr->next_drv = (uint32_t *)spi_nfc_Drv;
		spi_nfc_Drv->next_drv = NULL;
	}

	spi_nfc_Drv->process = get_current_process();

	switch(spi_nfc_Drv->nfc_model)
	{
	case NFC_IS_PN5180 :
		PN5180_cs_port = spi_nfc_Drv->cs_port;
		PN5180_cs_bit = spi_nfc_Drv->cs_bit;
		PN5180_reset_port = spi_nfc_Drv->reset_port;
		PN5180_reset_bit = spi_nfc_Drv->reset_bit;
		PN5180_spi_port = spi_nfc_Drv->bus;
		if ( spi_nfc_Drv->reset_time == 0 )
			spi_nfc_Drv->reset_time = DEFAULT_RESET_TIME;
		PN5180_reset_time = spi_nfc_Drv->reset_time;

		spi_nfc_Drv->nfc_init 			= pn5180_init;
		spi_nfc_Drv->nfc_reset			= pn5180_reset;
		spi_nfc_Drv->nfc_write_register	= pn5180_write_register;
		spi_nfc_Drv->nfc_send_data		= pn5180_send_data;
		spi_nfc_Drv->nfc_read_data		= pn5180_read_data;
		spi_nfc_Drv->nfc_load_rf_config	= pn5180_load_rf_config;
		spi_nfc_Drv->nfc_rf_on			= pn5180_rf_on;
		spi_nfc_Drv->nfc_rf_off			= pn5180_rf_off;

		PN5180_flags = (uint8_t *)&spi_nfc_Drv->flags;
		spi_nfc_Drv->dma_timeout = SPI_NFC_DMA_TIMEOUT;
		PN5180_dma_timeout = (uint8_t *)&spi_nfc_Drv->dma_timeout;
		return 0;
		break;
	default :
		return DRIVER_REQUEST_FAILED;
	}
	return DRIVER_REQUEST_FAILED;
}

#endif // #ifdef NFC_ENABLED
#endif // #ifdef A_OS_SPI_ENABLED
