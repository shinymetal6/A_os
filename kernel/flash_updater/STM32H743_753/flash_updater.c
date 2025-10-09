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
 *  Created on: Jun 6, 2024
 *      Author: fil
 */

#include "main.h"

#ifdef	STM32H753xx
#define FLASH_UPDATER_ENABLED
#endif
#ifdef	STM32H743xx
#define FLASH_UPDATER_ENABLED
#endif

#ifdef	FLASH_UPDATER_ENABLED
#include "../../A.h"
#include "../../A_exported_functions.h"
#include "flash_updater.h"

extern	uint8_t	*_fdata_start,*_fdata_end,*_d2ram_start;

uint32_t	flash_swTick=0;
#define	FLASH_RAM_FUNC		__attribute__((section(".RamFunc")))


FLASH_RAM_FUNC void FlashDWT_Delay_us(uint32_t au32_microseconds)
{
uint32_t au32_initial_ticks = DWT->CYCCNT;
uint32_t au32_ticks = (HSI_VALUE / 1000000);
	au32_microseconds *= au32_ticks;
	while ((DWT->CYCCNT - au32_initial_ticks) < au32_microseconds-au32_ticks);
}

FLASH_RAM_FUNC void FlashDelay_1MS(void)
{
	FlashDWT_Delay_us(1000);
	flash_swTick++;
}

FLASH_RAM_FUNC uint8_t WaitForLastFlashOperation(uint32_t Timeout, uint32_t Bank)
{
uint32_t bsyflag = FLASH_FLAG_QW_BANK1;
uint32_t errorflag = 0;

	if (Bank == FLASH_BANK_2)
		bsyflag = FLASH_FLAG_QW_BANK2;

	flash_swTick = 0;
	while(__HAL_FLASH_GET_FLAG(bsyflag))
	{
		FlashDelay_1MS();
		if(flash_swTick > Timeout)
			return 1;
	}

	if (Bank == FLASH_BANK_1)
		errorflag = FLASH->SR1 & FLASH_FLAG_ALL_ERRORS_BANK1;
	else
		errorflag = (FLASH->SR2 & FLASH_FLAG_ALL_ERRORS_BANK2) | 0x80000000U;

	if((errorflag & 0x7FFFFFFFU) != 0U)
		return 1;

	if(Bank == FLASH_BANK_1)
	{
		if (__HAL_FLASH_GET_FLAG_BANK1(FLASH_FLAG_EOP_BANK1))
			__HAL_FLASH_CLEAR_FLAG_BANK1(FLASH_FLAG_EOP_BANK1);
	}
	else
	{
		if (__HAL_FLASH_GET_FLAG_BANK2(FLASH_FLAG_EOP_BANK2))
			__HAL_FLASH_CLEAR_FLAG_BANK2(FLASH_FLAG_EOP_BANK2);
	}
	return 0;
}

FLASH_RAM_FUNC uint8_t flash_unlock(void)
{
	if(READ_BIT(FLASH->CR1, FLASH_CR_LOCK) != 0U)
	{
		/* Authorize the FLASH Bank1 Registers access */
		WRITE_REG(FLASH->KEYR1, FLASH_KEY1);
		WRITE_REG(FLASH->KEYR1, FLASH_KEY2);

		/* Verify Flash Bank1 is unlocked */
		if (READ_BIT(FLASH->CR1, FLASH_CR_LOCK) != 0U)
			return 1;
	}

	if(READ_BIT(FLASH->CR2, FLASH_CR_LOCK) != 0U)
	{
		/* Authorize the FLASH Bank2 Registers access */
		WRITE_REG(FLASH->KEYR2, FLASH_KEY1);
		WRITE_REG(FLASH->KEYR2, FLASH_KEY2);

		/* Verify Flash Bank2 is unlocked */
		if (READ_BIT(FLASH->CR2, FLASH_CR_LOCK) != 0U)
			return 1;
	}
	return 0;
}

FLASH_RAM_FUNC uint8_t flash_Lock(void)
{
	SET_BIT(FLASH->CR1, FLASH_CR_LOCK);

	if (READ_BIT(FLASH->CR1, FLASH_CR_LOCK) == 0U)
		return 1;

	/* Set the LOCK Bit to lock the FLASH Bank2 Control Register access */
	SET_BIT(FLASH->CR2, FLASH_CR_LOCK);

	/* Verify Flash Bank2 is locked */
	if (READ_BIT(FLASH->CR2, FLASH_CR_LOCK) == 0U)
		return 1;

	return 0;
}

