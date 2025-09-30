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
 *  Created on: Sep 30, 2025
 *      Author: fil
 */
#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"

#ifdef A_OS_SPI_ENABLED
#ifdef	WIRELESS_NRF24L01

#include "nrf24l01.h"

extern	SPI_DriverStruct_t	SPI_DriverStruct[MAX_SPI_DEVICES];
extern	uint8_t				last_spi_used_handle,spi_driver_request;

extern	void task_delay(uint32_t tick_count);

ITCM_AREA_CODE static inline void nrf24l01_cs_high(nrf24l01_Drv_TypeDef	*nrf24l01_Drv)
{
	HAL_GPIO_WritePin(nrf24l01_Drv->CS_port,nrf24l01_Drv->CS_bit, GPIO_PIN_SET);
}

ITCM_AREA_CODE static inline void nrf24l01_cs_low(nrf24l01_Drv_TypeDef	*nrf24l01_Drv)
{
	HAL_GPIO_WritePin(nrf24l01_Drv->CS_port,nrf24l01_Drv->CS_bit, GPIO_PIN_RESET);
}

ITCM_AREA_CODE static inline void nrf24l01_ce_high(nrf24l01_Drv_TypeDef	*nrf24l01_Drv)
{
	HAL_GPIO_WritePin(nrf24l01_Drv->CE_port,nrf24l01_Drv->CE_bit, GPIO_PIN_SET);
}

ITCM_AREA_CODE static inline void nrf24l01_ce_low(nrf24l01_Drv_TypeDef	*nrf24l01_Drv)
{
	HAL_GPIO_WritePin(nrf24l01_Drv->CE_port,nrf24l01_Drv->CE_bit, GPIO_PIN_RESET);
}

ITCM_AREA_CODE static uint8_t nrf24l01_read_register(uint8_t handle,uint8_t reg)
{
uint8_t command = NRF24L01_CMD_R_REGISTER | reg;
uint8_t status;
uint8_t read_val;

nrf24l01_Drv_TypeDef	*nrf24l01_Drv = (nrf24l01_Drv_TypeDef *)SPI_DriverStruct[handle].driver_private_data;

	if ( SPI_DriverStruct[handle].process  != Asys.current_process )
		return NRF24L01_ERROR;

	nrf24l01_cs_low(nrf24l01_Drv);
	HAL_SPI_TransmitReceive(nrf24l01_Drv->spi, &command, &status, 1, nrf24l01_Drv->spi_timeout_ms);
	HAL_SPI_Receive(nrf24l01_Drv->spi, &read_val, 1, nrf24l01_Drv->spi_timeout_ms);
	nrf24l01_cs_high(nrf24l01_Drv);
	return read_val;
}

ITCM_AREA_CODE static uint8_t nrf24l01_write_register(uint8_t handle,uint8_t reg, uint8_t value)
{
uint8_t command = NRF24L01_CMD_W_REGISTER | reg;
uint8_t status;
uint8_t write_val = value;
nrf24l01_Drv_TypeDef	*nrf24l01_Drv = (nrf24l01_Drv_TypeDef *)SPI_DriverStruct[handle].driver_private_data;

	if ( SPI_DriverStruct[handle].process  != Asys.current_process )
		return NRF24L01_ERROR;

	nrf24l01_cs_low(nrf24l01_Drv);
	HAL_SPI_TransmitReceive(nrf24l01_Drv->spi, &command, &status, 1, nrf24l01_Drv->spi_timeout_ms);
	HAL_SPI_Transmit(nrf24l01_Drv->spi, &write_val, 1, nrf24l01_Drv->spi_timeout_ms);
	nrf24l01_cs_high(nrf24l01_Drv);
	return write_val;
}

ITCM_AREA_CODE static uint8_t nrf24l01_write_multiple_register(uint8_t handle,uint8_t reg, uint8_t *values,uint8_t reg_num)
{
uint8_t command = NRF24L01_CMD_W_REGISTER | reg;
uint8_t status;

nrf24l01_Drv_TypeDef	*nrf24l01_Drv = (nrf24l01_Drv_TypeDef *)SPI_DriverStruct[handle].driver_private_data;

	if ( SPI_DriverStruct[handle].process  != Asys.current_process )
		return NRF24L01_ERROR;

	nrf24l01_cs_low(nrf24l01_Drv);
	HAL_SPI_TransmitReceive(nrf24l01_Drv->spi, &command, &status, 1, nrf24l01_Drv->spi_timeout_ms);
	HAL_SPI_Transmit(nrf24l01_Drv->spi, values, reg_num, nrf24l01_Drv->spi_timeout_ms);
	nrf24l01_cs_high(nrf24l01_Drv);
	return NRF24L01_SUCCESS;
}

