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
 * LoRa.c
 *
 *  Created on: Jul 30, 2025
 *      Author: fil
 */
#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"

#ifdef A_OS_SPI_ENABLED
#ifdef LORA_ENABLED

#include "LoRa.h"
#include "sx126x.h"
#include "sx127x.h"

extern	SPI_DriverStruct_t	SPI_DriverStruct[MAX_SPI_DEVICES];
extern	uint8_t				last_spi_used_handle,spi_driver_request;
LORA_Drv_TypeDef			*lora_Drv;

void LoRa_Init(void)
{
	lora_Drv->LoRa_Init();
}
void LoRa_Tx(uint8_t *buffer, uint8_t size)
{
	lora_Drv->TX_Buf = buffer;
	lora_Drv->tx_payloadLen = size;
	lora_Drv->LoRa_Transmit(lora_Drv->TX_Buf,lora_Drv->tx_payloadLen);
}

void LoRa_SetModeReceive(uint32_t timeoutMs)
{
	lora_Drv->LoRa_setModeReceive(timeoutMs);
}

void LoRa_SetModeStandby(void)
{
	lora_Drv->LoRa_setModeStandby();
}

uint32_t LoRa_GetRSSI(void)
{
	return lora_Drv->LoRa_GetRSSI();
}

void LoRa_SetFrequency(uint32_t frequency)
{
	lora_Drv->LoRa_SetFrequency(frequency);
}

void LoRa_WriteRegisters(uint16_t addr, uint8_t *buffer, uint8_t size)
{
	lora_Drv->LoRa_WriteRegisters(addr, buffer, size);
}

void LoRa_ReadRegisters(uint16_t addr, uint8_t *buffer, uint8_t size)
{
	lora_Drv->LoRa_ReadRegisters(addr, buffer, size);
}

void LoRa_WriteSingleRegister(uint16_t addr, uint8_t data)
{
	lora_Drv->LoRa_WriteRegisters(addr, &data,1);
}

uint8_t LoRa_ReadSingleRegister(uint16_t addr)
{
uint8_t buffer;
	lora_Drv->LoRa_ReadRegisters(addr, &buffer, 1);
	return buffer;
}


ITCM_AREA_CODE uint32_t	LoRa_register(LORA_Drv_TypeDef *driver_private_data)
{
	if ( SPI_DriverStruct[last_spi_used_handle].process != 0 )
		return DRIVER_REQUEST_FAILED;
	if ( driver_private_data->spi == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( driver_private_data->CS_port == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( driver_private_data->RADIO_BUSY_port == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( driver_private_data->device_id == 0)
		return DRIVER_REQUEST_FAILED;
	if (( driver_private_data->IRQ_number != EXTI9_5_IRQn) || ( driver_private_data->IRQ_number != EXTI15_10_IRQn))
	{
		if (( driver_private_data->IRQ_number < EXTI0_IRQn ) || ( driver_private_data->IRQ_number > EXTI4_IRQn ))
			return DRIVER_REQUEST_FAILED;
	}
	if ( driver_private_data->TX_Buf == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( driver_private_data->RX_Buf == NULL)
		return DRIVER_REQUEST_FAILED;

	SPI_DriverStruct[last_spi_used_handle].driver_private_data = (uint32_t *)driver_private_data;
	lora_Drv = (LORA_Drv_TypeDef *)SPI_DriverStruct[last_spi_used_handle].driver_private_data;
	lora_Drv->spi_timeout_ms = 100;
	SPI_DriverStruct[last_spi_used_handle].process = get_current_process();
	SPI_DriverStruct[last_spi_used_handle].driver_private_data = (uint32_t *)driver_private_data;
	lora_Drv->device_id = driver_private_data->device_id;
	lora_Drv->IRQ_number 			= driver_private_data->IRQ_number;
	if (( lora_Drv->device_id == ID_SX1261 ) || ( lora_Drv->device_id == ID_SX1262 ))
	{
		lora_Drv->LoRa_Init 			= sx126x_init;
		lora_Drv->LoRa_HandleCallback 	= sx126x_handle_dio1_irq;
		lora_Drv->LoRa_Transmit 		= sx126x_transmit;
		lora_Drv->LoRa_setModeStandby 	= sx126x_set_standby;
		lora_Drv->LoRa_setModeReceive 	= sx126x_set_rx;
		lora_Drv->LoRa_getstatus 		= sx126x_get_irq_status;
		lora_Drv->LoRa_SetFrequency 	= sx126x_set_rf_frequency;
		lora_Drv->LoRa_GetRSSI		 	= sx126x_getRSSI;
		lora_Drv->LoRa_WriteRegisters	= sx126x_write_register;
		lora_Drv->LoRa_ReadRegisters	= sx126x_read_register;
		lora_Drv->LoRa_WriteSingleRegister	= sx126x_write_single_register;
		lora_Drv->LoRa_ReadSingleRegister= sx126x_read_single_register;
		lora_Drv->status 				|= LORA_DRIVER_STATUS_IN_USE;
	}
	/*
	if ( lora_Drv->device_id == ID_SX1278 )
	{
		lora_Drv->LoRa_Init 			= sx127x_init;
		lora_Drv->LoRa_HandleCallback 	= sx127x_handle_dio0_irq;
		lora_Drv->LoRa_Transmit 		= sx127x_transmit;
		lora_Drv->LoRa_setModeStandby 	= sx127x_set_standby;
		lora_Drv->LoRa_setModeReceive 	= LoRa_1278_setModeReceive;
		lora_Drv->LoRa_getstatus 		= LoRa_1278_getstatus;
		lora_Drv->LoRa_SetFrequency 	= LoRa_1278_setFrequency;
		lora_Drv->LoRa_GetRSSI		 	= LoRa_1278_getRSSI;
		lora_Drv->status 				|= LORA_DRIVER_STATUS_IN_USE;
	}
	*/
	SPI_DriverStruct[last_spi_used_handle].status = DRIVER_STATUS_IN_USE;
	last_spi_used_handle++;
	return 0;
}
#endif // #ifdef LORA_ENABLED

#endif // #ifdef A_OS_SPI_ENABLED