FLASH_RAM_FUNC uint8_t Flash_Erase_Sector(uint32_t Sector, uint32_t Banks)
{
  if((Banks & FLASH_BANK_1) == FLASH_BANK_1)
  {
    /* Reset Program/erase VoltageRange and Sector Number for Bank1 */
    FLASH->CR1 &= ~(FLASH_CR_PSIZE | FLASH_CR_SNB);
    FLASH->CR1 |= (FLASH_CR_SER | FLASH_VOLTAGE_RANGE_3 | (Sector << FLASH_CR_SNB_Pos) | FLASH_CR_START);
  }
  if((Banks & FLASH_BANK_2) == FLASH_BANK_2)
  {
    FLASH->CR2 &= ~(FLASH_CR_PSIZE | FLASH_CR_SNB);
    FLASH->CR2 |= (FLASH_CR_SER | FLASH_VOLTAGE_RANGE_3  | (Sector << FLASH_CR_SNB_Pos) | FLASH_CR_START);
  }
  return WaitForLastFlashOperation((uint32_t)FLASH_TIMEOUT_VALUE, Banks);
}


FLASH_RAM_FUNC static uint8_t Flash_Erase_Sector_by_address(uint32_t Address, uint32_t Bank)
{
uint32_t Sector = 0;

	if(((Address < ADDR_FLASH_SECTOR_1_BANK1) && (Address >= ADDR_FLASH_SECTOR_0_BANK1)) || \
	((Address < ADDR_FLASH_SECTOR_1_BANK2) && (Address >= ADDR_FLASH_SECTOR_0_BANK2)))
	{
		Sector = FLASH_SECTOR_0;
	}
	else if(((Address < ADDR_FLASH_SECTOR_2_BANK1) && (Address >= ADDR_FLASH_SECTOR_1_BANK1)) || \
	((Address < ADDR_FLASH_SECTOR_2_BANK2) && (Address >= ADDR_FLASH_SECTOR_1_BANK2)))
	{
		Sector = FLASH_SECTOR_1;
	}
	else if(((Address < ADDR_FLASH_SECTOR_3_BANK1) && (Address >= ADDR_FLASH_SECTOR_2_BANK1)) || \
	((Address < ADDR_FLASH_SECTOR_3_BANK2) && (Address >= ADDR_FLASH_SECTOR_2_BANK2)))
	{
		Sector = FLASH_SECTOR_2;
	}
	else if(((Address < ADDR_FLASH_SECTOR_4_BANK1) && (Address >= ADDR_FLASH_SECTOR_3_BANK1)) || \
	((Address < ADDR_FLASH_SECTOR_4_BANK2) && (Address >= ADDR_FLASH_SECTOR_3_BANK2)))
	{
		Sector = FLASH_SECTOR_3;
	}
	else if(((Address < ADDR_FLASH_SECTOR_5_BANK1) && (Address >= ADDR_FLASH_SECTOR_4_BANK1)) || \
	((Address < ADDR_FLASH_SECTOR_5_BANK2) && (Address >= ADDR_FLASH_SECTOR_4_BANK2)))
	{
		Sector = FLASH_SECTOR_4;
	}
	else if(((Address < ADDR_FLASH_SECTOR_6_BANK1) && (Address >= ADDR_FLASH_SECTOR_5_BANK1)) || \
	((Address < ADDR_FLASH_SECTOR_6_BANK2) && (Address >= ADDR_FLASH_SECTOR_5_BANK2)))
	{
		Sector = FLASH_SECTOR_5;
	}
	else if(((Address < ADDR_FLASH_SECTOR_7_BANK1) && (Address >= ADDR_FLASH_SECTOR_6_BANK1)) || \
	((Address < ADDR_FLASH_SECTOR_7_BANK2) && (Address >= ADDR_FLASH_SECTOR_6_BANK2)))
	{
		Sector = FLASH_SECTOR_6;
	}
	else if(((Address < ADDR_FLASH_SECTOR_0_BANK2) && (Address >= ADDR_FLASH_SECTOR_7_BANK1)) || \
	((Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_7_BANK2)))
	{
		Sector = FLASH_SECTOR_7;
	}
	else
	{
		return 1;
	}
	if((Bank & FLASH_BANK_1) == FLASH_BANK_1)
	{
		/* Reset Program/erase VoltageRange and Sector Number for Bank1 */
		FLASH->CR1 &= ~(FLASH_CR_PSIZE | FLASH_CR_SNB);
		FLASH->CR1 |= (FLASH_CR_SER | FLASH_VOLTAGE_RANGE_3 | (Sector << FLASH_CR_SNB_Pos) | FLASH_CR_START);
	}
	if((Bank & FLASH_BANK_2) == FLASH_BANK_2)
	{
		/* Reset Program/erase VoltageRange and Sector Number for Bank2 */
		FLASH->CR2 &= ~(FLASH_CR_PSIZE | FLASH_CR_SNB);
		FLASH->CR2 |= (FLASH_CR_SER | FLASH_VOLTAGE_RANGE_3  | (Sector << FLASH_CR_SNB_Pos) | FLASH_CR_START);
	}

	return WaitForLastFlashOperation((uint32_t)FLASH_TIMEOUT_VALUE, Bank);
}