ITCM_AREA_CODE static uint8_t nrf24l01_flush_rx_fifo(uint8_t handle)
{
uint8_t command = NRF24L01_CMD_FLUSH_RX;
uint8_t status;

nrf24l01_Drv_TypeDef	*nrf24l01_Drv = (nrf24l01_Drv_TypeDef *)SPI_DriverStruct[handle].driver_private_data;

	if ( SPI_DriverStruct[handle].process  != Asys.current_process )
		return NRF24L01_ERROR;

	nrf24l01_cs_low(nrf24l01_Drv);
	HAL_SPI_TransmitReceive(nrf24l01_Drv->spi, &command, &status, 1, nrf24l01_Drv->spi_timeout_ms);
	nrf24l01_cs_high(nrf24l01_Drv);
	return NRF24L01_SUCCESS;
}

ITCM_AREA_CODE static uint8_t nrf24l01_flush_tx_fifo(uint8_t handle)
{
uint8_t command = NRF24L01_CMD_FLUSH_TX;
uint8_t status;

nrf24l01_Drv_TypeDef	*nrf24l01_Drv = (nrf24l01_Drv_TypeDef *)SPI_DriverStruct[handle].driver_private_data;

	if ( SPI_DriverStruct[handle].process  != Asys.current_process )
		return NRF24L01_ERROR;

	nrf24l01_cs_low(nrf24l01_Drv);
	HAL_SPI_TransmitReceive(nrf24l01_Drv->spi, &command, &status, 1, nrf24l01_Drv->spi_timeout_ms);
	nrf24l01_cs_high(nrf24l01_Drv);
	return NRF24L01_SUCCESS;
}

ITCM_AREA_CODE static uint8_t nrf24l01_read_rx_fifo(uint8_t handle,uint8_t* rx_payload)
{
uint8_t command = NRF24L01_CMD_R_RX_PAYLOAD;
uint8_t status;
nrf24l01_Drv_TypeDef	*nrf24l01_Drv = (nrf24l01_Drv_TypeDef *)SPI_DriverStruct[handle].driver_private_data;

	if ( SPI_DriverStruct[handle].process  != Asys.current_process )
		return NRF24L01_ERROR;

	nrf24l01_cs_low(nrf24l01_Drv);
	HAL_SPI_TransmitReceive(nrf24l01_Drv->spi, &command, &status, 1, nrf24l01_Drv->spi_timeout_ms);
	HAL_SPI_Receive(nrf24l01_Drv->spi, rx_payload, NRF24L01_PAYLOAD_LENGTH, nrf24l01_Drv->spi_timeout_ms);
	nrf24l01_cs_high(nrf24l01_Drv);
	return status;
}

ITCM_AREA_CODE static uint8_t nrf24l01_write_tx_fifo(uint8_t handle,uint8_t* tx_payload)
{
uint8_t command = NRF24L01_CMD_W_TX_PAYLOAD;
uint8_t status;
nrf24l01_Drv_TypeDef	*nrf24l01_Drv = (nrf24l01_Drv_TypeDef *)SPI_DriverStruct[handle].driver_private_data;

	if ( SPI_DriverStruct[handle].process  != Asys.current_process )
		return NRF24L01_ERROR;

	nrf24l01_flush_tx_fifo(handle);
	nrf24l01_cs_low(nrf24l01_Drv);
	HAL_SPI_TransmitReceive(nrf24l01_Drv->spi, &command, &status, 1, nrf24l01_Drv->spi_timeout_ms);
	HAL_SPI_Transmit(nrf24l01_Drv->spi, tx_payload, NRF24L01_PAYLOAD_LENGTH, nrf24l01_Drv->spi_timeout_ms);
	nrf24l01_cs_high(nrf24l01_Drv);
	return status;
}

ITCM_AREA_CODE uint8_t nrf24l01_rx(uint8_t handle,uint8_t* rx_payload )
{
uint8_t status;

	if ( SPI_DriverStruct[handle].process  != Asys.current_process )
		return NRF24L01_ERROR;

	nrf24l01_read_rx_fifo(handle,rx_payload);
	status = nrf24l01_read_register(handle,NRF24L01_REG_STATUS);
	nrf24l01_write_register(handle,NRF24L01_REG_STATUS, status |= 0x40);
	return status;
}

