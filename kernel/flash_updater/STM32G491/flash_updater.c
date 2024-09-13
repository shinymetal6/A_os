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
 * flash_updater.c
 *
 *  Created on: Jan 31, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../A.h"

#ifdef	STM32G491xx
#include "../../system_default.h"
#include "flash_updater.h"

uint32_t	G491_flash_swTick=0;

#define	G491_RAMFUNC	 __attribute__((section(".RamFunc")))

G491_RAMFUNC void G491_flash_unlock(void)
{
	if ((FLASH->CR & FLASH_CR_LOCK) == FLASH_CR_LOCK)
	{
		FLASH->KEYR = 0x45670123;
		FLASH->KEYR = 0xCDEF89AB;
	}
	return;
}

G491_RAMFUNC void G491_flash_Lock(void)
{
	FLASH->CR |= FLASH_CR_LOCK;
}

G491_RAMFUNC void G491_flash_Program_DoubleWord(uint32_t Address, uint32_t *Data)
{
	FLASH->SR = 0xffffffff;
	*(uint32_t *)Address = *Data;
	Address++;
	Data++;
	__ISB();
	*(uint32_t *)Address = *Data;

	while(FLASH->SR & FLASH_SR_BSY);

	while((FLASH->SR & FLASH_SR_EOP) != FLASH_SR_EOP);
	FLASH->SR |= FLASH_SR_EOP;
}

G491_RAMFUNC void G491_flash_write(uint8_t *src, uint8_t *dst,uint32_t size)
{
uint32_t *FlashAddress = (uint32_t *)dst, *SrcAddress = (uint32_t *)src, i;

	while(FLASH->SR & FLASH_SR_BSY);
	FLASH->CR = FLASH_CR_PG;

	for(i=0;i<size;i+=8)
	{
		FLASH->SR = 0xffffffff;
		*FlashAddress = *SrcAddress;
		FlashAddress++;
		SrcAddress++;
		__ISB();
		__DSB();
		*FlashAddress = *SrcAddress;
		FlashAddress++;
		SrcAddress++;
		__ISB();
		__DSB();

		while(FLASH->SR & FLASH_SR_BSY);
	}

	FLASH->CR &= ~FLASH_CR_PG;
}

G491_RAMFUNC void G491_flash_erase(uint32_t FirstPage,uint32_t NPages)
{
uint8_t page_index;

	FLASH->SR |= FLASH_SR_PGAERR;
	for (page_index = FirstPage; page_index < FirstPage+NPages; page_index++)
	{
		while (FLASH->SR & FLASH_SR_BSY);
		FLASH->CR = (page_index << 3) | FLASH_CR_PER;
		FLASH->CR |= FLASH_CR_STRT;
		while (FLASH->SR & FLASH_SR_BSY);
	}
}

G491_RAMFUNC void G491_DWT_Delay_us(uint32_t au32_microseconds)
{
	uint32_t au32_initial_ticks = DWT->CYCCNT;
	uint32_t au32_ticks = (HAL_RCC_GetHCLKFreq() / 1000000);
	au32_microseconds *= au32_ticks;
	while ((DWT->CYCCNT - au32_initial_ticks) < au32_microseconds-au32_ticks);
}


G491_RAMFUNC void flash_update(uint8_t *flash_data,uint32_t size)
{
uint32_t PagesToErase = size / FLASH_PAGE_SIZE;
uint32_t	i;
	/* stops A_os schedule activities */
	__disable_irq();

	Asys.g_os_started = 0;
	/* leave only systick enabled */
	for(i=0;i<128;i++)
		HAL_NVIC_DisableIRQ(i);
	__enable_irq();
	/* go to low clock after a bit of delay */
	HAL_Delay(10);
	HAL_RCC_DeInit();

	__disable_irq();
	__HAL_FLASH_DATA_CACHE_DISABLE();
	__HAL_FLASH_DATA_CACHE_RESET();
	__HAL_FLASH_INSTRUCTION_CACHE_DISABLE();
	__HAL_FLASH_INSTRUCTION_CACHE_RESET();

	G491_flash_unlock();
	G491_flash_erase(0,PagesToErase);
	G491_flash_write(flash_data,(uint8_t *)FLASH_BASE,size);
    G491_flash_Lock();
    // All done, restart
    G491_DWT_Delay_us(100000);
	__ISB();
	__DSB();

	NVIC_SystemReset();
}

#endif // #ifdef STM32G491xx