FLASH_RAM_FUNC uint8_t FLASH_32B_data(uint32_t FlashAddress, uint32_t DataAddress)
{
uint8_t status;
__IO uint32_t *dest_addr = (__IO uint32_t *)FlashAddress;
__IO uint32_t *src_addr = (__IO uint32_t*)DataAddress;
uint32_t bank;
uint8_t row_index = FLASH_NB_32BITWORD_IN_FLASHWORD;

	if(IS_FLASH_PROGRAM_ADDRESS_BANK1(FlashAddress))
	{
		bank = FLASH_BANK_1;
		status = WaitForLastFlashOperation((uint32_t)FLASH_TIMEOUT_VALUE, bank);
		if(status == HAL_OK)
			SET_BIT(FLASH->CR1, FLASH_CR_PG);
	}
	else if(IS_FLASH_PROGRAM_ADDRESS_BANK2(FlashAddress))
	{
		bank = FLASH_BANK_2;
		status = WaitForLastFlashOperation((uint32_t)FLASH_TIMEOUT_VALUE, bank);
		if(status == HAL_OK)
			SET_BIT(FLASH->CR2, FLASH_CR_PG);
	}
	else
		return 1;

	__ISB();
	__DSB();

	do
	{
		*dest_addr = *src_addr;
		dest_addr++;
		src_addr++;
		row_index--;
	} while (row_index != 0U);

	__ISB();
	__DSB();

	/* Wait for last operation to be completed */
	status = WaitForLastFlashOperation((uint32_t)FLASH_TIMEOUT_VALUE, bank);

	if(bank == FLASH_BANK_1)
		CLEAR_BIT(FLASH->CR1, FLASH_CR_PG);
	else
		CLEAR_BIT(FLASH->CR2, FLASH_CR_PG);

	return status;
}

FLASH_RAM_FUNC uint32_t flash_write(uint8_t const *src, uint8_t *dst,uint32_t size)
{
uint32_t FlashAddress , Sector, Bank;
uint32_t	i;

	__disable_irq();
	FlashAddress = (uint32_t)dst;
	if (FlashAddress & (32-1))
		return 1; // bad alignment
	if(IS_FLASH_PROGRAM_ADDRESS_BANK1(FlashAddress))
		Bank = FLASH_BANK_1;
	else if(IS_FLASH_PROGRAM_ADDRESS_BANK2(FlashAddress))
		Bank = FLASH_BANK_2;
	else
		return 1;

	if ( flash_unlock() )
		return 1;

//	Sector = (size / FLASH_SECTOR_SIZE)+1;
	Sector = (size / FLASH_SECTOR_SIZE);
	if ( Sector == 0 )
		Sector = 1;
	for(i=0;i<Sector;i++)
	{
		if ( Flash_Erase_Sector_by_address(FlashAddress, Bank) )
			return 1;
		FlashAddress += FLASH_SECTOR_SIZE;
	}
	FlashAddress = (uint32_t)dst;
	for(i=0,FlashAddress = (uint32_t)dst;i<size;i+=32,FlashAddress+=32)
	{
		if (  FLASH_32B_data(FlashAddress, (uint32_t)(src + i)) != 0 )
			return 1;
	}
	if ( flash_Lock() )
		return 1;
	__enable_irq();
	return 0;
}

FLASH_RAM_FUNC uint32_t flash_erase_by_address(uint8_t *flash_address,uint32_t size)
{
uint32_t FlashAddress , Sector, Bank;
uint32_t	i;

	__disable_irq();
	FlashAddress = (uint32_t)flash_address;
	if (FlashAddress & (32-1))
		return 1; // bad alignment
	if(IS_FLASH_PROGRAM_ADDRESS_BANK1(FlashAddress))
		Bank = FLASH_BANK_1;
	else if(IS_FLASH_PROGRAM_ADDRESS_BANK2(FlashAddress))
		Bank = FLASH_BANK_2;
	else
		return 1;

	if ( flash_unlock() )
		return 1;

	Sector = (size / FLASH_SECTOR_SIZE);
	if ( Sector == 0 )
		Sector = 1;
	for(i=0;i<Sector;i++)
	{
		if ( Flash_Erase_Sector_by_address(FlashAddress, Bank) )
			return 1;
		FlashAddress += FLASH_SECTOR_SIZE;
	}
	if ( flash_Lock() )
		return 1;
	__enable_irq();
	return 0;
}

