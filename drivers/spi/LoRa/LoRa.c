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
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/kernel_opt.h"

#ifdef A_OS_SPI_ENABLED

#include "LoRa.h"
#include "LoRa_1262.h"
#include "LoRa_1278.h"

extern	SPI_DriverStruct_t	SPI_DriverStruct[MAX_SPI_DEVICES];
extern	uint8_t				last_spi_used_handle,spi_driver_request;
LORA_Drv_TypeDef			*lora_Drv;

void LoRa_Init(void)
{
	lora_Drv->LoRa_Init();
}
void LoRa_Tx(void)
{
	lora_Drv->LoRa_Transmit(lora_Drv->TX_Buf,lora_Drv->tx_payloadLen);
}

void LoRa_SetModeReceive(void)
{
	lora_Drv->LoRa_setModeReceive();
}

uint32_t LoRa_GetRSSI(void)
{
	return lora_Drv->LoRa_GetRSSI();
}

void LoRa_SetFrequency(uint32_t frequency)
{
	lora_Drv->LoRa_SetFrequency(frequency);
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
	if (( driver_private_data->IRQ_number < EXTI0_IRQn ) || ( driver_private_data->IRQ_number > EXTI4_IRQn ))
		return DRIVER_REQUEST_FAILED;
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
		lora_Drv->LoRa_Init 			= LoRa_1262_Init;
		lora_Drv->LoRa_HandleCallback 	= LoRa_1262_HandleCallback;
		lora_Drv->LoRa_Transmit 		= LoRa_1262_Transmit;
		lora_Drv->LoRa_setModeStandby 	= LoRa_1262_setModeStandby;
		lora_Drv->LoRa_setModeReceive 	= LoRa_1262_setModeReceive;
		lora_Drv->LoRa_getstatus 		= LoRa_1262_getstatus;
		lora_Drv->LoRa_SetFrequency 	= LoRa_1262_SetFrequency;
		lora_Drv->LoRa_GetRSSI		 	= LoRa_1262_getRSSI;
		lora_Drv->status 				|= LORA_DRIVER_STATUS_IN_USE;
	}
	if ( lora_Drv->device_id == ID_SX1278 )
	{
		lora_Drv->LoRa_Init 			= LoRa_1278_init;
		lora_Drv->LoRa_HandleCallback 	= LoRa_1278_HandleCallback;
		lora_Drv->LoRa_Transmit 		= LoRa_1278_Transmit;
		lora_Drv->LoRa_setModeStandby 	= LoRa_1278_setModeStandby;
		lora_Drv->LoRa_setModeReceive 	= LoRa_1278_setModeReceive;
		lora_Drv->LoRa_getstatus 		= LoRa_1278_getstatus;
		lora_Drv->LoRa_SetFrequency 	= LoRa_1278_setFrequency;
		lora_Drv->LoRa_GetRSSI		 	= LoRa_1278_getRSSI;
		lora_Drv->status 				|= LORA_DRIVER_STATUS_IN_USE;
	}

	SPI_DriverStruct[last_spi_used_handle].status = DRIVER_STATUS_IN_USE;
	last_spi_used_handle++;
	return 0;
}

#endif // #ifdef A_OS_SPI_ENABLED
