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
#include "../../system_default.h"
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

FLASH_RAM_FUNC uint8_t FLASH_32B_data(uint32_t TypeProgram, uint32_t FlashAddress, uint32_t DataAddress)
{
uint8_t status;
__IO uint32_t *dest_addr = (__IO uint32_t *)FlashAddress;
__IO uint32_t *src_addr = (__IO uint32_t*)DataAddress;
uint32_t bank;
uint8_t row_index = FLASH_NB_32BITWORD_IN_FLASHWORD;

	if(IS_FLASH_PROGRAM_ADDRESS_BANK1(FlashAddress))
		bank = FLASH_BANK_1;
	else if(IS_FLASH_PROGRAM_ADDRESS_BANK2(FlashAddress))
		bank = FLASH_BANK_2;
	else
		return 1;

	/* Wait for last operation to be completed */
	status = WaitForLastFlashOperation((uint32_t)FLASH_TIMEOUT_VALUE, bank);

	if(status == HAL_OK)
	{
		if(bank == FLASH_BANK_1)
			SET_BIT(FLASH->CR1, FLASH_CR_PG);
	}
	else
		SET_BIT(FLASH->CR2, FLASH_CR_PG);
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
uint32_t FlashAddress = (uint32_t)dst , Sector, Banks;
uint32_t	i;

	Sector = (size / FLASH_SECTOR_SIZE)+1;

	if (FlashAddress & (32-1))
		return 1; // bad alignment
	if ( flash_unlock() )
		return 1;
	if(IS_FLASH_PROGRAM_ADDRESS_BANK1(FlashAddress))
		Banks = FLASH_BANK_1;
	else if(IS_FLASH_PROGRAM_ADDRESS_BANK2(FlashAddress))
		Banks = FLASH_BANK_2;
	else
		return 1;
	for(i=0;i<Sector;i++)
	{
		if ( Flash_Erase_Sector(i, Banks) )
			return 1;
	}
	for(i=0,FlashAddress = (uint32_t)dst;i<size;i+=32,FlashAddress+=32)
	{
		if (  FLASH_32B_data(FLASH_TYPEPROGRAM_FLASHWORD, FlashAddress, (uint32_t)(src + i)) != 0 )
			return 1;
	}
	if ( flash_Lock() )
		return 1;
	return 0;
}


FLASH_RAM_FUNC void FLASH_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
  if (PinState != GPIO_PIN_RESET)
  {
    GPIOx->BSRR = GPIO_Pin;
  }
  else
  {
    GPIOx->BSRR = (uint32_t)GPIO_Pin << 16;
  }
}

FLASH_RAM_FUNC	void sys_jump(void)
{
uint32_t	*ram_ptr,i;

	__DSB();
	__ISB();
	FlashDelay_1MS();
	__DSB();
	__ISB();

	FlashDelay_1MS();

    ram_ptr = (uint32_t	*)0x00000000;
	for(i=0;i<0x10000;i+=4)
		ram_ptr[i] = 0;
    __DSB();
    __ISB();

	FlashDelay_1MS();

	ram_ptr = (uint32_t	*)0x20000000;
	for(i=0;i<0x20000;i+=4)
		ram_ptr[i] = 0;
    __DSB();
    __ISB();

	FlashDelay_1MS();
    __DSB();
    __ISB();
	FlashDelay_1MS();

#ifdef LED_3_GPIOPORT
	FLASH_GPIO_WritePin(LED_3_GPIOPORT, LED_3_GPIOBIT,GPIO_PIN_SET);
#endif
	IWDG1->KR = 0x0000cccc;
	IWDG1->KR = 0x00005555;
	IWDG1->PR = 0;
	IWDG1->RLR = 4095;
	IWDG1->WINR = 4095;
	IWDG1->KR = 0x0000aaaa;
    __DSB();
    __ISB();
    while(1);
	//NVIC_SystemReset();
}

FLASH_RAM_FUNC void flash_update(uint8_t *flash_data,uint32_t size)
{
uint8_t		status;

	HAL_RCC_DeInit();
	__disable_irq();

#ifdef LED_3_GPIOPORT
	HAL_GPIO_WritePin(LED_3_GPIOPORT, LED_3_GPIOBIT,GPIO_PIN_RESET);
#endif
#ifdef LED_2_GPIOPORT
	HAL_GPIO_WritePin(LED_2_GPIOPORT, LED_2_GPIOBIT,GPIO_PIN_SET);
#endif

	status = flash_write(flash_data,(uint8_t *)&_fdata_start,size); // ADDR_FLASH_SECTOR_0_BANK2
    if ( status  )
    {
    	while(1);	// error so loop forever
    }

    // All done, restart
#ifdef LED_3_GPIOPORT
	FLASH_GPIO_WritePin(LED_3_GPIOPORT, LED_3_GPIOBIT,GPIO_PIN_SET);
#endif
	sys_jump();
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

#endif // #ifdef	FLASH_UPDATER_ENABLED

