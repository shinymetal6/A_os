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
 * A.c
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */

#include "main.h"
#include "A.h"
#include "A_exported_functions.h"


SYSTEM_RAM		Asys_t			Asys;
SYSTEM_RAM 		PCB_t 			process[MAX_PROCESS];
#ifdef POOL_ENABLE
SYSTEM_RAM		MEMpool_t		MEMpool[POOL_NUM];
#endif

VERSIONING	uint8_t	aos_name[8]	 			= "A_os";
VERSIONING	uint8_t	aos_version[32] 		= A_OS_VERSION;
VERSIONING	uint8_t	lwip_name[8]	 		= "lwip";
#ifdef	NETWORKING_ENABLED
#include "../libraries/lwip2.2/version.h"
VERSIONING	uint8_t	lwip_version[32] 		= LWIP_VERSION;
#else
VERSIONING	uint8_t	lwip_version[32] 		= "Disabled";
#endif
AOS_FLASH_END	uint8_t	aos_end[32]	 			= "A_os End Code";

extern	USRprcs_t	UserProcesses[USR_PROCESS_NUMBER];

#ifdef	NETWORKING_ENABLED
A_IpAddr_t	A_IpAddr =
{
		.IP_ADDRHH 		= 192,
		.IP_ADDRHL 		= 168,
		.IP_ADDRLH		= 10,
		.IP_ADDRLL 		= 210,
		.NETMASK_ADDRHH	= 255,
		.NETMASK_ADDRHL	= 255,
		.NETMASK_ADDRLH	= 255,
		.NETMASK_ADDRLL	= 0,
		.GW_ADDRHH 		= 192,
		.GW_ADDRHL 		= 168,
		.GW_ADDRLH 		= 10,
		.GW_ADDRLL 		= 1
};
A_IpAddr_t	A_DhcpIpAddr;

#endif

uint32_t	UsbDeviceId0 = 0xdeadbeef;
uint32_t	UsbDeviceId1 = 0xbeefdead;

__attribute__((naked)) void init_scheduler_stack(uint32_t sched_top_of_stack)
{
     __set_MSP(sched_top_of_stack);
     __asm volatile("BX LR");
}

void init_processes_stacks(void)
{
uint32_t *pPSP,i,j;
	process[0].psp_value = (uint32_t )IDLE_STACK_START;
	process[1].psp_value = (uint32_t )FIRST_PRC_STACK_START;
	process[2].psp_value = (uint32_t )(process[1].psp_value - UserProcesses[0].stack_size);
	process[3].psp_value = (uint32_t )(process[2].psp_value - UserProcesses[1].stack_size);
	process[4].psp_value = (uint32_t )(process[3].psp_value - UserProcesses[2].stack_size);

	process[0].task_handler = supervisor;
	process[1].task_handler = supervisor_process1;
	process[2].task_handler = supervisor_process2;
	process[3].task_handler = supervisor_process3;
	process[4].task_handler = supervisor_process4;

	for(i = 0 ; i < MAX_TASKS ;i++)
	{
		process[i].current_state = PROCESS_READY_STATE;
		pPSP = (uint32_t*) process[i].psp_value;
		pPSP--;
		*pPSP = DUMMY_XPSR;//0x00100000
		pPSP--; //PC
		*pPSP = (uint32_t) process[i].task_handler;
		pPSP--; //LR
		*pPSP = 0xFFFFFFFD;
		for(j = 0 ; j < 13 ; j++)
		{
			pPSP--;
			*pPSP = 0;
		}
		process[i].psp_value = (uint32_t)pPSP;
	}
}

void init_systick_timer(uint32_t tick_hz)
{
uint32_t ticks = (SYSTICK_TIM_CLK/tick_hz)-1;
    SysTick_Config(ticks);
    Asys.system_flags |= SYS_FLAGS_OS_STARTED;
}

void A_init_mem(void)
{
	Asys.osSysRam_start = (uint32_t *)&_osSysRam_start;
	Asys.osSysRam_size_word  = &_osSysRam_end - &_osSysRam_start;
	//A_clear32((uint8_t *)Asys.osSysRam_start,Asys.osSysRam_size_word*4);
	bzero((uint8_t *)Asys.osSysRam_start,( &_osSysRam_end - &_osSysRam_start)*4);
#ifdef	POOL_ENABLE
	bzero((uint8_t *)POOL_START,POOL_SIZE);
#endif
#ifdef CUSTOM_RAM
	bzero((uint8_t *)CUSTOM_RAM__START,CUSTOM_RAM__SIZE);
#endif
}

void A_enable_processor_faults(void)
{
	SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
	SCB->SHCSR |= SCB_SHCSR_BUSFAULTENA_Msk;
	SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk;
}

