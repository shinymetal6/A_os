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
 * Project : A_os_U575_devel 
*/
/*
 * weak_functions.c
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */
#include "main.h"
#include "A.h"
#include "system_default.h"

extern	A_IpAddr_t	A_IpAddr;

__weak void supervisor_callback(void)
{

}

__weak void MX_USB_Device_Init(void)
{

}

__weak void MX_LWIP_Init(A_IpAddr_t *A_IpAddr)
{

}

__weak void MX_LWIP_Process(void)
{

}

__weak extern	uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
	return 0;
}

/* os related */
__weak void A_PreOS_Init(void)
{
	MX_USB_Device_Init();
	MX_LWIP_Init(&A_IpAddr);
}

__weak void A_PreScheduler_Init(void)
{
}

__weak void A_IrqPriority_Init(void)
{
	HAL_NVIC_SetPriority(PendSV_IRQn,  PendSV_PRIORITY, 0);		/* Make PendSV_IRQn lower priority */
	HAL_NVIC_SetPriority(SysTick_IRQn, SysTick_PRIORITY, 0);	/* Make PendSV_IRQn lower priority */
}

