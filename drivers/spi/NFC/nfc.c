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

ITCM_AREA_CODE uint32_t	spi_nfc_ISO14443_init(SPI_NFC_DriverStruct_t *spi_nfc_Drv)
{
	if ( spi_nfc_Drv->nfc_ISO14443_init != NULL )
		return spi_nfc_Drv->nfc_ISO14443_init((uint32_t *)spi_nfc_Drv);
	return 1;
}

ITCM_AREA_CODE uint8_t	spi_nfc_ISO14443_send_REQA(SPI_NFC_DriverStruct_t *spi_nfc_Drv)
{
	if ( spi_nfc_Drv->nfc_ISO14443_send_REQA != NULL )
		return spi_nfc_Drv->nfc_ISO14443_send_REQA((uint32_t *)spi_nfc_Drv);
	return 1;
}

ITCM_AREA_CODE uint32_t	spi_nfc_send_ISO14443_AntiCollision(SPI_NFC_DriverStruct_t *spi_nfc_Drv)
{
	if ( spi_nfc_Drv->nfc_send_ISO14443_AntiCollision != NULL )
		return spi_nfc_Drv->nfc_send_ISO14443_AntiCollision((uint32_t *)spi_nfc_Drv);
	return 1;
}

ITCM_AREA_CODE uint32_t	spi_nfc_rf_on(SPI_NFC_DriverStruct_t *spi_nfc_Drv)
{
	if ( spi_nfc_Drv->nfc_rf_on != NULL )
		return spi_nfc_Drv->nfc_rf_on((uint32_t *)spi_nfc_Drv);
	return 1;
}

ITCM_AREA_CODE uint32_t	spi_nfc_rf_off(SPI_NFC_DriverStruct_t *spi_nfc_Drv)
{
	if ( spi_nfc_Drv->nfc_rf_off != NULL )
		return spi_nfc_Drv->nfc_rf_off((uint32_t *)spi_nfc_Drv);
	return 1;
}

ITCM_AREA_CODE uint32_t	spi_nfc_get_hwdata(SPI_NFC_DriverStruct_t *spi_nfc_Drv,uint32_t addr, uint8_t *buffer, uint32_t len)
{
	if ( spi_nfc_Drv->nfc_hw_data != NULL )
		return spi_nfc_Drv->nfc_hw_data((uint32_t *)spi_nfc_Drv,addr,buffer,len);
	return 1;
}

ITCM_AREA_CODE uint32_t	spi_nfc_reset(SPI_NFC_DriverStruct_t *spi_nfc_Drv)
{
	if ( spi_nfc_Drv->nfc_reset != NULL )
		return spi_nfc_Drv->nfc_reset((uint32_t *)spi_nfc_Drv);
	return 1;
}


ITCM_AREA_CODE uint32_t	spi_ISO14443_Authenticate(SPI_NFC_DriverStruct_t *spi_nfc_Drv,uint8_t *Key,uint8_t KeyType, uint8_t BlockNo)
{
	if ( spi_nfc_Drv->nfc_ISO14443_Authenticate != NULL )
		return spi_nfc_Drv->nfc_ISO14443_Authenticate((uint32_t *)spi_nfc_Drv,Key,KeyType,BlockNo);
	return 1;
}

ITCM_AREA_CODE uint32_t	spi_ISO14443_BlockRead(SPI_NFC_DriverStruct_t *spi_nfc_Drv, uint8_t BlockNo,uint8_t *Block)
{
	if ( spi_nfc_Drv->nfc_ISO14443_BlockRead != NULL )
		return spi_nfc_Drv->nfc_ISO14443_BlockRead((uint32_t *)spi_nfc_Drv,BlockNo,Block);
	return 1;
}

