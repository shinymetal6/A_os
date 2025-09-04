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
 * svc.h
 *
 *  Created on: Dec 4, 2024
 *      Author: fil
 */

#ifndef KERNEL_SVC_H_
#define KERNEL_SVC_H_

extern void SVC_Handler_init(void (*function),uint8_t idx);
extern void svc_call(uint8_t svc_num);

#endif /* KERNEL_SVC_H_ */
