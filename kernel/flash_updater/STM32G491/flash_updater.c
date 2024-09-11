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

#ifdef	STM32G491xx
#include "../../system_default.h"
#include "flash_updater.h"

uint32_t	G491_flash_swTick=0;

#define	G491_RAMFUNC	 __attribute__((section(".RamFunc")))

G491_RAMFUNC void G491_DWT_Delay_us(uint32_t au32_microseconds)
{
	uint32_t au32_initial_ticks = DWT->CYCCNT;
	uint32_t au32_ticks = (HAL_RCC_GetHCLKFreq() / 1000000);
	au32_microseconds *= au32_ticks;
	while ((DWT->CYCCNT - au32_initial_ticks) < au32_microseconds-au32_ticks);
}

G491_RAMFUNC void G491_FlashDelay_1MS(void)
{
	G491_DWT_Delay_us(1000);
	G491_flash_swTick++;
}

G491_RAMFUNC uint8_t G491_WaitForLastFlashOperation(uint32_t Timeout)
{
uint32_t error;

	G491_flash_swTick = 0;
	while (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY))
	{
		G491_FlashDelay_1MS();
		if(G491_flash_swTick > Timeout)
			return 1;
	}

	/* Check FLASH operation error flags */
	error = (FLASH->SR & FLASH_FLAG_SR_ERRORS);
	if (error != 0u)
	{
		/* Save the error code */
		pFlash.ErrorCode |= error;
		/* Clear error programming flags */
		__HAL_FLASH_CLEAR_FLAG(error);
		return 1;
	}

	/* Check FLASH End of Operation flag  */
	if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_EOP))
	{
		/* Clear FLASH End of Operation pending bit */
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP);
	}
	return 0;
}

G491_RAMFUNC uint8_t G491_flash_unlock(void)
{
	if (READ_BIT(FLASH->CR, FLASH_CR_LOCK) != 0U)
	{
		/* Authorize the FLASH Registers access */
		WRITE_REG(FLASH->KEYR, FLASH_KEY1);
		WRITE_REG(FLASH->KEYR, FLASH_KEY2);

		/* verify Flash is unlocked */
		if (READ_BIT(FLASH->CR, FLASH_CR_LOCK) != 0U)
			return 1;
	}
	return 0;
}

G491_RAMFUNC uint8_t G491_flash_Lock(void)
{
	/* Set the LOCK Bit to lock the FLASH Registers access */
	SET_BIT(FLASH->CR, FLASH_CR_LOCK);
	/* verify Flash is locked */
	if (READ_BIT(FLASH->CR, FLASH_CR_LOCK) != 0U)
		return 0;
	return 1;
}


G491_RAMFUNC uint32_t G491_flash_Program_DoubleWord(uint32_t Address, uint32_t *Data)
{
	/* Set PG bit */
	SET_BIT(FLASH->CR, FLASH_CR_PG);
	/* Program first word */
	*(uint32_t *)Address = *Data;
	/* Barrier to ensure programming is performed in 2 steps, in right order
	(independently of compiler optimization behavior) */
	__ISB();
	/* Program second word */
	Data++;
	*(uint32_t *)(Address + 4U) = *Data;
	return 0;
}

G491_RAMFUNC uint32_t G491_flash_write(uint8_t *src, uint8_t *dst,uint32_t size)
{
uint32_t FlashAddress = (uint32_t)dst , i;

	if ( G491_flash_unlock() )
		return 1;
	FlashAddress = FLASH_BASE;

	for(i=0,FlashAddress = (uint32_t)dst;i<size;i+=8,FlashAddress+=8)
	{
		if ( G491_flash_Program_DoubleWord(FlashAddress, (uint32_t *)&src[i] ) )
			break;
		if ( G491_WaitForLastFlashOperation((uint32_t)FLASH_G491_TIMEOUT_VALUE) )
			break;
	}

	/* prog bit will be cleared at the end of the operation */
	if ( G491_WaitForLastFlashOperation((uint32_t)FLASH_G491_TIMEOUT_VALUE) != 0 )
		CLEAR_BIT(FLASH->CR, FLASH_CR_FSTPG);
	if ( G491_flash_Lock() )
		return 1;
	return 0;
}

G491_RAMFUNC void G491_flash_PAGE_erase(uint32_t Page)
{
	MODIFY_REG(FLASH->CR, FLASH_CR_PNB, ((Page & 0xFFU) << FLASH_CR_PNB_Pos));
	SET_BIT(FLASH->CR, FLASH_CR_PER);
	SET_BIT(FLASH->CR, FLASH_CR_STRT);
}


G491_RAMFUNC uint32_t G491_flash_erase(uint32_t NbPages)
{
HAL_StatusTypeDef status;
uint32_t page_index;

	if ( G491_flash_unlock() )
		return 1;

	G491_WaitForLastFlashOperation((uint32_t)FLASH_G491_TIMEOUT_VALUE);
	for (page_index = 0; page_index < NbPages; page_index++)
	{
		G491_flash_PAGE_erase(page_index);
		status =  G491_WaitForLastFlashOperation((uint32_t)FLASH_G491_TIMEOUT_VALUE);
		if (status != HAL_OK)
			return 1;
		/* If the erase operation is completed, disable the PER Bit */
		CLEAR_BIT(FLASH->CR, (FLASH_CR_PER | FLASH_CR_PNB));
		status =  G491_WaitForLastFlashOperation((uint32_t)FLASH_G491_TIMEOUT_VALUE);
	}
	CLEAR_BIT(FLASH->CR, (FLASH_CR_PER | FLASH_CR_PNB));
	status =  G491_WaitForLastFlashOperation((uint32_t)FLASH_G491_TIMEOUT_VALUE);
	return 0;
}

G491_RAMFUNC void flash_update(uint8_t *flash_data,uint32_t size)
{
uint8_t	status;
uint32_t NbOfPages = 30;

	__disable_irq();

	__HAL_FLASH_DATA_CACHE_DISABLE();
	__HAL_FLASH_DATA_CACHE_RESET();
	__HAL_FLASH_INSTRUCTION_CACHE_DISABLE();
	__HAL_FLASH_INSTRUCTION_CACHE_RESET();

	if (G491_flash_erase(NbOfPages))
    {
    	while(1);	// error so loop forever
    }
	status = G491_flash_write(flash_data,(uint8_t *)FLASH_BASE,size);
    if ( status  )
    {
    	while(1);	// error so loop forever
    }
    // All done, restart
	__HAL_FLASH_DATA_CACHE_DISABLE();
	__HAL_FLASH_DATA_CACHE_RESET();
	__HAL_FLASH_INSTRUCTION_CACHE_DISABLE();
	__HAL_FLASH_INSTRUCTION_CACHE_RESET();

	NVIC_SystemReset();
}

#endif // #ifdef STM32G491xx

