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

#ifdef	STM32F446xx
#include "../../system_default.h"
#include "flash_updater.h"

extern	uint8_t	*_fdata_start,*_fdata_end,*_d2ram_start;

uint32_t	flash_uwTick=0;
DTCM_VECTORS_DATA	uint32_t	VectorTable_DTCM[VECTOR_TABLE_SIZE];

ITCM_AREA_CODE uint8_t WaitForLastFlashOperation(uint32_t Timeout)
{
	while (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY))
	{
		flash_uwTick = 0;
		if(flash_uwTick > Timeout)
			return 1;
	}

	/* Check FLASH End of Operation flag  */
	if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_EOP) != RESET)
	{
	/* Clear FLASH End of Operation pending bit */
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP);
	}
	#if defined(FLASH_SR_RDERR)
	if(__HAL_FLASH_GET_FLAG((FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR | FLASH_FLAG_RDERR)) != RESET)
	#else
	if(__HAL_FLASH_GET_FLAG((FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR)) != RESET)
	#endif /* FLASH_SR_RDERR */
		return 1;

	/* If there is no error flag set */
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

ITCM_AREA_CODE	static void flash_Program_Byte(uint32_t Address, uint8_t Data)
{
	CLEAR_BIT(FLASH->CR, FLASH_CR_PSIZE);
	FLASH->CR |= FLASH_PSIZE_BYTE;
	FLASH->CR |= FLASH_CR_PG;
	*(__IO uint8_t*)Address = Data;
}

ITCM_AREA_CODE	static void flash_Program_HalfWord(uint32_t Address, uint16_t Data)
{
	CLEAR_BIT(FLASH->CR, FLASH_CR_PSIZE);
	FLASH->CR |= FLASH_PSIZE_HALF_WORD;
	FLASH->CR |= FLASH_CR_PG;
	*(__IO uint16_t*)Address = Data;
}

ITCM_AREA_CODE	static void flash_Program_Word(uint32_t Address, uint32_t Data)
{
	CLEAR_BIT(FLASH->CR, FLASH_CR_PSIZE);
	FLASH->CR |= FLASH_PSIZE_WORD;
	FLASH->CR |= FLASH_CR_PG;
	*(__IO uint32_t*)Address = Data;
}

ITCM_AREA_CODE static void flash_Program_DoubleWord(uint32_t Address, uint64_t Data)
{
  CLEAR_BIT(FLASH->CR, FLASH_CR_PSIZE);
  FLASH->CR |= FLASH_PSIZE_DOUBLE_WORD;
  FLASH->CR |= FLASH_CR_PG;
  /* Program first word */
  *(__IO uint32_t*)Address = (uint32_t)Data;
  __ISB();
  *(__IO uint32_t*)(Address+4) = (uint32_t)(Data >> 32);
}

ITCM_AREA_CODE uint8_t  FLASH_64B_data(uint32_t TypeProgram, uint32_t Address, uint64_t Data)
{
	if(WaitForLastFlashOperation((uint32_t)FLASH_F446_TIMEOUT_VALUE) == 0)
	{
		flash_Program_DoubleWord(Address, Data);
		if(WaitForLastFlashOperation((uint32_t)(FLASH_F446_TIMEOUT_VALUE)) == 1)
			return 1;
		/* Disable the PG Bit */
		FLASH->CR &= (~FLASH_CR_PG);
		return 0;
	}
	return 1;
}

ITCM_AREA_CODE uint8_t  FLASH_32B_data(uint32_t Address, uint32_t Data)
{
	if(WaitForLastFlashOperation((uint32_t)FLASH_F446_TIMEOUT_VALUE) == 0)
	{
		flash_Program_Word(Address, (uint32_t) Data);

		if(WaitForLastFlashOperation((uint32_t)(FLASH_F446_TIMEOUT_VALUE)) == 1)
			return 1;
		/* Disable the PG Bit */
		FLASH->CR &= (~FLASH_CR_PG);
		return 0;
	}
	return 1;
}

ITCM_AREA_CODE	void flash_mass_erase(void)
{
	  CLEAR_BIT(FLASH->CR, FLASH_CR_PSIZE);
	  FLASH->CR |= FLASH_CR_MER;
	  FLASH->CR |= FLASH_CR_STRT | ((uint32_t)FLASH_VOLTAGE_RANGE_3 << 8U);
}

ITCM_AREA_CODE uint32_t flash_erase(uint32_t erase_type)
{

	if ( flash_Lock() )
		return 1;

	/* Wait for last operation to be completed */
	if(WaitForLastFlashOperation((uint32_t)FLASH_F446_TIMEOUT_VALUE) == 0)
	{
		flash_mass_erase();
		if(WaitForLastFlashOperation((uint32_t)FLASH_F446_TIMEOUT_VALUE) == 0)
		{
			/* Flush the caches to be sure of the data consistency */
			/* Flush instruction cache  */
			if (READ_BIT(FLASH->ACR, FLASH_ACR_ICEN) != RESET)
			{
				/* Disable instruction cache  */
				__HAL_FLASH_INSTRUCTION_CACHE_DISABLE();
				/* Reset instruction cache */
				__HAL_FLASH_INSTRUCTION_CACHE_RESET();
				/* Enable instruction cache */
				__HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
			}
			/* Flush data cache */
			if (READ_BIT(FLASH->ACR, FLASH_ACR_DCEN) != RESET)
			{
				/* Disable data cache  */
				__HAL_FLASH_DATA_CACHE_DISABLE();
				/* Reset data cache */
				__HAL_FLASH_DATA_CACHE_RESET();
				/* Enable data cache */
				__HAL_FLASH_DATA_CACHE_ENABLE();
			}
			return 0;
		}
	}
	return 1;
}


ITCM_AREA_CODE uint32_t flash_write(uint8_t const *src, uint8_t *dst,uint32_t size)
{
uint32_t FlashAddress = (uint32_t)dst;

	if ( flash_unlock() )
		return 1;
	if ( flash_erase(0) == 0 )
	{
		FlashAddress = FLASH_BASE_ADDR;
		while (FlashAddress < FLASH_END_ADDR)
		{
			if ( HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FlashAddress, (uint64_t )src) == 0 )
				FlashAddress = FlashAddress + (FLASH_ROW_SIZE*sizeof(uint64_t));
			else
				return 1;
		}
		if ( flash_Lock() )
			return 1;
		return 0;
	}
	return 1;
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

/*
ITCM_AREA_CODE uint32_t get_flash_storage_ptr(void)
{
	return (uint32_t )&_d2ram_start;
}
*/
ITCM_AREA_CODE uint32_t get_flash_size(void)
{
uint32_t size = (&_fdata_end - &_fdata_start) + 32;
	return size * 4;
}

#endif // #ifdef STM32F446xx

