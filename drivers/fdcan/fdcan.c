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
 * fdcan.c
 *
 *  Created on: Jun 23, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"
#ifdef A_OS_CAN_ENABLED

#include "fdcan.h"
#include <string.h>
CAN_DriverStruct_t	CAN_DriverStruct[2];

ITCM_AREA_CODE static uint32_t int_can_send(CAN_Drv_TypeDef *private_data)
{
	return HAL_FDCAN_AddMessageToTxFifoQ(private_data->hfdcan, private_data->TxHeader, (const uint8_t *)&private_data->TxData);
}

ITCM_AREA_CODE static uint32_t int_can_update_filter(CAN_Drv_TypeDef *private_data,FDCAN_FilterTypeDef *FDCAN_Filter)
{
uint32_t	ret_val;
	if ( (ret_val = HAL_FDCAN_Stop(private_data->hfdcan)) != HAL_OK)
		return ret_val;
	private_data->FilterConfig->FilterType = FDCAN_FILTER_DISABLE;
	if ( (ret_val = HAL_FDCAN_ConfigFilter(&hfdcan1, private_data->FilterConfig)) != HAL_OK)
		return ret_val;
	private_data->FilterConfig->FilterType = FDCAN_FILTER_MASK;
	if ( (ret_val = HAL_FDCAN_ConfigFilter(&hfdcan1, private_data->FilterConfig)) != HAL_OK)
		return ret_val;
	return HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
}

ITCM_AREA_CODE uint32_t	can_register(CAN_Drv_TypeDef *private_data)
{
uint8_t channel = (private_data->hfdcan == &hfdcan1) ? 0 : 1;

	if ( private_data->flags != 0 )
	{
		if ( private_data->wakeup_id == 0)
			return DRIVER_REQUEST_FAILED;
		if ( private_data->FilterConfig == NULL)
			return DRIVER_REQUEST_FAILED;
		if ( private_data->TxHeader == NULL)
			return DRIVER_REQUEST_FAILED;
		if ( private_data->RxHeader == NULL)
			return DRIVER_REQUEST_FAILED;
	}
	private_data->status |= FDCAN_DRIVER_STATUS_IN_USE;

	CAN_DriverStruct[channel].process = get_current_process();
	CAN_DriverStruct[channel].private_data = (uint32_t *)private_data;

	CAN_DriverStruct[channel].status = DRIVER_STATUS_IN_USE;
	CAN_DriverStruct[channel].can_send = int_can_send;
	CAN_DriverStruct[channel].can_update_filter = int_can_update_filter;


	if (HAL_FDCAN_ConfigFilter(&hfdcan1, private_data->FilterConfig) != HAL_OK)
		return 1;
	if ( HAL_FDCAN_Start(private_data->hfdcan) != HAL_OK )
		return 1;
	return HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
}

ITCM_AREA_CODE uint32_t can_send(CAN_Drv_TypeDef *private_data)
{
	if ( CAN_DriverStruct[private_data->channel-1].can_send != NULL )
		return CAN_DriverStruct[private_data->channel-1].can_send(private_data);
	return 1;
}

ITCM_AREA_CODE uint32_t can_update_filter(CAN_Drv_TypeDef *private_data, FDCAN_FilterTypeDef *FDCAN_Filter)
{
	if ( CAN_DriverStruct[private_data->channel-1].can_update_filter != NULL )
		return CAN_DriverStruct[private_data->channel-1].can_update_filter(private_data,FDCAN_Filter);
	return 1;
}

ITCM_AREA_CODE uint32_t can_update_header(CAN_Drv_TypeDef *private_data, FDCAN_TxHeaderTypeDef *TxHeader)
{
	memcpy(&private_data->TxHeader,TxHeader,sizeof(FDCAN_TxHeaderTypeDef));
	return 0;
}

/* Interrupts */

void HAL_FDCAN_TxEventFifoCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t TxEventFifoITs)
{
uint8_t channel = (hfdcan == &hfdcan1) ? 0 : 1;
CAN_Drv_TypeDef		*can_Drv = (CAN_Drv_TypeDef	*)CAN_DriverStruct[channel].private_data;

	if (( can_Drv->flags & FDCAN_WAKEUP_ON_TX) == FDCAN_WAKEUP_ON_TX)
	{
		activate_process(CAN_DriverStruct[channel].process,can_Drv->wakeup_id,FDCAN_WAKEUP_ON_TX);
	}
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
uint8_t channel = (hfdcan == &hfdcan1) ? 0 : 1;
CAN_Drv_TypeDef	*can_Drv = (CAN_Drv_TypeDef	*)CAN_DriverStruct[channel].private_data;

	HAL_FDCAN_GetRxMessage(can_Drv->hfdcan, FDCAN_RX_FIFO0, can_Drv->RxHeader, can_Drv->RxData);
	if (( can_Drv->flags & FDCAN_WAKEUP_ON_RX0) == FDCAN_WAKEUP_ON_RX0 )
		activate_process(CAN_DriverStruct[channel].process,can_Drv->wakeup_id,FDCAN_WAKEUP_ON_RX0);
}

void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs)
{
uint8_t channel = (hfdcan == &hfdcan1) ? 0 : 1;
CAN_Drv_TypeDef		*can_Drv = (CAN_Drv_TypeDef	*)CAN_DriverStruct[channel].private_data;

	if (( can_Drv->flags & FDCAN_WAKEUP_ON_RX1) == FDCAN_WAKEUP_ON_RX1)
		activate_process(CAN_DriverStruct[channel].process,can_Drv->wakeup_id,FDCAN_WAKEUP_ON_RX1);
}
#endif // #ifdef A_OS_CAN_ENABLED


