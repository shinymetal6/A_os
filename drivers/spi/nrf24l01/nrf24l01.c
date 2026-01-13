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
 * nrf24l01.c
 *
 *  Created on: Jan 12, 2026
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"

#ifdef A_OS_SPI_ENABLED
#ifdef WIRELESS_NRF24L01
#include "nrf24l01.h"

ITCM_AREA_CODE static inline void spi_nrf24l01_cs_low(SPI_NRF24L01_DriverStruct_t	*spi_nrf24l01_Drv)
{
	HAL_GPIO_WritePin(spi_nrf24l01_Drv->cs_port,spi_nrf24l01_Drv->cs_bit, GPIO_PIN_RESET);
}

ITCM_AREA_CODE static inline void spi_nrf24l01_cs_high(SPI_NRF24L01_DriverStruct_t	*spi_nrf24l01_Drv)
{
	HAL_GPIO_WritePin(spi_nrf24l01_Drv->cs_port,spi_nrf24l01_Drv->cs_bit, GPIO_PIN_SET);
}

ITCM_AREA_CODE static inline void spi_nrf24l01_ce_low(SPI_NRF24L01_DriverStruct_t	*spi_nrf24l01_Drv)
{
	HAL_GPIO_WritePin(spi_nrf24l01_Drv->ce_port,spi_nrf24l01_Drv->ce_bit, GPIO_PIN_RESET);
}

ITCM_AREA_CODE static inline void spi_nrf24l01_ce_high(SPI_NRF24L01_DriverStruct_t	*spi_nrf24l01_Drv)
{
	HAL_GPIO_WritePin(spi_nrf24l01_Drv->ce_port,spi_nrf24l01_Drv->ce_bit, GPIO_PIN_SET);
}


ITCM_AREA_CODE static uint32_t spi_nrf24l01_write_register(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv,uint8_t reg, uint8_t value)
{
uint8_t write_val = value;
uint8_t command = NRF24L01_CMD_W_REGISTER | reg;
	spi_nrf24l01_cs_low(spi_nrf24l01_Drv);
	spi_nrf24l01_Drv->spi_transfer_result = HAL_SPI_TransmitReceive(spi_nrf24l01_Drv->bus, &command, &spi_nrf24l01_Drv->nrf_status, 1, spi_nrf24l01_Drv->spi_timeout_ms);
	spi_nrf24l01_Drv->spi_transfer_result = HAL_SPI_Transmit(spi_nrf24l01_Drv->bus, &write_val, 1, spi_nrf24l01_Drv->spi_timeout_ms);
	spi_nrf24l01_cs_high(spi_nrf24l01_Drv);
	return write_val;
}

ITCM_AREA_CODE static uint32_t spi_nrf24l01_write_multiple_register(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv,uint8_t reg, uint8_t *values,uint8_t reg_num)
{
uint8_t command = NRF24L01_CMD_W_REGISTER | reg;
	spi_nrf24l01_cs_low(spi_nrf24l01_Drv);
	spi_nrf24l01_Drv->spi_transfer_result = HAL_SPI_TransmitReceive(spi_nrf24l01_Drv->bus, &command, &spi_nrf24l01_Drv->nrf_status, 1, spi_nrf24l01_Drv->spi_timeout_ms);
	spi_nrf24l01_Drv->spi_transfer_result = HAL_SPI_Transmit(spi_nrf24l01_Drv->bus, values, reg_num, spi_nrf24l01_Drv->spi_timeout_ms);
	spi_nrf24l01_cs_high(spi_nrf24l01_Drv);
	return NRF24L01_SUCCESS;
}

ITCM_AREA_CODE static uint32_t spi_nrf24l01_read_register(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv,uint8_t reg)
{
uint8_t read_val;
uint8_t command = NRF24L01_CMD_R_REGISTER | reg;
	spi_nrf24l01_cs_low(spi_nrf24l01_Drv);
	spi_nrf24l01_Drv->spi_transfer_result = HAL_SPI_TransmitReceive(spi_nrf24l01_Drv->bus, &command, &spi_nrf24l01_Drv->nrf_status, 1, spi_nrf24l01_Drv->spi_timeout_ms);
	spi_nrf24l01_Drv->spi_transfer_result = HAL_SPI_Receive(spi_nrf24l01_Drv->bus, &read_val, 1, spi_nrf24l01_Drv->spi_timeout_ms);
	spi_nrf24l01_cs_high(spi_nrf24l01_Drv);
	return read_val;
}