void A_IrqPriority_Init(void)
{
	//The lower the number, the higher the priority,
	HAL_NVIC_SetPriority(PendSV_IRQn,  PendSV_PRIORITY, 0);		/* Make PendSV_IRQn lower priority */
	HAL_NVIC_SetPriority(SysTick_IRQn, SysTick_PRIORITY, 0);	/* Make SysTick_IRQn higher priority than PendSV_IRQn */
}

void DWT_Delay_us(uint32_t au32_microseconds)
{
	uint32_t au32_initial_ticks = DWT->CYCCNT;
	uint32_t au32_ticks = (HAL_RCC_GetHCLKFreq() / 1000000);
	au32_microseconds *= au32_ticks;
	while ((DWT->CYCCNT - au32_initial_ticks) < au32_microseconds-au32_ticks);
}

uint32_t DWT_Delay_Init(void)
{
	/* Disable TRC */
	CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk; // ~0x01000000;
	/* Enable TRC */
	CoreDebug->DEMCR |=  CoreDebug_DEMCR_TRCENA_Msk; // 0x01000000;
	/* Disable clock cycle counter */
	DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //~0x00000001;
	/* Enable  clock cycle counter */
	DWT->CTRL |=  DWT_CTRL_CYCCNTENA_Msk; //0x00000001;
	/* Reset the clock cycle counter value */
	DWT->CYCCNT = 0;
	/* 3 NO OPERATION instructions */
	__ASM volatile ("NOP");
	__ASM volatile ("NOP");
	__ASM volatile ("NOP");
	/* Check if clock cycle counter has started */
	if(DWT->CYCCNT)
		return 0; /*clock cycle counter started*/
	return 1; /*clock cycle counter not started*/
}

#ifdef DATA_CACHE_ENABLE
void A_MPU_Config(void)
{
#ifdef	STM32H743xx

	MPU_Region_InitTypeDef MPU_InitStruct = {0};

	/* Disables the MPU */
	HAL_MPU_Disable();

	/** Initializes and configures the Region and the memory to be protected
	*/
	MPU_InitStruct.Enable = MPU_REGION_ENABLE;
	MPU_InitStruct.Number = MPU_REGION_NUMBER0;
	MPU_InitStruct.BaseAddress = 0x24040000;
	MPU_InitStruct.Size = MPU_REGION_SIZE_64KB;
	MPU_InitStruct.SubRegionDisable = 0x87;
	MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
	MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
	MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

	HAL_MPU_ConfigRegion(&MPU_InitStruct);
	/* Enables the MPU */
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
#endif // #ifdef	STM32H743xx
}
#endif // #ifdef DATA_CACHE_ENABLE

void A_initialize_onchip_peripherals(void)
{
#ifdef USB_DEVICE_ENABLED
#ifdef	STM32U575xx
	HAL_Delay(1000);
#endif
	MX_USB_Device_Init();
#endif // #ifdef USB_ENABLED
}

void user_processes_init(void)
{
uint32_t i;
	__enable_irq();
	for(i = 0 ; i < USR_PROCESS_NUMBER ;i++)
	{
		Asys.current_process = i+1;
		if ( UserProcesses[i].user_init )
			UserProcesses[i].user_init(i+1);
	}
	Asys.current_process = 0;
	__disable_irq();
}

void A_start(void)
{
	HAL_Delay(2);	// wait some ticks so system will correctly fires some irqs
	sprintf(Asys.version,"%s",A_OS_VERSION);
#ifdef USB_DEVICE_ENABLED
#ifdef	STM32U575xx
	HAL_Delay(1000);
	MX_USB_Device_Init();
#endif
#endif // #ifdef USB_ENABLED
	A_PreOS_Init();
	__disable_irq();

	DWT_Delay_Init();
	A_Processor_Quirks();

	A_enable_processor_faults();
	A_IrqPriority_Init();

	//init_scheduler_stack(SCHED_STACK_START);
	init_processes_stacks();
#ifndef AOS_USER_SHORT_INIT
	user_processes_init();
#endif // #ifdef AOS_USER_LONG_INIT
	init_systick_timer(TICK_HZ);
#ifdef POOL_ENABLE
	//A_mem_init();
#endif // #ifdef	POOL_ENABLE
#ifdef DATA_CACHE_ENABLE
#ifdef	STM32H743xx
	A_MPU_Config();
	SCB_EnableDCache();
#endif // #ifdef	STM32H743xx
#endif // #ifdef DATA_CACHE_ENABLE

	A_PreScheduler_Init();
	switch_sp_to_psp();

	__enable_irq();
	supervisor();
}

