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

ITCM_AREA_CODE uint32_t	spi_nfc_activate_read(SPI_NFC_DriverStruct_t *spi_nfc_Drv)
{
	if ( spi_nfc_Drv->nfc_activate_read != NULL )
		return spi_nfc_Drv->nfc_activate_read((uint32_t *)spi_nfc_Drv);
	return 1;
}


ITCM_AREA_CODE uint32_t	spi_nfc_register(SPI_NFC_DriverStruct_t *spi_nfc_Drv)
{
SPI_NFC_DriverStruct_t *eptr, *pre_eptr;

	if ( spi_nfc_Drv->cs_port == NULL )
		return DRIVER_REQUEST_FAILED;
	if ( spi_nfc_Drv->reset_port == NULL )
		return DRIVER_REQUEST_FAILED;
	if ( spi_nfc_Drv->busy_port == NULL )
		return DRIVER_REQUEST_FAILED;
	if ( spi_nfc_Drv->irq_port == NULL )
		return DRIVER_REQUEST_FAILED;
	if ( spi_nfc_Drv->nfc_irq_driver == NULL )
		return DRIVER_REQUEST_FAILED;
	if ((spi_nfc_Drv->iso_card != CARD_IS_14443) && (spi_nfc_Drv->iso_card != CARD_IS_15693))
		return DRIVER_REQUEST_FAILED;

	if ( spi_nfc_Drv->bus == NULL )
		return DRIVER_REQUEST_FAILED;
	if ( spi_nfc_Drv->tx_data_ptr == NULL )
		return DRIVER_REQUEST_FAILED;
	if ( spi_nfc_Drv->rx_data_ptr == NULL )
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
		if ( spi_nfc_Drv->reset_time == 0 )
			spi_nfc_Drv->reset_time = DEFAULT_RESET_TIME;
		spi_nfc_Drv->dma_timeout = SPI_NFC_DMA_TIMEOUT;
		spi_nfc_Drv->nfc_activate_read = pn5180_activate_read;
		/* Extern IRQ allocation */
		bzero(spi_nfc_Drv->nfc_irq_driver,sizeof(GPIO_Interrupt_DriverStruct_t));
		spi_nfc_Drv->nfc_irq_driver->IRQ_port = spi_nfc_Drv->irq_port;
		spi_nfc_Drv->nfc_irq_driver->IRQ_bit = spi_nfc_Drv->irq_bit;
		spi_nfc_Drv->nfc_irq_driver->IRQ_type = GPIO_INT_TYPE_RISING;
		spi_nfc_Drv->nfc_irq_driver->flags = GPIO_INT_WAKEUP_ON_EVENT;
		spi_nfc_Drv->nfc_irq_driver->wakeup_id = WAKEUP_FROM_EXT_INT_IRQ;
		spi_nfc_Drv->nfc_irq_driver->irq_origin_struct_ptr = (uint32_t *)spi_nfc_Drv;
		gpio_int_register(spi_nfc_Drv->nfc_irq_driver);
		return 0;
		break;
	default :
		return DRIVER_REQUEST_FAILED;
	}
	return DRIVER_REQUEST_FAILED;
}

#endif // #ifdef NFC_ENABLED
#endif // #ifdef A_OS_SPI_ENABLED