ITCM_AREA_CODE uint32_t spi_nrf24l01_flush_rx_fifo(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv)
{
uint8_t command = NRF24L01_CMD_FLUSH_RX;
	if ( spi_nrf24l01_Drv->process == get_current_process())
	{
		spi_nrf24l01_cs_low(spi_nrf24l01_Drv);
		spi_nrf24l01_Drv->spi_transfer_result = HAL_SPI_TransmitReceive(spi_nrf24l01_Drv->bus, &command, &spi_nrf24l01_Drv->nrf_status, 1, spi_nrf24l01_Drv->spi_timeout_ms);
		spi_nrf24l01_cs_high(spi_nrf24l01_Drv);
		return NRF24L01_SUCCESS;
	}
	return 1;
}

ITCM_AREA_CODE uint32_t spi_nrf24l01_flush_tx_fifo(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv)
{
uint8_t command = NRF24L01_CMD_FLUSH_TX;
	if ( spi_nrf24l01_Drv->process == get_current_process())
	{
		spi_nrf24l01_cs_low(spi_nrf24l01_Drv);
		spi_nrf24l01_Drv->spi_transfer_result = HAL_SPI_TransmitReceive(spi_nrf24l01_Drv->bus, &command, &spi_nrf24l01_Drv->nrf_status, 1, spi_nrf24l01_Drv->spi_timeout_ms);
		spi_nrf24l01_cs_high(spi_nrf24l01_Drv);
		return NRF24L01_SUCCESS;
	}
	return 1;
}

ITCM_AREA_CODE uint32_t spi_nrf24l01_read_rx_fifo(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv,uint8_t* rx_payload)
{
uint8_t command = NRF24L01_CMD_R_RX_PAYLOAD;
	if ( spi_nrf24l01_Drv->process == get_current_process())
	{
		spi_nrf24l01_cs_low(spi_nrf24l01_Drv);
		spi_nrf24l01_Drv->spi_transfer_result = HAL_SPI_TransmitReceive(spi_nrf24l01_Drv->bus, &command, &spi_nrf24l01_Drv->nrf_status, 1, spi_nrf24l01_Drv->spi_timeout_ms);
		spi_nrf24l01_Drv->spi_transfer_result = HAL_SPI_Receive(spi_nrf24l01_Drv->bus, rx_payload, NRF24L01_PAYLOAD_LENGTH, spi_nrf24l01_Drv->spi_timeout_ms);
		spi_nrf24l01_cs_high(spi_nrf24l01_Drv);
		return spi_nrf24l01_Drv->nrf_status;
	}
	return 1;
}

ITCM_AREA_CODE uint32_t spi_nrf24l01_write_tx_fifo(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv,uint8_t* tx_payload)
{
uint8_t command = NRF24L01_CMD_W_TX_PAYLOAD;
	if ( spi_nrf24l01_Drv->process == get_current_process())
	{
		spi_nrf24l01_flush_tx_fifo(spi_nrf24l01_Drv);
		spi_nrf24l01_cs_low(spi_nrf24l01_Drv);
		spi_nrf24l01_Drv->spi_transfer_result = HAL_SPI_TransmitReceive(spi_nrf24l01_Drv->bus, &command, &spi_nrf24l01_Drv->nrf_status, 1, spi_nrf24l01_Drv->spi_timeout_ms);
		spi_nrf24l01_Drv->spi_transfer_result = HAL_SPI_Transmit(spi_nrf24l01_Drv->bus, tx_payload, NRF24L01_PAYLOAD_LENGTH, spi_nrf24l01_Drv->spi_timeout_ms);
		spi_nrf24l01_cs_high(spi_nrf24l01_Drv);
		return spi_nrf24l01_Drv->nrf_status;
	}
	return 1;
}

