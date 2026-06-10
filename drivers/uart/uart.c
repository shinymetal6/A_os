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
 * uart.c
 *
 *  Created on: Nov 12, 2024
 *      Author: fil
 */

#include "main.h"

#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#ifdef A_OS_UART_ENABLED

#include "uart.h"
#include <string.h>

UART_DriverStruct_t	*uart_drv_ptr;

ITCM_AREA_CODE  uint32_t uart_init(UART_DriverStruct_t *uart_drv)
{
	uart_drv->timeout_reload_value = uart_drv->timeout;
	return 0;
}

ITCM_AREA_CODE  uint32_t uart_get_status(UART_DriverStruct_t *uart_drv)
{
	return 0;
}

uint32_t ret_val = 0;
ITCM_AREA_CODE  uint32_t	uart_send(UART_DriverStruct_t *uart_drv, uint8_t *buffer,uint16_t len)
{
	if ( (uart_drv->flags & UART_USES_DMA_TX) == UART_USES_DMA_TX )
		ret_val = HAL_UART_Transmit_DMA(uart_drv->uart , buffer, len);
	else
		ret_val = HAL_UART_Transmit_IT(uart_drv->uart , buffer, len);
	return ret_val;
}

ITCM_AREA_CODE  uint32_t	uart_start_receive(UART_DriverStruct_t *uart_drv)
{
	if ( (uart_drv->flags & UART_USES_DMA_RX) == UART_USES_DMA_RX )
		return HAL_UART_Receive_DMA(uart_drv->uart, uart_drv->data, uart_drv->rx_max_len);
	else
		return HAL_UART_Receive_IT(uart_drv->uart, &uart_drv->rx_char, 1);
}

ITCM_AREA_CODE  uint32_t	uart_get_rxlen(UART_DriverStruct_t *uart_drv)
{
	return (uint32_t )uart_drv->rx_num_chars;
}

ITCM_AREA_CODE  uint32_t	uart_set_rxlen(UART_DriverStruct_t *uart_drv,uint16_t rx_max_len)
{
	uart_drv->rx_max_len = rx_max_len;
	return 0;
}

ITCM_AREA_CODE  uint32_t	uart_set_sentinel(UART_DriverStruct_t *uart_drv, uint8_t sentinel_start, uint8_t sentinel_end)
{
	if ( sentinel_start)
		uart_drv->sentinel_start = sentinel_start;
	if ( sentinel_end)
		uart_drv->sentinel_end   = sentinel_end;
	return 0;
}

ITCM_AREA_CODE  uint32_t	uart_disable_receive(UART_DriverStruct_t *uart_drv)
{
UART_HandleTypeDef *huart = uart_drv->uart;
	if ( (uart_drv->flags & UART_USES_DMA_RX) == UART_USES_DMA_RX )
		if (huart->hdmarx != NULL)
			HAL_DMA_Abort(huart->hdmarx);  // Force-stop DMA channel
	__HAL_UART_DISABLE(huart);

	__HAL_UART_CLEAR_PEFLAG(huart);   // Parity error
	__HAL_UART_CLEAR_FEFLAG(huart);   // Framing error
	__HAL_UART_CLEAR_NEFLAG(huart);   // Noise error
	__HAL_UART_CLEAR_OREFLAG(huart);  // Overrun error — MOST IMPORTANT!

	huart->RxState = HAL_UART_STATE_RESET;
	huart->ErrorCode = HAL_UART_ERROR_NONE;
	return 0;
}

ITCM_AREA_CODE  uint32_t	uart_enable_receive(UART_DriverStruct_t *uart_drv)
{
UART_HandleTypeDef *huart = uart_drv->uart;
	__HAL_UART_CLEAR_PEFLAG(huart);   // Parity error
	__HAL_UART_CLEAR_FEFLAG(huart);   // Framing error
	__HAL_UART_CLEAR_NEFLAG(huart);   // Noise error
	__HAL_UART_CLEAR_OREFLAG(huart);  // Overrun error — MOST IMPORTANT!

	huart->RxState = HAL_UART_STATE_READY;
	huart->ErrorCode = HAL_UART_ERROR_NONE;

	__HAL_UART_ENABLE(huart);

	if ( (uart_drv->flags & UART_USES_DMA_RX) == UART_USES_DMA_RX )
		return HAL_UART_Receive_DMA(uart_drv->uart, uart_drv->data, uart_drv->rx_max_len);
	return 0;
}