ITCM_AREA_CODE uint8_t nrf24l01_set_rx_address(uint8_t handle,uint8_t* rx_address )
{
uint8_t nrf24l01_status;

	if ( SPI_DriverStruct[handle].process  != Asys.current_process )
		return NRF24L01_ERROR;

	nrf24l01_write_multiple_register(handle,NRF24L01_REG_RX_ADDR_P0,rx_address,5);
	nrf24l01_status = nrf24l01_read_register(handle,NRF24L01_REG_STATUS);
	nrf24l01_write_register(handle,NRF24L01_REG_STATUS, nrf24l01_status |= 0x40);
	return nrf24l01_status;
}

ITCM_AREA_CODE uint8_t nrf24l01_get_tx_irq_goto_rx(uint8_t handle)
{
uint8_t nrf24l01_status;
nrf24l01_Drv_TypeDef	*nrf24l01_Drv = (nrf24l01_Drv_TypeDef *)SPI_DriverStruct[handle].driver_private_data;

	if ( SPI_DriverStruct[handle].process  != Asys.current_process )
		return NRF24L01_ERROR;

	nrf24l01_status = nrf24l01_read_register(handle,NRF24L01_REG_STATUS);
	nrf24l01_write_register(handle,NRF24L01_REG_STATUS, 0x70);
	nrf24l01_ce_low(nrf24l01_Drv);
	nrf24l01_write_register(handle,NRF24L01_REG_CONFIG, 0x00);						// power down
	nrf24l01_write_register(handle,NRF24L01_REG_CONFIG, 0x3b);						// go to rx : pup, crc en 1 bytes,rx, txdr & maxrt irq disabled
	nrf24l01_ce_high(nrf24l01_Drv);
	return nrf24l01_status;
}

ITCM_AREA_CODE uint8_t nrf24l01_get_status(uint8_t handle)
{
	return nrf24l01_read_register(handle,NRF24L01_REG_STATUS);
}

ITCM_AREA_CODE uint8_t nrf24l01_tx(uint8_t handle,uint8_t* tx_payload , uint8_t* tx_address)
{
uint8_t nrf24l01_status;
nrf24l01_Drv_TypeDef	*nrf24l01_Drv = (nrf24l01_Drv_TypeDef *)SPI_DriverStruct[handle].driver_private_data;

	if ( SPI_DriverStruct[handle].process  != Asys.current_process )
		return NRF24L01_ERROR;

	nrf24l01_ce_low(nrf24l01_Drv);
	nrf24l01_write_register(handle,NRF24L01_REG_CONFIG, 0x00);						// power down
	nrf24l01_write_register(handle,NRF24L01_REG_CONFIG, 0x4a);						// go to tx : pup, crc en 1 bytes,tx, rx dr irq disabled
	task_delay(1);															// need to wait at least 130uSec for radio switch
	nrf24l01_write_register(handle,NRF24L01_REG_STATUS, 0x70);						// clear irqs
	nrf24l01_write_multiple_register(handle,NRF24L01_REG_TX_ADDR,tx_address,5);
	nrf24l01_write_multiple_register(handle,NRF24L01_REG_RX_ADDR_P0,tx_address,5);
	nrf24l01_status = nrf24l01_read_register(handle,NRF24L01_REG_STATUS);
	nrf24l01_write_register(handle,NRF24L01_REG_STATUS, nrf24l01_status & 0x40);
	nrf24l01_write_tx_fifo(handle,tx_payload);
	nrf24l01_ce_high(nrf24l01_Drv);
	return nrf24l01_read_register(handle,NRF24L01_REG_STATUS);
}