ITCM_AREA_CODE uint32_t spi_nrf24l01_rx(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv,uint8_t* rx_payload )
{
	if ( spi_nrf24l01_Drv->process == get_current_process())
	{
		spi_nrf24l01_read_rx_fifo(spi_nrf24l01_Drv,rx_payload);
		spi_nrf24l01_Drv->nrf_status = spi_nrf24l01_read_register(spi_nrf24l01_Drv,NRF24L01_REG_STATUS);
		spi_nrf24l01_write_register(spi_nrf24l01_Drv,NRF24L01_REG_STATUS, spi_nrf24l01_Drv->nrf_status |= 0x40);
		return spi_nrf24l01_Drv->nrf_status;
	}
	return 1;
}

ITCM_AREA_CODE uint32_t spi_nrf24l01_set_rx_address(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv,uint8_t* rx_address )
{
	if ( spi_nrf24l01_Drv->process == get_current_process())
	{
		spi_nrf24l01_write_multiple_register(spi_nrf24l01_Drv,NRF24L01_REG_RX_ADDR_P5,rx_address,5);
		spi_nrf24l01_Drv->nrf_status = spi_nrf24l01_read_register(spi_nrf24l01_Drv,NRF24L01_REG_STATUS);
		spi_nrf24l01_write_register(spi_nrf24l01_Drv,NRF24L01_REG_STATUS, spi_nrf24l01_Drv->nrf_status |= 0x40);
		return spi_nrf24l01_Drv->nrf_status;
	}
	return NRF24L01_ERROR;
}

ITCM_AREA_CODE uint32_t spi_nrf24l01_set_tx_address(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv,uint8_t* tx_address )
{
	if ( spi_nrf24l01_Drv->process == get_current_process())
	{
		spi_nrf24l01_write_multiple_register(spi_nrf24l01_Drv,NRF24L01_REG_TX_ADDR,tx_address,5);
		spi_nrf24l01_Drv->nrf_status = spi_nrf24l01_read_register(spi_nrf24l01_Drv,NRF24L01_REG_STATUS);
		return spi_nrf24l01_Drv->nrf_status;
	}
	return NRF24L01_ERROR;
}

ITCM_AREA_CODE uint32_t spi_nrf24l01_get_tx_irq_goto_rx(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv)
{
	if ( spi_nrf24l01_Drv->process == get_current_process())
	{
		spi_nrf24l01_Drv->nrf_status = spi_nrf24l01_read_register(spi_nrf24l01_Drv,NRF24L01_REG_STATUS);
		spi_nrf24l01_write_register(spi_nrf24l01_Drv,NRF24L01_REG_STATUS, 0x70);
		spi_nrf24l01_ce_low(spi_nrf24l01_Drv);
		spi_nrf24l01_write_register(spi_nrf24l01_Drv,NRF24L01_REG_CONFIG, 0x00);						// power down
		spi_nrf24l01_write_register(spi_nrf24l01_Drv,NRF24L01_REG_CONFIG, 0x3b);						// go to rx : pup, crc en 1 bytes,rx, txdr & maxrt irq disabled
		spi_nrf24l01_ce_high(spi_nrf24l01_Drv);
		return spi_nrf24l01_Drv->nrf_status;
	}
	return NRF24L01_ERROR;
}

ITCM_AREA_CODE uint32_t spi_nrf24l01_get_status(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv)
{
	spi_nrf24l01_Drv->nrf_status = spi_nrf24l01_read_register(spi_nrf24l01_Drv,NRF24L01_REG_STATUS);
	return spi_nrf24l01_Drv->nrf_status;
}

ITCM_AREA_CODE uint32_t spi_nrf24l01_get_mode(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv)
{
	return spi_nrf24l01_Drv->mode;
}

