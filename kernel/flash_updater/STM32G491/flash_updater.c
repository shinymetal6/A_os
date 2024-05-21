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

extern	uint8_t	*_fdata_start,*_fdata_end,*_d2ram_start;

uint32_t	flash_uwTick=0;
DTCM_VECTORS_DATA	uint32_t	VectorTable_DTCM[VECTOR_TABLE_SIZE];

ITCM_AREA_CODE uint8_t WaitForLastFlashOperation(uint32_t Timeout)
{
uint32_t error;

	while (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY))
	{
		flash_uwTick = 0;
		if(flash_uwTick > Timeout)
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

ITCM_AREA_CODE uint8_t flash_unlock(void)
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

ITCM_AREA_CODE uint8_t flash_Lock(void)
{
	/* Set the LOCK Bit to lock the FLASH Registers access */
	SET_BIT(FLASH->CR, FLASH_CR_LOCK);
	/* verify Flash is locked */
	if (READ_BIT(FLASH->CR, FLASH_CR_LOCK) != 0U)
		return 0;
	return 1;
}

ITCM_AREA_CODE	static void flash_Program_DoubleWord(uint32_t Address, uint64_t Data)
{
	/* Set PG bit */
	SET_BIT(FLASH->CR, FLASH_CR_PG);

	/* Program first word */
	*(uint32_t *)Address = (uint32_t)Data;
	/* Barrier to ensure programming is performed in 2 steps, in right order
	(independently of compiler optimization behavior) */
	__ISB();
	/* Program second word */
	*(uint32_t *)(Address + 4U) = (uint32_t)(Data >> 32U);
}

ITCM_AREA_CODE static void FLASH_Program_Fast(uint32_t Address, uint32_t DataAddress)
{
uint8_t row_index = (2 * FLASH_NB_DOUBLE_WORDS_IN_ROW);
uint32_t *dest_addr = (uint32_t *)Address;
uint32_t *src_addr = (uint32_t *)DataAddress;
uint32_t primask_bit;

	/* Set FSTPG bit */
	SET_BIT(FLASH->CR, FLASH_CR_FSTPG);
	/* Enter critical section: Disable interrupts to avoid any interruption during the loop */
	primask_bit = __get_PRIMASK();
	__disable_irq();
	/* Program the double words of the row */
	do
	{
		*dest_addr = *src_addr;
		dest_addr++;
		src_addr++;
		row_index--;
	}
	while (row_index != 0U);
	__set_PRIMASK(primask_bit);
}

ITCM_AREA_CODE uint8_t  FLASH_64B_data(uint32_t TypeProgram, uint32_t Address, uint64_t Data)
{
uint32_t prog_bit = 0;

	/* Wait for last operation to be completed */
	if ( WaitForLastFlashOperation((uint32_t)FLASH_G491_TIMEOUT_VALUE) == 0 )
	{
		pFlash.ErrorCode = HAL_FLASH_ERROR_NONE;

		/* Deactivate the data cache if they are activated to avoid data misbehavior */
		if(READ_BIT(FLASH->ACR, FLASH_ACR_DCEN) != 0U)
		{
			/* Disable data cache  */
			__HAL_FLASH_DATA_CACHE_DISABLE();
			pFlash.CacheToReactivate = FLASH_CACHE_DCACHE_ENABLED;
		}
		else
			pFlash.CacheToReactivate = FLASH_CACHE_DISABLED;
		if (TypeProgram == FLASH_TYPEPROGRAM_DOUBLEWORD)
		{
			/* Program double-word (64-bit) at a specified address */
			flash_Program_DoubleWord(Address, Data);
			prog_bit = FLASH_CR_PG;
		}
		else if ((TypeProgram == FLASH_TYPEPROGRAM_FAST) || (TypeProgram == FLASH_TYPEPROGRAM_FAST_AND_LAST))
		{
			/* Fast program a 32 row double-word (64-bit) at a specified address */
			FLASH_Program_Fast(Address, (uint32_t)Data);
			/* If it is the last row, the bit will be cleared at the end of the operation */
			if (TypeProgram == FLASH_TYPEPROGRAM_FAST_AND_LAST)
				prog_bit = FLASH_CR_FSTPG;
		}

		/* Wait for last operation to be completed */
		if ( WaitForLastFlashOperation((uint32_t)FLASH_G491_TIMEOUT_VALUE) != 0 )
			CLEAR_BIT(FLASH->CR, prog_bit);
		/* Flush the caches to be sure of the data consistency */
		FLASH_FlushCaches();
	}
	return 0;
}


ITCM_AREA_CODE uint32_t flash_write(uint8_t const *src, uint8_t *dst,uint32_t size)
{
uint32_t FlashAddress = (uint32_t)dst;

	if ( flash_unlock() )
		return 1;
	FlashAddress = FLASH_BASE_ADDR;

	while (FlashAddress < FLASH_END_ADDR)
	{
		if (FLASH_64B_data(FLASH_TYPEPROGRAM_FAST, FlashAddress, (uint64_t )*src) == 0)
		{
			FlashAddress = FlashAddress + (FLASH_ROW_SIZE*sizeof(uint64_t));
		}
		else
		{
			return 1;
		}
	}
	if ( flash_Lock() )
		return 1;
	return 0;
}

ITCM_AREA_CODE void  FlashIrq_Error_Handler(void)
{
	__disable_irq();
	while(1);	// hangs badly
}

void  ITCM_AREA_CODE FlashSysTick_Handler(void)
{
	flash_uwTick += (uint32_t)HAL_TICK_FREQ_DEFAULT;
}

static void relocate_vtable_systick(void)
{
uint32_t i;
	// 1 -  sync with irqs and disable all
	HAL_Delay(1);
    __disable_irq();
	for(i=0;i<8;i++)
		NVIC->ICER[i] = 0xffffffff;
	for(i=0;i<240;i++)
		NVIC->IP[i] = 0;
    __enable_irq();
	HAL_Delay(10);
    __disable_irq();
    // 2 - disable caches if enabled
    __DSB();
    __ISB();
	// 3 - compile a new vector table with only systick enabled
	for( i = 0; i < VECTOR_TABLE_SIZE; i++)
		VectorTable_DTCM[i] = (uint32_t )&(*FlashIrq_Error_Handler);
	VectorTable_DTCM[SYSTICK_VECTOR] = (uint32_t )&(*FlashSysTick_Handler);
	// 4 - set new vector table
    SCB->VTOR = ((uint32_t) &VectorTable_DTCM);
    // 5 - now only systick should be enabled, wait for some ticks after enabling irqs
    flash_uwTick=0;
    __enable_irq();
	while ( flash_uwTick < 4);
	// 7 - vectors relocated
}

ITCM_AREA_CODE void flash_update(uint8_t *flash_data,uint32_t size)
{
uint8_t	status;
	relocate_vtable_systick();
	status = flash_write(flash_data,(uint8_t *)&_fdata_start,size); // ADDR_FLASH_SECTOR_0_BANK2
    if ( status  )
    	{ while(1);	}	// error so loop forever
    // All done, restart
	NVIC_SystemReset();
}


ITCM_AREA_CODE uint32_t get_flash_storage_ptr(void)
{
	return (uint32_t )&_d2ram_start;
}

ITCM_AREA_CODE uint32_t get_flash_size(void)
{
uint32_t size = (&_fdata_end - &_fdata_start) + 32;
	return size * 4;
}

#endif // #ifdef STM32G491xx

