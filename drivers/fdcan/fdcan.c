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
	return HAL_FDCAN_AddMessageToTxFifoQ(private_data->hfdcan, &private_data->TxHeader, (const uint8_t *)&private_data->TxData);
}


ITCM_AREA_CODE uint32_t	can_register(CAN_Drv_TypeDef *private_data)
{
uint8_t channel = (private_data->hfdcan == &hfdcan1) ? 0 : 1;

	if ( private_data->flags != 0 )
	{
		if ( private_data->wakeup_id == 0)
			return DRIVER_REQUEST_FAILED;
	}
	private_data->status |= FDCAN_DRIVER_STATUS_IN_USE;

	CAN_DriverStruct[channel].process = get_current_process();
	CAN_DriverStruct[channel].private_data = (uint32_t *)private_data;

	CAN_DriverStruct[channel].status = DRIVER_STATUS_IN_USE;
	CAN_DriverStruct[channel].can_send = int_can_send;

	 /* Bit time configuration:
	   fdcan_ker_ck               = 40 MHz
	   Time_quantum (tq)          = 25 ns
	   Synchronization_segment    = 1 tq
	   Propagation_segment        = 23 tq
	   Phase_segment_1            = 8 tq
	   Phase_segment_2            = 8 tq
	   Synchronization_Jump_width = 8 tq
	   Bit_length                 = 40 tq = 1 \B5s
	   Bit_rate                   = 1 MBit/s
	 */

	private_data->TxHeader.Identifier = 0x321;
	private_data->TxHeader.IdType = FDCAN_STANDARD_ID;
	private_data->TxHeader.TxFrameType = FDCAN_DATA_FRAME;
	private_data->TxHeader.DataLength = FDCAN_DLC_BYTES_2;
	private_data->TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	private_data->TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
	private_data->TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
	private_data->TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	private_data->TxHeader.MessageMarker = 0;

	 /* Configure Rx filter */
	private_data->sFilterConfig.IdType = FDCAN_STANDARD_ID;
	private_data->sFilterConfig.FilterIndex = 0;
	private_data->sFilterConfig.FilterType = FDCAN_FILTER_MASK;
	private_data->sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	private_data->sFilterConfig.FilterID1 = 0x321;
	private_data->sFilterConfig.FilterID2 = 0x7FF;
	if (HAL_FDCAN_ConfigFilter(&hfdcan1, &private_data->sFilterConfig) != HAL_OK)
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
CAN_Drv_TypeDef		*can_Drv = (CAN_Drv_TypeDef	*)CAN_DriverStruct[channel].private_data;

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