ITCM_AREA_CODE uint32_t spi_nrf24l01_tx(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv,uint8_t* tx_payload , uint8_t* tx_address)
{

	if ( spi_nrf24l01_Drv->process == get_current_process())
	{
		spi_nrf24l01_ce_low(spi_nrf24l01_Drv);
		spi_nrf24l01_write_register(spi_nrf24l01_Drv,NRF24L01_REG_CONFIG, 0x00);						// power down
		spi_nrf24l01_write_register(spi_nrf24l01_Drv,NRF24L01_REG_CONFIG, 0x4a);						// go to tx : pup, crc en 1 bytes,tx, rx dr irq disabled
		task_delay(1);																	// need to wait at least 130uSec for radio switch
		spi_nrf24l01_write_register(spi_nrf24l01_Drv,NRF24L01_REG_STATUS, 0x70);						// clear irqs
		spi_nrf24l01_write_multiple_register(spi_nrf24l01_Drv,NRF24L01_REG_TX_ADDR,tx_address,5);
		spi_nrf24l01_write_multiple_register(spi_nrf24l01_Drv,NRF24L01_REG_RX_ADDR_P0,tx_address,5);
		spi_nrf24l01_Drv->nrf_status = spi_nrf24l01_read_register(spi_nrf24l01_Drv,NRF24L01_REG_STATUS);
		spi_nrf24l01_write_register(spi_nrf24l01_Drv,NRF24L01_REG_STATUS, spi_nrf24l01_Drv->nrf_status & 0x40);
		spi_nrf24l01_write_tx_fifo(spi_nrf24l01_Drv,tx_payload);
		spi_nrf24l01_ce_high(spi_nrf24l01_Drv);
		return spi_nrf24l01_read_register(spi_nrf24l01_Drv,NRF24L01_REG_STATUS);
	}
	return NRF24L01_ERROR;
}

ITCM_AREA_CODE uint32_t	spi_nrf24l01_init(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv)
{
	if ( spi_nrf24l01_Drv->process == get_current_process())
	{
		spi_nrf24l01_ce_low(spi_nrf24l01_Drv);
		spi_nrf24l01_write_register(spi_nrf24l01_Drv,NRF24L01_REG_CONFIG, 0x08);
		spi_nrf24l01_write_register(spi_nrf24l01_Drv,NRF24L01_REG_EN_AA, 0x3f);
		spi_nrf24l01_write_register(spi_nrf24l01_Drv,NRF24L01_REG_EN_RXADDR, 0x3f);
		spi_nrf24l01_write_register(spi_nrf24l01_Drv,NRF24L01_REG_SETUP_AW, 0x03);
		spi_nrf24l01_write_register(spi_nrf24l01_Drv,NRF24L01_REG_SETUP_RETR, 0xff);	// 4000 uS , 15 retransmit
		spi_nrf24l01_write_register(spi_nrf24l01_Drv,NRF24L01_REG_RF_CH, spi_nrf24l01_Drv->MHz - 2400);
		spi_nrf24l01_write_register(spi_nrf24l01_Drv,NRF24L01_REG_RF_SETUP, 0x07 | ((spi_nrf24l01_Drv->bps << 3) & 0x08 ));
		spi_nrf24l01_write_register(spi_nrf24l01_Drv,NRF24L01_REG_DYNPD, 0x00);
		spi_nrf24l01_write_register(spi_nrf24l01_Drv,NRF24L01_REG_FEATURE, 0x00);

		spi_nrf24l01_write_register(spi_nrf24l01_Drv,NRF24L01_REG_RX_PW_P0, 32);
		spi_nrf24l01_write_register(spi_nrf24l01_Drv,NRF24L01_REG_RX_PW_P1, 32);
		spi_nrf24l01_write_register(spi_nrf24l01_Drv,NRF24L01_REG_RX_PW_P2, 32);
		spi_nrf24l01_write_register(spi_nrf24l01_Drv,NRF24L01_REG_RX_PW_P3, 32);
		spi_nrf24l01_write_register(spi_nrf24l01_Drv,NRF24L01_REG_RX_PW_P4, 32);
		spi_nrf24l01_write_register(spi_nrf24l01_Drv,NRF24L01_REG_RX_PW_P5, 32);

		spi_nrf24l01_write_multiple_register(spi_nrf24l01_Drv,NRF24L01_REG_TX_ADDR,   spi_nrf24l01_Drv->nrf_tx_address,5);
		spi_nrf24l01_write_multiple_register(spi_nrf24l01_Drv,NRF24L01_REG_RX_ADDR_P0,spi_nrf24l01_Drv->nrf_rx_address,5);
		spi_nrf24l01_write_multiple_register(spi_nrf24l01_Drv,NRF24L01_REG_RX_ADDR_P5,spi_nrf24l01_Drv->nrf_rx_address,5);

		spi_nrf24l01_flush_rx_fifo(spi_nrf24l01_Drv);
		spi_nrf24l01_flush_tx_fifo(spi_nrf24l01_Drv);

		spi_nrf24l01_Drv->mode = NRF24L01_MODE_RX;
		spi_nrf24l01_write_register(spi_nrf24l01_Drv,NRF24L01_REG_CONFIG, 0x3b);						// pup, crc en 1 bytes,rx, txdr & maxrt irq disabled
		spi_nrf24l01_write_register(spi_nrf24l01_Drv,NRF24L01_REG_STATUS, 0x70);
		spi_nrf24l01_Drv->nrf_status = spi_nrf24l01_read_register(spi_nrf24l01_Drv,NRF24L01_REG_STATUS);

		spi_nrf24l01_ce_high(spi_nrf24l01_Drv);
		return spi_nrf24l01_Drv->nrf_status;
	}
	return 1;
}

