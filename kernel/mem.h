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
 * mem.h
 *
 *  Created on: Nov 11, 2024
 *      Author: fil
 */

#ifndef KERNEL_MEM_H_
#define KERNEL_MEM_H_

extern	uint8_t *mem_get(uint32_t size );
extern	uint32_t mem_release(uint8_t *data_ptr);

#endif /* KERNEL_MEM_H_ */
