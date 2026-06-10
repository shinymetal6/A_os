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
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#ifdef A_OS_CAN_ENABLED

#include "fdcan.h"
#include <string.h>
FDCAN_DriverStruct_t	*fdcan_drv_ptr = NULL;



ITCM_AREA_CODE uint32_t can_send(FDCAN_DriverStruct_t *fdcan_drv)
{
	return HAL_FDCAN_AddMessageToTxFifoQ(fdcan_drv->hfdcan, fdcan_drv->TxHeader, (const uint8_t *)&fdcan_drv->TxData);

}

ITCM_AREA_CODE uint32_t can_update_filter(FDCAN_DriverStruct_t *fdcan_drv, FDCAN_FilterTypeDef *FDCAN_Filter)
{
uint32_t	ret_val;
	if ( (ret_val = HAL_FDCAN_Stop(fdcan_drv->hfdcan)) != HAL_OK)
		return ret_val;
	fdcan_drv->FilterConfig->FilterType = FDCAN_FILTER_DISABLE;
	if ( (ret_val = HAL_FDCAN_ConfigFilter(&hfdcan1, fdcan_drv->FilterConfig)) != HAL_OK)
		return ret_val;
	fdcan_drv->FilterConfig->FilterType = FDCAN_FILTER_MASK;
	if ( (ret_val = HAL_FDCAN_ConfigFilter(&hfdcan1, fdcan_drv->FilterConfig)) != HAL_OK)
		return ret_val;
	return HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
}

ITCM_AREA_CODE uint32_t can_update_header(FDCAN_DriverStruct_t *fdcan_drv, FDCAN_TxHeaderTypeDef *TxHeader)
{
	memcpy(&fdcan_drv->TxHeader,TxHeader,sizeof(FDCAN_TxHeaderTypeDef));
	return 0;
}

ITCM_AREA_CODE uint32_t	can_register(FDCAN_DriverStruct_t *fdcan_drv)
{
FDCAN_DriverStruct_t *eptr, *pre_eptr;
	if ( fdcan_drv->flags != 0 )
	{
		if ( fdcan_drv->wakeup_id == 0)
			return DRIVER_REQUEST_FAILED;
		if ( fdcan_drv->FilterConfig == NULL)
			return DRIVER_REQUEST_FAILED;
		if ( fdcan_drv->TxHeader == NULL)
			return DRIVER_REQUEST_FAILED;
		if ( fdcan_drv->RxHeader == NULL)
			return DRIVER_REQUEST_FAILED;
	}

	if ( fdcan_drv_ptr == NULL)
	{
		fdcan_drv_ptr = fdcan_drv;
		fdcan_drv->next_fdcan = NULL;
	}
	else
	{
		eptr = pre_eptr = fdcan_drv_ptr;
		while(eptr->next_fdcan != NULL)
		{
			pre_eptr = eptr;
			eptr = (FDCAN_DriverStruct_t *)eptr->next_fdcan;
		}
		pre_eptr->next_fdcan = (uint32_t *)fdcan_drv;
		fdcan_drv->next_fdcan = NULL;
	}

	fdcan_drv->status |= FDCAN_DRIVER_STATUS_IN_USE;
	fdcan_drv->process = get_current_process();

	if (HAL_FDCAN_ConfigFilter(&hfdcan1, fdcan_drv->FilterConfig) != HAL_OK)
		return 1;
	if ( HAL_FDCAN_Start(fdcan_drv->hfdcan) != HAL_OK )
		return 1;

	return HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
}

/* Interrupts */

ITCM_AREA_CODE FDCAN_DriverStruct_t *get_fdcan_ptr_from_workers(FDCAN_HandleTypeDef *hfdcan)
{
FDCAN_DriverStruct_t *eptr, *pre_eptr;

	eptr = pre_eptr = fdcan_drv_ptr;
	while(eptr != NULL)
	{
		if ( eptr->hfdcan == hfdcan )
			return eptr;
		pre_eptr = eptr;
		if ( eptr->next_fdcan == NULL )
			return NULL;
		eptr = (FDCAN_DriverStruct_t *)eptr->next_fdcan;
	}
	return NULL;
}

void HAL_FDCAN_TxEventFifoCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t TxEventFifoITs)
{
FDCAN_DriverStruct_t	*can_drv = get_fdcan_ptr_from_workers(hfdcan);

	if (( can_drv->flags & FDCAN_WAKEUP_ON_TX) == FDCAN_WAKEUP_ON_TX)
		activate_process(can_drv->process,can_drv->wakeup_id,FDCAN_WAKEUP_ON_TX);
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
FDCAN_DriverStruct_t	*can_drv = get_fdcan_ptr_from_workers(hfdcan);

	HAL_FDCAN_GetRxMessage(can_drv->hfdcan, FDCAN_RX_FIFO0, can_drv->RxHeader, can_drv->RxData);
	if (( can_drv->flags & FDCAN_WAKEUP_ON_RX0) == FDCAN_WAKEUP_ON_RX0 )
		activate_process(can_drv->process,can_drv->wakeup_id,FDCAN_WAKEUP_ON_RX0);
}

void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs)
{
FDCAN_DriverStruct_t	*can_drv = get_fdcan_ptr_from_workers(hfdcan);

	if (( can_drv->flags & FDCAN_WAKEUP_ON_RX1) == FDCAN_WAKEUP_ON_RX1)
		activate_process(can_drv->process,can_drv->wakeup_id,FDCAN_WAKEUP_ON_RX1);
}
#endif // #ifdef A_OS_CAN_ENABLED


