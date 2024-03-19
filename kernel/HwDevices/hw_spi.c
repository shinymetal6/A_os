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
 * hw_spi.c
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */

//#define	OLD_SPI
#ifdef OLD_SPI

#include "main.h"
#include "hw_spi.h"
#include "../A.h"
#include "../scheduler.h"
#include "../A_exported_functions.h"
#include "../hwmanager.h"
#include "../system_default.h"

#if defined SPI1_ENABLED

extern	HWMngr_t	HWMngr[PERIPHERAL_NUM];
extern	Asys_t		Asys;

#define	DONT_CHECK_PROCESS	1
int32_t hw_spi_Send(uint8_t *pData, uint16_t Length)
{
uint8_t   tx[256];
#ifndef DONT_CHECK_PROCESS
	if ( HWMngr[HW_SPI1].process != Asys.current_process )
		return HW_SPI_ERROR_HW_NOT_OWNED;
#endif
	if (pData != NULL)
		A_memcpy(tx, pData, Length);
	return HAL_SPI_Transmit(&ST25R3915_SPI, tx, Length, HW_SPI_TIMEOUT);
}

int32_t hw_spi_Recv(uint8_t * const pData, const uint16_t Length)
{
uint8_t   rx[256];
#ifndef DONT_CHECK_PROCESS
	if ( HWMngr[HW_SPI1].process != Asys.current_process )
		return HW_SPI_ERROR_HW_NOT_OWNED;
#endif
	return HAL_SPI_Receive(&ST25R3915_SPI, (pData != NULL) ? (uint8_t *)pData : rx, Length, HW_SPI_TIMEOUT);
}

int32_t hw_spi_SendRecv(const uint8_t * const pTxData, uint8_t * const pRxData, uint16_t Length)
{
#ifndef DONT_CHECK_PROCESS
	if ( HWMngr[HW_SPI1].process != Asys.current_process )
		return HW_SPI_ERROR_HW_NOT_OWNED;
#endif
	if((pTxData != NULL)&&(pRxData != NULL))
		return HAL_SPI_TransmitReceive(&ST25R3915_SPI, (uint8_t *)pTxData, (uint8_t *)pRxData, Length, HW_SPI_TIMEOUT);
	else if ((pTxData != NULL)&&(pRxData == NULL))
		return HAL_SPI_Transmit(&ST25R3915_SPI, (uint8_t *)pTxData, Length, HW_SPI_TIMEOUT);
	else if ((pTxData == NULL)&&(pRxData != NULL))
		return HAL_SPI_Receive(&ST25R3915_SPI, (uint8_t *)pRxData, Length, HW_SPI_TIMEOUT);
	return HW_SPI_ERROR_FEATURE_NOT_SUPPORTED;
}

int32_t hw_spi_Error(void)
{
	return 1;
}

void 	hw_spi_Error_Handler(char * file, int line)
{

}
#endif

#else

#include "main.h"
#include "../system_default.h"
#include "../A.h"
#include "../scheduler.h"
#include "../A_exported_functions.h"
#include "../hwmanager.h"
#include "../kernel_opt.h"
#include "hw_spi.h"

extern	HWMngr_t	HWMngr[PERIPHERAL_NUM];
extern	Asys_t		Asys;

#ifdef A_HAS_SPI_BUS
extern	HW_Spi_t	HW_Spi[A_MAX_SPI];

int32_t hw_spi_Send(uint32_t spi,uint8_t *pData, uint16_t Length)
{
	if ( HWMngr[spi].process != Asys.current_process )
		return HW_SPI_ERROR;
	if (pData == NULL)
		return HW_SPI_ERROR;
	return HAL_SPI_Transmit(HW_Spi[spi-HW_SPI1].hwspi_handle, pData, Length, HW_SPI_TIMEOUT);
}

int32_t hw_spi_Send_DMA(uint32_t spi,uint8_t *pData, uint16_t Length)
{
	if ( HWMngr[spi].process != Asys.current_process )
		return HW_SPI_ERROR;
	if (pData == NULL)
		return HW_SPI_ERROR;
	HW_Spi[spi-HW_SPI1].hwspi_flags |= A_SPI_USE_DMA;
	if (( HW_Spi[spi-HW_SPI1].hwspi_flags & A_SPI_DMA_DONE) == A_SPI_DMA_DONE)
	{
		if ( HAL_SPI_Transmit_DMA(HW_Spi[spi-HW_SPI1].hwspi_handle, pData, Length) == HW_SPI_ERROR_NONE )
		{
			HW_Spi[spi-HW_SPI1].hwspi_flags &= ~A_SPI_DMA_DONE;
			return HW_SPI_ERROR_NONE;
		}
	}
	return HW_SPI_ERROR;
}

int32_t hw_spi_Recv(uint32_t spi,uint8_t *pData,  uint16_t Length)
{
	if ( HWMngr[spi].process != Asys.current_process )
		return HW_SPI_ERROR;
	if (pData == NULL)
		return HW_SPI_ERROR;
	return HAL_SPI_Receive(HW_Spi[spi-HW_SPI1].hwspi_handle, pData, Length, HW_SPI_TIMEOUT);
}

