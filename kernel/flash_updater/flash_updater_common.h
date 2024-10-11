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

#ifdef	STM32G491xx
extern	void flash_update(uint32_t flash_address_offset,uint8_t *data,uint32_t size);
#endif
#ifdef	STM32F446xx
extern void flash_update(uint8_t *flash_data,uint32_t size);
#endif
#ifdef	STM32H753xx
extern void flash_update(uint8_t *flash_data,uint32_t size);
#endif
#ifdef	STM32H743xx
extern void flash_update(uint8_t *flash_data,uint32_t size);
#endif

#endif /* KERNEL_FLASH_UPDATER_FLASH_UPDATER_COMMON_H_ */
