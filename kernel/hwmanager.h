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
 * hwmanager.h
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */

#ifndef KERNEL_HWMANAGER_H_
#define KERNEL_HWMANAGER_H_

extern	uint32_t allocate_hw(uint8_t peripheral,uint8_t config);
extern	uint32_t deallocate_hw(uint8_t peripheral);
extern	uint32_t allocate_hw_with_irq_callback(uint8_t bus_peripheral,uint8_t device_peripheral,uint8_t config,void (*irq_callback)(void));
extern	uint8_t get_busdevice_from_device(uint8_t device_peripheral);

#endif /* KERNEL_HWMANAGER_H_ */
