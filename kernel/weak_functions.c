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
 * weak_functions.c
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */
#include "main.h"
#include "system_default.h"
#include "A.h"
#include "A_exported_functions.h"
#include "kernel_opt.h"

#ifdef CODEC_NAU88C22
extern	void Nau88c22_Init(void);
#endif

extern	A_IpAddr_t	A_IpAddr;

__weak void supervisor_callback(void)
{

}

__weak uint8_t MX_USB_Device_Init(void)
{
	return 1;
}

__weak void MX_USB_DEVICE_Init(void)
{
}



__weak void MX_LWIP_Init(A_IpAddr_t *A_IpAddr)
{

}

__weak void MX_LWIP_Process(void)
{

}

__weak void MX_USB_HOST_Process(void)
{

}

__weak extern	uint8_t CDC_Receive_Callback(uint8_t* Buf, uint16_t Len)
{
	return 0;
}

__weak uint8_t MIDI_Receive_Callback(uint8_t* Buf, uint16_t Len)
{
	return 0;
}

/* os related */

__weak void postpone_init(void)
{

}

__weak void A_PreOS_Init(void)
{
#ifdef NETWORKING_ENABLED
	#if defined ETH_NRST_Pin
		HAL_GPIO_WritePin(ETH_NRST_GPIO_Port, ETH_NRST_Pin,GPIO_PIN_RESET);
		HAL_Delay(10);
		HAL_GPIO_WritePin(ETH_NRST_GPIO_Port, ETH_NRST_Pin,GPIO_PIN_SET);
		HAL_Delay(1);
	#endif // 	#if defined ETH_NRST_Pin
		MX_LWIP_Init(&A_IpAddr);
		MX_LWIP_Process();
#else
	#if defined ETH_NRST_Pin
		HAL_GPIO_WritePin(ETH_NRST_GPIO_Port, ETH_NRST_Pin,GPIO_PIN_RESET);
	#endif // 	#if defined ETH_NRST_Pin
#endif // #ifdef NETWORKING_ENABLED


#ifdef LORA_ENABLED
	#if defined SX1278_RESET_Pin
		HAL_GPIO_WritePin(SX1278_RESET_GPIO_Port, SX126X_RESET_Pin,GPIO_PIN_RESET);
	#endif // #if defined ETH_NRST_Pin
#else
	#if defined SX1278_RESET_Pin
		HAL_GPIO_WritePin(SX1278_RESET_GPIO_Port, SX1278_RESET_Pin,GPIO_PIN_RESET);
	#endif // #if defined ETH_NRST_Pin
#endif // #ifdef LORA_ENABLED
}

__weak void A_Processor_Quirks(void)
{
}

__weak void A_PreScheduler_Init(void)
{
}

__weak void A_IrqPriority_Init(void)
{
	HAL_NVIC_SetPriority(PendSV_IRQn,  PendSV_PRIORITY, 0);		/* Make PendSV_IRQn lower priority */
	HAL_NVIC_SetPriority(SysTick_IRQn, SysTick_PRIORITY, 0);	/* Make PendSV_IRQn lower priority */
}

__weak void encoder_button_irq_callback(void)
{

}

__weak void encoder_clock_irq_callback(void)
{

}

__weak void httpd_init(void)
{

}