FLASH_RAM_FUNC uint32_t flash_update(uint8_t *flash_data,uint8_t *dest_address,uint32_t size)
{
uint32_t ret_val=0;
    if ( (ret_val = flash_write(flash_data,dest_address,size) ) )
    {
    	__disable_irq();
    	while(1);	// error so loop forever
    }
    return ret_val;
}

FLASH_RAM_FUNC uint32_t get_flash_storage_ptr(void)
{
	return (uint32_t )&_d2ram_start;
}

FLASH_RAM_FUNC uint32_t get_flash_size(void)
{
uint32_t size = (&_fdata_end - &_fdata_start) + 32;
	return size * 4;
}

uint32_t ConfigureBootBank(uint32_t bank)
{
uint32_t switched = 0;
	SCB_DisableICache();
	HAL_FLASH_Unlock();
	HAL_FLASH_OB_Unlock();

	FLASH_OBProgramInitTypeDef OBInit;
	OBInit.Banks     = FLASH_BANK_1;
	HAL_FLASHEx_OBGetConfig(&OBInit);
	/* Check Swap flash memory banks status */
	if ((OBInit.USERConfig & OB_SWAP_BANK_ENABLE) == OB_SWAP_BANK_DISABLE)
	{
		if ( bank == 2 )
		{
			/*Swap to bank2 */
			/*Set OB SWAP_BANK_OPT to swap Bank2*/
			OBInit.OptionType = OPTIONBYTE_USER;
			OBInit.USERType   = OB_USER_SWAP_BANK;
			OBInit.USERConfig = OB_SWAP_BANK_ENABLE;
			HAL_FLASHEx_OBProgram(&OBInit);
			/* Launch Option bytes loading */
			HAL_FLASH_OB_Launch();
			switched = 1;
		}
	}
	else
	{
		if ( bank == 1 )
		{
			/* Swap to bank1 */
			/*Set OB SWAP_BANK_OPT to swap Bank1*/
			OBInit.OptionType = OPTIONBYTE_USER;
			OBInit.USERType = OB_USER_SWAP_BANK;
			OBInit.USERConfig = OB_SWAP_BANK_DISABLE;
			HAL_FLASHEx_OBProgram(&OBInit);
			/* Launch Option bytes loading */
			HAL_FLASH_OB_Launch();
			switched = 1;
		}
	}
	if ( switched )
	{
	    HAL_FLASH_OB_Lock();
	    HAL_FLASH_Lock();
		__DSB();
		__ISB();
		FlashDelay_1MS();
		IWDG1->KR = 0x0000cccc;
		IWDG1->KR = 0x00005555;
		IWDG1->PR = 0;
		IWDG1->RLR = 4095;
		IWDG1->WINR = 4095;
		IWDG1->KR = 0x0000aaaa;
	    __DSB();
	    __ISB();
	    while(1);
	}
    return 0;
}

/*
 * Probe read access to address in the MCU memory space
 * Return: true = OK, false = error
 * Supported: Cortex-M3, M4/F, M7
 * NOTE: call this with interrupts disabled to avoid side effects
 */

uint8_t  flashval;
uint32_t busfault_CFSR;
FLASH_RAM_FUNC uint32_t flash_probe_address(uint8_t *address)
{
uint32_t BFARVALID_MASK = (0x80 << SCB_CFSR_BUSFAULTSR_Pos);
uint32_t ret_val = 0;


    /* Clear BFARVALID flag by writing 1 to it */
    SCB->CFSR |= BFARVALID_MASK;

    /* Ignore BusFault by enabling BFHFNMIGN; disable faults and interrupts */
    uint32_t mask = __get_FAULTMASK();
    __disable_fault_irq();
    SCB->CCR |= SCB_CCR_BFHFNMIGN_Msk;

    /* probe the address by performing 8-bit read */
    __DSB();
    flashval = *address;
    __DMB();

    busfault_CFSR = SCB->CFSR ;
    if (SCB->CFSR & BFARVALID_MASK)
    {
        /* Yes, Bus Fault occurred */
    	ret_val = 1;
    }

    /* Re-enable BusFault by clearing  BFHFNMIGN */
    SCB->CCR &= ~SCB_CCR_BFHFNMIGN_Msk;
    __set_FAULTMASK(mask);
    __enable_fault_irq();
    __DSB();

    return ret_val;
}

#endif // #ifdef	FLASH_UPDATER_ENABLED