ITCM_AREA_CODE uint32_t uart_reinit_on_error(UART_DriverStruct_t *uart_drv)
{
UART_HandleTypeDef *huart = uart_drv->uart;
	if ( (uart_drv->flags & UART_USES_DMA_RX) == UART_USES_DMA_RX )
	{
		if (huart->hdmarx != NULL)
			HAL_DMA_Abort(huart->hdmarx);  // Force-stop DMA channel

		__HAL_UART_DISABLE(huart);

		__HAL_UART_CLEAR_PEFLAG(huart);   // Parity error
		__HAL_UART_CLEAR_FEFLAG(huart);   // Framing error
		__HAL_UART_CLEAR_NEFLAG(huart);   // Noise error
		__HAL_UART_CLEAR_OREFLAG(huart);  // Overrun error — MOST IMPORTANT!

		huart->RxState = HAL_UART_STATE_READY;
		huart->ErrorCode = HAL_UART_ERROR_NONE;

		__HAL_UART_ENABLE(huart);
		return HAL_UART_Receive_DMA(uart_drv->uart, uart_drv->data, uart_drv->rx_max_len);
	}
	return 0;
}

ITCM_AREA_CODE uint32_t uart_restart_DMA_on_RX(UART_DriverStruct_t *uart_drv)
{
	return uart_reinit_on_error(uart_drv);
}


ITCM_AREA_CODE uint32_t uart_get_rxerrors_number(UART_DriverStruct_t *uart_drv)
{
	return uart_drv->rx_errors;
}

ITCM_AREA_CODE uint32_t uart_get_timeouts_number(UART_DriverStruct_t *uart_drv)
{
	return uart_drv->to_errors;
}

extern void UART_Driver_RxTimeoutCheckCallback(uint32_t *param);