ITCM_AREA_CODE uint8_t nrf24l01_init(uint8_t handle )
{
uint8_t nrf24l01_status;
nrf24l01_Drv_TypeDef	*nrf24l01_Drv = (nrf24l01_Drv_TypeDef *)SPI_DriverStruct[handle].driver_private_data;

	if ( SPI_DriverStruct[handle].process  != Asys.current_process )
		return NRF24L01_ERROR;


	nrf24l01_ce_low(nrf24l01_Drv);
	nrf24l01_write_register(handle,NRF24L01_REG_CONFIG, 0x08);
	nrf24l01_write_register(handle,NRF24L01_REG_EN_AA, 0x3f);
	nrf24l01_write_register(handle,NRF24L01_REG_EN_RXADDR, 0x3f);
	nrf24l01_write_register(handle,NRF24L01_REG_SETUP_AW, 0x03);
	nrf24l01_write_register(handle,NRF24L01_REG_SETUP_RETR, 0xff);	// 4000 uS , 15 retransmit
	nrf24l01_write_register(handle,NRF24L01_REG_RF_CH, nrf24l01_Drv->MHz - 2400);
	nrf24l01_write_register(handle,NRF24L01_REG_RF_SETUP, 0x07 | ((nrf24l01_Drv->bps << 3) & 0x08 ));
	nrf24l01_write_register(handle,NRF24L01_REG_DYNPD, 0x00);
	nrf24l01_write_register(handle,NRF24L01_REG_FEATURE, 0x00);

	nrf24l01_write_multiple_register(handle,NRF24L01_REG_TX_ADDR,nrf24l01_Drv->nrf_address,5);
	nrf24l01_write_multiple_register(handle,NRF24L01_REG_RX_ADDR_P0,nrf24l01_Drv->nrf_address,5);
	nrf24l01_write_multiple_register(handle,NRF24L01_REG_RX_ADDR_P5,nrf24l01_Drv->nrf_address,5);

	/* */
	nrf24l01_write_register(handle,NRF24L01_REG_RX_PW_P0, 32);
	nrf24l01_write_register(handle,NRF24L01_REG_RX_PW_P1, 32);
	nrf24l01_write_register(handle,NRF24L01_REG_RX_PW_P2, 32);
	nrf24l01_write_register(handle,NRF24L01_REG_RX_PW_P3, 32);
	nrf24l01_write_register(handle,NRF24L01_REG_RX_PW_P4, 32);
	nrf24l01_write_register(handle,NRF24L01_REG_RX_PW_P5, 32);

	nrf24l01_write_multiple_register(handle,NRF24L01_REG_TX_ADDR,nrf24l01_Drv->nrf_address,5);
	nrf24l01_write_multiple_register(handle,NRF24L01_REG_RX_ADDR_P0,nrf24l01_Drv->nrf_address,5);
	nrf24l01_write_multiple_register(handle,NRF24L01_REG_RX_ADDR_P5,nrf24l01_Drv->nrf_address,5);

	nrf24l01_flush_rx_fifo(handle);
	nrf24l01_flush_tx_fifo(handle);

	if ( nrf24l01_Drv->mode == NRF24L01_MODE_TX )
	{
		nrf24l01_write_register(handle,NRF24L01_REG_CONFIG, 0x4a);						// pup, crc en 1 bytes,tx, rx dr irq disabled
	}
	if ( nrf24l01_Drv->mode == NRF24L01_MODE_RX )
	{
		nrf24l01_write_register(handle,NRF24L01_REG_CONFIG, 0x3b);						// pup, crc en 1 bytes,rx, txdr & maxrt irq disabled
	}
	nrf24l01_write_register(handle,NRF24L01_REG_STATUS, 0x70);


	nrf24l01_status = nrf24l01_read_register(handle,NRF24L01_REG_STATUS);

	if ( nrf24l01_Drv->mode == NRF24L01_MODE_RX )
		nrf24l01_ce_high(nrf24l01_Drv);
	return nrf24l01_status;
}

ITCM_AREA_CODE uint32_t	nrf24l01_register(nrf24l01_Drv_TypeDef *private_data)
{
	if ( SPI_DriverStruct[last_spi_used_handle].process != 0 )
		return DRIVER_REQUEST_FAILED;
	if ( private_data->spi == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( private_data->CS_port == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( private_data->CE_port == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( private_data->RESET_port == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( private_data->IRQ_port == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( private_data->device_id == 0)
		return DRIVER_REQUEST_FAILED;
	if (( private_data->IRQ_number != EXTI9_5_IRQn) || ( private_data->IRQ_number != EXTI15_10_IRQn))
	{
		if (( private_data->IRQ_number < EXTI0_IRQn ) || ( private_data->IRQ_number > EXTI4_IRQn ))
			return DRIVER_REQUEST_FAILED;
	}
	if ( private_data->TX_Buf == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( private_data->RX_Buf == NULL)
		return DRIVER_REQUEST_FAILED;

	if ( private_data->MHz == 0)
		private_data->MHz = DEFAULT_FREQ;
	if ( private_data->spi_timeout_ms == 0 )
		private_data->spi_timeout_ms = NRF24L01_SPI_TIMEOUT;

	SPI_DriverStruct[last_spi_used_handle].driver_private_data = (uint32_t *)private_data;
	SPI_DriverStruct[last_spi_used_handle].process = get_current_process();
	SPI_DriverStruct[last_spi_used_handle].status = DRIVER_STATUS_IN_USE;
	nrf24l01_init(last_spi_used_handle);
	last_spi_used_handle++;
	return 0;
}

#endif	//#ifdef	WIRELESS_NRF24L01
#endif // #ifdef A_OS_SPI_ENABLED


