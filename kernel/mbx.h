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
 * mbx.h
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */

#ifndef KERNEL_MBX_H_
#define KERNEL_MBX_H_

extern void mbx_send(uint8_t process_number,uint8_t mailbox_number,uint8_t *mbx_ptr,uint32_t mbx_size);
extern uint32_t mbx_receive(uint8_t mailbox_number,uint8_t *buf_ptr);


#endif /* KERNEL_MBX_H_ */