ITCM_AREA_CODE uint32_t	uart_register(UART_DriverStruct_t *uart_drv)
{

UART_DriverStruct_t *eptr, *pre_eptr;

	if ( uart_drv->uart == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( uart_drv_ptr == NULL)
	{
		uart_drv_ptr = uart_drv;
		uart_drv->next_uart = NULL;
	}
	else
	{
		eptr = pre_eptr = uart_drv_ptr;
		while(eptr->next_uart != NULL)
		{
			pre_eptr = eptr;
			eptr = (UART_DriverStruct_t *)eptr->next_uart;
		}
		pre_eptr->next_uart = (uint32_t *)uart_drv;
		uart_drv->next_uart = NULL;
	}

	uart_drv->process = get_current_process();
	uart_drv_ptr = uart_drv;

	uart_drv->timeout_reload_value = uart_drv->timeout;
#ifdef	STM32U575xxA
	uart_drv->flags &= ~UART_USES_DMA_RX;
	uart_drv->flags &= ~UART_USES_DMA_TX;
	#define	SKIP_DMA_UART 1
#endif
#ifdef	STM32H563xx
	uart_drv->flags &= ~UART_USES_DMA_RX;
	uart_drv->flags &= ~UART_USES_DMA_TX;
	#define	SKIP_DMA_UART 1
#endif
#ifdef	STM32N657xx
	uart_drv->flags &= ~UART_USES_DMA_RX;
	uart_drv->flags &= ~UART_USES_DMA_TX;
	#define	SKIP_DMA_UART 1
#endif
#ifndef	SKIP_DMA_UART
	if ( uart_drv->uart->hdmarx == NULL )
	{
		/* disable dma if they are not configured in hw */
		uart_drv->flags &= ~UART_USES_DMA_RX;
	}
	if ( uart_drv->uart->hdmatx == NULL )
	{
		/* disable dma if they are not configured in hw */
		uart_drv->flags &= ~UART_USES_DMA_TX;
	}
#endif
	uart_drv->rx_errors = 0;
	uart_drv->status = DRIVER_STATUS_IN_USE;
	set_before_check_timers_callback(UART_Driver_RxTimeoutCheckCallback,(uint32_t *)uart_drv);

#ifndef	SKIP_DMA_UART
	if ( (uart_drv->flags & UART_USES_DMA_RX) == UART_USES_DMA_RX )
#ifdef	STM32U575xx
		uart_drv->rx_num_chars = uart_drv->rx_max_len - (uart_drv->uart->hdmarx->Instance->CBR1 & DMA_CBR1_BNDT);
		#define	DMA_UART_FOUND 1
#endif
#ifdef	STM32F446xx
		uart_drv->rx_num_chars = uart_drv->rx_max_len-uart_drv->uart->hdmarx->Instance->NDTR;
		#define	DMA_UART_FOUND 1
#endif
#ifdef	STM32L152xE
		uart_drv->rx_num_chars = uart_drv->rx_max_len-uart_drv->uart->hdmarx->Instance->CNDTR;
		#define	DMA_UART_FOUND 1
#endif
#ifdef	STM32G474xx
		uart_drv->rx_num_chars = uart_drv->rx_max_len-uart_drv->uart->hdmarx->Instance->CNDTR;
		#define	DMA_UART_FOUND 1
#endif
#ifdef	STM32G491xx
		uart_drv->rx_num_chars = uart_drv->rx_max_len-uart_drv->uart->hdmarx->Instance->CNDTR;
		#define	DMA_UART_FOUND 1
#endif
#ifdef	STM32G431xx
		uart_drv->rx_num_chars = uart_drv->rx_max_len-uart_drv->uart->hdmarx->Instance->CNDTR;
		#define	DMA_UART_FOUND 1
#endif
#ifdef	STM32H563xx
		uart_drv->rx_num_chars = uart_drv->rx_max_len-((DMA_NodeTypeDef *)uart_drv->uart->hdmarx->Instance)->NDTR;
		#define	DMA_UART_FOUND 1
#endif
#ifndef	DMA_UART_FOUND
		uart_drv->rx_num_chars = uart_drv->rx_max_len-((DMA_Stream_TypeDef *)uart_drv->uart->hdmarx->Instance)->NDTR;
#endif
#endif // #ifndef	SKIP_DMA_UART
		return 0;
}

/***********************************/
/****	Interrupt functions 	****/
/***********************************/
ITCM_AREA_CODE UART_DriverStruct_t *find_uart(UART_HandleTypeDef *huart)
{
UART_DriverStruct_t *eptr, *pre_eptr;

	eptr = pre_eptr = uart_drv_ptr;
	while(eptr != NULL)
	{
		if ( eptr->uart == huart )
			return eptr;
		pre_eptr = eptr;
		if ( eptr->next_uart == NULL )
			return NULL;
		eptr = (UART_DriverStruct_t *)eptr->next_uart;
	}
	return NULL;
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
UART_DriverStruct_t	*uart_drv;
	__disable_irq();
	if ( (uart_drv = find_uart(huart)) != NULL)
	{
		uart_drv->uart_error++;
		if (( uart_drv->flags & UART_WAKEUP_ON_ERRORS) == UART_WAKEUP_ON_ERRORS)
		{
			// // clear_hw_flag();
			activate_process(uart_drv->process,uart_drv->wakeup_id,WAKEUP_FLAGS_UART_ERR);
		}
		uart_drv->timeout = uart_drv->timeout_reload_value;
		uart_drv->rx_errors ++;
	}
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
UART_DriverStruct_t	*uart_drv;

	__disable_irq();
	if ( (uart_drv = find_uart(huart)) != NULL)
	{
		if (( uart_drv->flags & UART_WAKEUP_ON_TX) == UART_WAKEUP_ON_TX)
		{
			// // clear_hw_flag();
			activate_process(uart_drv->process,uart_drv->wakeup_id,WAKEUP_FLAGS_UART_TX);
		}
	}
	__enable_irq();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
UART_DriverStruct_t	*uart_drv;
	__disable_irq();
	if ( (uart_drv = find_uart(huart)) != NULL)
	{
#ifdef	STM32H563xx
		#define	SKIP_DMA_UART 1
#endif
#ifndef	SKIP_DMA_UART
		if ( (uart_drv->flags & UART_USES_DMA_RX) == UART_USES_DMA_RX )
		{

#ifdef   STM32_HAS_STREAM_DMA
#ifdef	STM32U575xx
			uart_drv->rx_num_chars = uart_drv->rx_max_len - (uart_drv->uart->hdmarx->Instance->CBR1 & DMA_CBR1_BNDT);
#else	//#ifdef	STM32U575xx
			uart_drv->rx_num_chars = uart_drv->rx_max_len-((DMA_Stream_TypeDef *)uart_drv->uart->hdmarx->Instance)->NDTR;
#endif	//#ifdef	STM32U575xx
#else	//#ifdef   STM32_HAS_STREAM_DMA
#ifdef	STM32F446xx
			uart_drv->rx_num_chars = uart_drv->rx_max_len-uart_drv->uart->hdmarx->Instance->NDTR;
#else
			uart_drv->rx_num_chars = uart_drv->rx_max_len-((DMA_Channel_TypeDef *)uart_drv->uart->hdmarx->Instance)->CNDTR;
#endif //#ifdef   STM32_HAS_STREAM_DMA
#endif //#ifdef   STM32_HAS_STREAM_DMA


			uart_drv->timeout = uart_drv->timeout_reload_value;
			uart_drv->timeout_mask = 1;
			// clear_hw_flag();
			activate_process(uart_drv->process,uart_drv->wakeup_id,WAKEUP_FLAGS_UART_RX);
			HAL_UART_Receive_DMA(uart_drv->uart, uart_drv->data, uart_drv->rx_max_len);
			return;
		}
#endif // #ifndef	SKIP_DMA_UART

		if (uart_drv->data != NULL )
		{
			if ((uart_drv->sentinel_start == 0) && ( uart_drv->sentinel_end == 0 ))
			{
				uart_drv->data[uart_drv->rx_index] = uart_drv->rx_char;
				uart_drv->timeout = uart_drv->timeout_reload_value;
				uart_drv->rx_index ++;
				if ( uart_drv->rx_index >= uart_drv->rx_max_len )
				{
					uart_drv->rx_num_chars = uart_drv->rx_index;
					uart_drv->rx_index = 0;
					if (( uart_drv->flags & UART_WAKEUP_ON_RXFULL) == UART_WAKEUP_ON_RXFULL)
					{
						// clear_hw_flag();
						activate_process(uart_drv->process,uart_drv->wakeup_id,WAKEUP_FLAGS_UART_RX);
					}
				}
				if (( uart_drv->flags & UART_WAKEUP_ON_RXCHAR) == UART_WAKEUP_ON_RXCHAR)
				{
					// clear_hw_flag();
					activate_process(uart_drv->process,uart_drv->wakeup_id,WAKEUP_FLAGS_UART_RX);
				}
			}
			else if ((uart_drv->sentinel_start != 0) && ( uart_drv->sentinel_end == 0 ))
			{
				uart_drv->timeout = uart_drv->timeout_reload_value;
				if ((uart_drv->sentinel_flags & UART_SENTINEL_START_FOUND) == UART_SENTINEL_START_FOUND)
				{
					uart_drv->data[uart_drv->rx_index] = uart_drv->rx_char;
					uart_drv->rx_index ++;
					if ( uart_drv->rx_index > uart_drv->rx_max_len )
					{
						uart_drv->rx_num_chars = uart_drv->rx_index;
						uart_drv->rx_index = 0;
						uart_drv->sentinel_flags &= ~UART_SENTINEL_START_FOUND;
						if (( uart_drv->flags & UART_WAKEUP_ON_RXFULL) == UART_WAKEUP_ON_RXFULL)
						{
							// clear_hw_flag();
							activate_process(uart_drv->process,uart_drv->wakeup_id,WAKEUP_FLAGS_UART_RX);
						}
					}
					if (( uart_drv->flags & UART_WAKEUP_ON_RXCHAR) == UART_WAKEUP_ON_RXCHAR)
					{
						// clear_hw_flag();
						activate_process(uart_drv->process,uart_drv->wakeup_id,WAKEUP_FLAGS_UART_RX);
					}
				}
				else
				{
					if ( uart_drv->rx_char == uart_drv->sentinel_start)
					{
						if ( uart_drv->rx_char == uart_drv->sentinel_start)
						{
							uart_drv->sentinel_flags |= UART_SENTINEL_START_FOUND;
							uart_drv->data[uart_drv->rx_index] = uart_drv->rx_char;
							uart_drv->rx_index = 1;
							uart_drv->rx_num_chars = uart_drv->rx_index;
						}
					}
				}
			}
			else if ((uart_drv->sentinel_start != 0) && ( uart_drv->sentinel_end != 0 ))
			{
				uart_drv->timeout = uart_drv->timeout_reload_value;
				if (( uart_drv->rx_char == uart_drv->sentinel_start) && ((uart_drv->sentinel_flags & UART_SENTINEL_START_FOUND) != UART_SENTINEL_START_FOUND))
				{
					uart_drv->sentinel_flags |= UART_SENTINEL_START_FOUND;
					uart_drv->data[uart_drv->rx_index] = uart_drv->rx_char;
					uart_drv->rx_index = 1;
					uart_drv->rx_num_chars = uart_drv->rx_index;
				}
				if (( uart_drv->rx_char == uart_drv->sentinel_end) && ((uart_drv->sentinel_flags & UART_SENTINEL_START_FOUND) == UART_SENTINEL_START_FOUND))
				{
					uart_drv->data[uart_drv->rx_index] = uart_drv->rx_char;
					uart_drv->sentinel_flags |= UART_SENTINEL_END_FOUND;
					if (( uart_drv->flags & UART_WAKEUP_ON_RXFULL) == UART_WAKEUP_ON_RXFULL)
					{
						uart_drv->rx_num_chars = uart_drv->rx_index;
						uart_drv->rx_index = 0;
						uart_drv->sentinel_flags &= ~(UART_SENTINEL_START_FOUND | UART_SENTINEL_END_FOUND);
						// clear_hw_flag();
						activate_process(uart_drv->process,uart_drv->wakeup_id,WAKEUP_FLAGS_UART_RX);
					}
				}
				else
				{
					uart_drv->data[uart_drv->rx_index] = uart_drv->rx_char;
					uart_drv->rx_index ++;
					uart_drv->rx_num_chars = uart_drv->rx_index;
					if ( uart_drv->rx_index > uart_drv->rx_max_len )
					{
						uart_drv->rx_index = 0;
						uart_drv->sentinel_flags &= ~(UART_SENTINEL_START_FOUND | UART_SENTINEL_END_FOUND);
					}
				}
			}
		}
		uart_drv->timeout = uart_drv->timeout_reload_value;
		HAL_UART_Receive_IT(uart_drv->uart, &uart_drv->rx_char, 1);
	}
	__enable_irq();
}

ITCM_AREA_CODE void UART_Driver_RxTimeoutCheckCallback(uint32_t *param)
{
UART_DriverStruct_t	*uart_drv = (UART_DriverStruct_t *)param;
	if ( uart_drv->process != 0 )
	{
#ifdef	STM32H563xx
	#define	SKIP_DMA_UART 1
#endif
#ifndef	SKIP_DMA_UART
		if ( (uart_drv->flags & UART_USES_DMA_RX) == UART_USES_DMA_RX )
		{
#ifdef   STM32_HAS_STREAM_DMA
#ifdef	STM32U575xx
			uart_drv->rx_num_chars = uart_drv->rx_max_len - (uart_drv->uart->hdmarx->Instance->CBR1 & DMA_CBR1_BNDT);
#else	//#ifdef	STM32U575xx
			uart_drv->rx_num_chars = uart_drv->rx_max_len-((DMA_Stream_TypeDef *)uart_drv->uart->hdmarx->Instance)->NDTR;
#endif	//#ifdef	STM32U575xx
#else	//#ifdef   STM32_HAS_STREAM_DMA
#ifdef	STM32F446xx
		uart_drv->rx_num_chars = uart_drv->rx_max_len-uart_drv->uart->hdmarx->Instance->NDTR;
#else
		uart_drv->rx_num_chars = uart_drv->rx_max_len-((DMA_Channel_TypeDef *)uart_drv->uart->hdmarx->Instance)->CNDTR;
#endif
#endif //#ifdef   STM32_HAS_STREAM_DMA
			if (( uart_drv->rx_num_chars ) && ( uart_drv->rx_num_chars != uart_drv->rx_max_len))
			{
				if ( uart_drv->timeout )
					uart_drv->timeout--;
				if ( uart_drv->timeout == 0 )
				{
					if ( uart_drv->timeout_mask == 0 )
					{
						uart_drv->timeout_mask = 1;
#ifdef	STM32U575xx
						uart_drv->uart->hdmarx->Instance->CCR &= ~DMA_CCR_EN;
						 __HAL_DMA_SET_COUNTER(uart_drv->uart->hdmarx,uart_drv->rx_max_len);
						uart_drv->uart->hdmarx->Instance->CCR |= DMA_CCR_EN;
#endif
						if (( uart_drv->flags & UART_WAKEUP_ON_TIMEOUT) == UART_WAKEUP_ON_TIMEOUT)
						{
							activate_process(uart_drv->process,uart_drv->wakeup_id,WAKEUP_FLAGS_UART_TO | WAKEUP_FLAGS_UART_RX);
						}
					}
					else
						uart_drv->timeout_mask = 0;

					/* a bit of unicorn dust here ... */
					__HAL_DMA_DISABLE(uart_drv->uart->hdmarx);
					#ifdef   STM32_HAS_STREAM_DMA
					#ifndef	STM32U575xx
					if (( ((DMA_Stream_TypeDef *)uart_drv->uart->hdmarx->Instance)->FCR & DMA_FLAG_TCIF0_4) == DMA_FLAG_TCIF0_4)// ->BDMA->IFCR & DMA_FLAG_TCIF0_4) == DMA_FLAG_TCIF0_4)
						__HAL_DMA_CLEAR_FLAG(uart_drv->uart->hdmarx,DMA_FLAG_TCIF0_4);
					if (( ((DMA_Stream_TypeDef *)uart_drv->uart->hdmarx->Instance)->FCR & DMA_FLAG_TCIF1_5) == DMA_FLAG_TCIF1_5)// ->BDMA->IFCR & DMA_FLAG_TCIF0_4) == DMA_FLAG_TCIF0_4)
						__HAL_DMA_CLEAR_FLAG(uart_drv->uart->hdmarx,DMA_FLAG_TCIF1_5);
					if (( ((DMA_Stream_TypeDef *)uart_drv->uart->hdmarx->Instance)->FCR & DMA_FLAG_TCIF2_6) == DMA_FLAG_TCIF2_6)// ->BDMA->IFCR & DMA_FLAG_TCIF0_4) == DMA_FLAG_TCIF0_4)
						__HAL_DMA_CLEAR_FLAG(uart_drv->uart->hdmarx,DMA_FLAG_TCIF2_6);
					if (( ((DMA_Stream_TypeDef *)uart_drv->uart->hdmarx->Instance)->FCR & DMA_FLAG_TCIF3_7) == DMA_FLAG_TCIF3_7)// ->BDMA->IFCR & DMA_FLAG_TCIF0_4) == DMA_FLAG_TCIF0_4)
						__HAL_DMA_CLEAR_FLAG(uart_drv->uart->hdmarx,DMA_FLAG_TCIF3_7);
					((DMA_Stream_TypeDef *)uart_drv->uart->hdmarx->Instance)->NDTR = uart_drv->rx_max_len;
					#endif // 						#ifndef	STM32U575xx
					#else
#ifdef	STM32F446xx
					uart_drv->uart->hdmarx->Instance->NDTR = uart_drv->rx_max_len;
#else
					((DMA_Channel_TypeDef *)uart_drv->uart->hdmarx->Instance)->CNDTR = uart_drv->rx_max_len;
#endif
					__HAL_DMA_ENABLE(uart_drv->uart->hdmarx);
					#endif
					/* end of unicorn dust */

					uart_drv->timeout = uart_drv->timeout_reload_value;
					HAL_UART_Receive_DMA(uart_drv->uart, uart_drv->data, uart_drv->rx_max_len);
				}
			}
		}
		else
#endif //#ifndef	SKIP_DMA_UART
		{
			if ( uart_drv->timeout )
			{
				if ( uart_drv->rx_index )
				{
					uart_drv->timeout--;
					if ( uart_drv->timeout == 0 )
					{
						uart_drv->timeout = uart_drv->timeout_reload_value;
						uart_drv->rx_num_chars = uart_drv->rx_index;
						uart_drv->rx_index = 0;
						if (( uart_drv->flags & UART_WAKEUP_ON_TIMEOUT) == UART_WAKEUP_ON_TIMEOUT)
						{
							// clear_hw_flag();
							activate_process(uart_drv->process,uart_drv->wakeup_id,WAKEUP_FLAGS_UART_TO | WAKEUP_FLAGS_UART_RX);
						}
					}
				}
			}
		}
	}
}


#endif // #ifdef A_OS_UART_ENABLED


