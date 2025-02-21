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
 * flash_updater_common.h
 *
 *  Created on: May 20, 2024
 *      Author: fil
 */

#ifndef KERNEL_FLASH_UPDATER_FLASH_UPDATER_COMMON_H_
#define KERNEL_FLASH_UPDATER_FLASH_UPDATER_COMMON_H_

#ifdef	A_OS_STM32G4xx_PROCESSOR
extern	uint32_t flash_update(uint32_t flash_address_offset,uint8_t *data,uint32_t size);
#endif
#ifdef	STM32F446xx
extern 	uint32_t flash_update(uint8_t *flash_data,uint32_t size);
#endif
#ifdef	STM32H753xx
extern 	uint32_t flash_update(uint8_t *flash_data,uint8_t *dest_address,uint32_t size);
extern	uint32_t ConfigureBootBank(uint32_t bank);
#endif
#ifdef	STM32H743xx
extern 	uint32_t flash_update(uint8_t *flash_data,uint8_t *dest_address,uint32_t size);
extern	uint32_t ConfigureBootBank(uint32_t bank);
#endif

extern	uint32_t get_flash_storage_ptr(void);
extern 	uint32_t get_flash_size(void);

#endif /* KERNEL_FLASH_UPDATER_FLASH_UPDATER_COMMON_H_ */