ITCM_AREA_CODE uint32_t	spi_ISO14443_BlockWrite(SPI_NFC_DriverStruct_t *spi_nfc_Drv, uint8_t BlockNo,uint8_t *Block)
{
	if ( spi_nfc_Drv->nfc_ISO14443_BlockWrite != NULL )
		return spi_nfc_Drv->nfc_ISO14443_BlockWrite((uint32_t *)spi_nfc_Drv,BlockNo,Block);
	return 1;
}

ITCM_AREA_CODE uint32_t	spi_nfc_register(SPI_NFC_DriverStruct_t *spi_nfc_Drv)
{
SPI_NFC_DriverStruct_t *eptr;

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
	if ((spi_nfc_Drv->iso_card != NFC_CARD_IS_14443) && (spi_nfc_Drv->iso_card != NFC_CARD_IS_15693))
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
		eptr = (SPI_NFC_DriverStruct_t *)spi_drv_ptr;
		while(eptr->next_drv != NULL)
			eptr = (SPI_NFC_DriverStruct_t *)eptr->next_drv;
		eptr->next_drv = (uint32_t *)spi_nfc_Drv;
		spi_nfc_Drv->next_drv = NULL;
	}

	spi_nfc_Drv->process = get_current_process();

	switch(spi_nfc_Drv->nfc_model)
	{
	case NFC_IS_PN5180 :
		spi_nfc_Drv->nfc_ISO14443_init = pn5180_ISO14443_init;
		spi_nfc_Drv->nfc_ISO14443_send_REQA = pn5180_send_ISO14443_REQA;
		spi_nfc_Drv->nfc_send_ISO14443_AntiCollision = pn5180_send_ISO14443_AntiCollision;
		spi_nfc_Drv->nfc_ISO14443_Authenticate = pn5180_ISO14443_Authenticate;
		spi_nfc_Drv->nfc_ISO14443_BlockRead = pn5180_ISO14443_BlockRead;
		spi_nfc_Drv->nfc_ISO14443_BlockWrite = pn5180_ISO14443_BlockWrite;
		spi_nfc_Drv->nfc_rf_on = pn5180_set_rf_on;
		spi_nfc_Drv->nfc_rf_off = pn5180_set_rf_off;
		spi_nfc_Drv->nfc_hw_data = pn5180_readEEprom;
		spi_nfc_Drv->nfc_reset = pn5180_reset;
		if ( spi_nfc_Drv->reset_time == 0 )
			spi_nfc_Drv->reset_time = NFC_DEFAULT_RESET_TIME;
		spi_nfc_reset(spi_nfc_Drv);
		spi_nfc_Drv->dma_timeout = SPI_NFC_DMA_TIMEOUT;

		/* Extern IRQ allocation */
		bzero(spi_nfc_Drv->nfc_irq_driver,sizeof(GPIO_Interrupt_DriverStruct_t));
		spi_nfc_Drv->nfc_irq_driver->IRQ_port = spi_nfc_Drv->irq_port;
		spi_nfc_Drv->nfc_irq_driver->IRQ_bit = spi_nfc_Drv->irq_bit;
		spi_nfc_Drv->nfc_irq_driver->IRQ_type = GPIO_INT_TYPE_RISING;
		spi_nfc_Drv->nfc_irq_driver->flags = GPIO_INT_WAKEUP_ON_EVENT;
		spi_nfc_Drv->nfc_irq_driver->wakeup_id = WAKEUP_FROM_EXT_INT_IRQ;
		spi_nfc_Drv->nfc_irq_driver->irq_origin_struct_ptr = (uint32_t *)spi_nfc_Drv;
		spi_nfc_Drv->nfc_irq_driver->irq_exti_callback = spi_nfc_Drv->nfc_irq_callback;

		gpio_int_register(spi_nfc_Drv->nfc_irq_driver);
		spi_nfc_ISO14443_init(spi_nfc_Drv);
		return 0;
		break;
	default :
		return DRIVER_REQUEST_FAILED;
	}
	return DRIVER_REQUEST_FAILED;
}

#endif // #ifdef NFC_ENABLED
#endif // #ifdef A_OS_SPI_ENABLED