ITCM_AREA_CODE uint32_t	spi_nrf24l01_register(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv)
{
SPI_NRF24L01_DriverStruct_t *eptr, *pre_eptr;

	if ( spi_nrf24l01_Drv->cs_port == NULL )
		return DRIVER_REQUEST_FAILED;
	if ( spi_nrf24l01_Drv->ce_port == NULL )
		return DRIVER_REQUEST_FAILED;
	if ( spi_nrf24l01_Drv->nrf24l01_irq_callback == NULL )
		return DRIVER_REQUEST_FAILED;

	if ( spi_nrf24l01_Drv->bus == NULL )
		return DRIVER_REQUEST_FAILED;

	if ( spi_drv_ptr == NULL)
	{
		spi_drv_ptr = (SPI_DriverStruct_t *)spi_nrf24l01_Drv;
		spi_nrf24l01_Drv->next_drv = NULL;
	}
	else
	{
		eptr = pre_eptr = (SPI_NRF24L01_DriverStruct_t *)spi_drv_ptr;
		while(eptr->next_drv != NULL)
		{
			pre_eptr = eptr;
			eptr = (SPI_NRF24L01_DriverStruct_t *)eptr->next_drv;
		}
		pre_eptr->next_drv = (uint32_t *)spi_nrf24l01_Drv;
		spi_nrf24l01_Drv->next_drv = NULL;
	}

	spi_nrf24l01_Drv->process = get_current_process();
	spi_nrf24l01_init(spi_nrf24l01_Drv);
	/* Extern IRQ allocation */
	bzero(spi_nrf24l01_Drv->nrf24l01_irq_driver,sizeof(GPIO_Interrupt_DriverStruct_t));
	spi_nrf24l01_Drv->nrf24l01_irq_driver->IRQ_port = spi_nrf24l01_Drv->irq_port;
	spi_nrf24l01_Drv->nrf24l01_irq_driver->IRQ_bit = spi_nrf24l01_Drv->irq_bit;
	spi_nrf24l01_Drv->nrf24l01_irq_driver->IRQ_type = GPIO_INT_TYPE_RISING;
	spi_nrf24l01_Drv->nrf24l01_irq_driver->flags = GPIO_INT_WAKEUP_ON_EVENT;
	spi_nrf24l01_Drv->nrf24l01_irq_driver->wakeup_id = WAKEUP_FROM_EXT_INT_IRQ;
	spi_nrf24l01_Drv->nrf24l01_irq_driver->irq_origin_struct_ptr = (uint32_t *)spi_nrf24l01_Drv;
	spi_nrf24l01_Drv->nrf24l01_irq_driver->irq_exti_callback = spi_nrf24l01_Drv->nrf24l01_irq_callback;
	return gpio_int_register(spi_nrf24l01_Drv->nrf24l01_irq_driver);
}

#endif // #ifdef WIRELESS_NRF24L01
#endif // #ifdef A_OS_SPI_ENABLED