int32_t hw_spi_Recv_DMA(uint32_t spi,uint8_t *pData,  uint16_t Length)
{
	if ( HWMngr[spi].process != Asys.current_process )
		return HW_SPI_ERROR;
	if (pData == NULL)
		return HW_SPI_ERROR;

	HW_Spi[spi-HW_SPI1].hwspi_flags |= A_SPI_USE_DMA;
	if (( HW_Spi[spi-HW_SPI1].hwspi_flags & A_SPI_DMA_DONE) == A_SPI_DMA_DONE)
	{
		if ( HAL_SPI_Receive_DMA(HW_Spi[spi-HW_SPI1].hwspi_handle, pData, Length) == HW_SPI_ERROR_NONE )
		{
			HW_Spi[spi-HW_SPI1].hwspi_flags &= ~A_SPI_DMA_DONE;
			return HW_SPI_ERROR_NONE;
		}
	}
	return HW_SPI_ERROR;
}

int32_t hw_spi_SendRecv(uint32_t spi,const uint8_t * const pTxData, uint8_t * const pRxData, uint16_t Length)
{
	if ( HWMngr[spi].process != Asys.current_process )
		return HW_SPI_ERROR;
	if((pTxData == NULL)||(pRxData == NULL))
		return HW_SPI_ERROR;
	return HAL_SPI_TransmitReceive(HW_Spi[spi-HW_SPI1].hwspi_handle, (uint8_t *)pTxData, (uint8_t *)pRxData, Length, HW_SPI_TIMEOUT);
}

int32_t hw_spi_SendRecv_DMA(uint32_t spi,const uint8_t * const pTxData, uint8_t * const pRxData, uint16_t Length)
{
	if ( HWMngr[spi].process != Asys.current_process )
		return HW_SPI_ERROR;
	if((pTxData == NULL)||(pRxData == NULL))
		return HW_SPI_ERROR;

	HW_Spi[spi-HW_SPI1].hwspi_flags |= A_SPI_USE_DMA;
	if (( HW_Spi[spi-HW_SPI1].hwspi_flags & A_SPI_DMA_DONE) == A_SPI_DMA_DONE)
	{
		if ( HAL_SPI_TransmitReceive_DMA(HW_Spi[spi-HW_SPI1].hwspi_handle, pTxData, pRxData, Length) == HW_SPI_ERROR_NONE )
		{
			HW_Spi[spi-HW_SPI1].hwspi_flags &= ~A_SPI_DMA_DONE;
			return HW_SPI_ERROR_NONE;
		}
	}
	return HW_SPI_ERROR;
}

void process_spi_irq(SPI_HandleTypeDef *hspi,uint32_t flag)
{
uint32_t	i;
	for(i=0;i<A_MAX_SPI;i++)
	{
		if ( hspi == HW_Spi[i].hwspi_handle)
		{
			if (( HWMngr[HW_SPI1+i].status & HWMAN_STATUS_ALLOCATED) == HWMAN_STATUS_ALLOCATED)
			{
				HW_Spi[i].hwspi_flags |= A_SPI_DMA_DONE;
				activate_process(HWMngr[HW_SPI1+i].process,1<<(HW_SPI1+i),flag);
			}
		}
	}
}

void wait_dmadone(uint32_t spi)
{
	HW_Spi[spi-HW_SPI1].hwspi_flags &= ~A_SPI_DMA_DONE;
    while((HW_Spi[spi-HW_SPI1].hwspi_flags & A_SPI_DMA_DONE ) != A_SPI_DMA_DONE )
    	task_delay(1);
    return;
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	__disable_irq();
	process_spi_irq(hspi,WAKEUP_FLAGS_SPI_TX);
	__enable_irq();
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	__disable_irq();
	process_spi_irq(hspi,WAKEUP_FLAGS_SPI_RX);
	__enable_irq();
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	__disable_irq();
	process_spi_irq(hspi,WAKEUP_FLAGS_SPI_TX|WAKEUP_FLAGS_SPI_RX);
	__enable_irq();
}

int32_t hw_spi_Error(void)
{
	return 1;
}

void 	hw_spi_Error_Handler(char * file, int line)
{

}
#endif

void A_hw_spi_init(void)
{
#ifdef	A_HAS_SPI1
	HW_Spi[0].hwspi_handle = &hspi1;
	HW_Spi[0].hwspi_index  = HW_SPI1;
	HW_Spi[0].hwspi_flags  = A_SPI_DMA_DONE;
#endif
#ifdef	A_HAS_SPI2
	HW_Spi[1].hwspi_handle = &hspi2;
	HW_Spi[1].hwspi_index  = HW_SPI2;
	HW_Spi[1].hwspi_flags  = A_SPI_DMA_DONE;
#endif
#ifdef	A_HAS_SPI3
	HW_Spi[2].hwspi_handle = &hspi3;
	HW_Spi[2].hwspi_index  = HW_SPI3;
	HW_Spi[2].hwspi_flags  = A_SPI_DMA_DONE;
#endif
#ifdef	A_HAS_SPI4
	HW_Spi[3].hwspi_handle = &hspi4;
	HW_Spi[3].hwspi_index  = HW_SPI4;
	HW_Spi[3].hwspi_flags  = A_SPI_DMA_DONE;
#endif
}
#endif
