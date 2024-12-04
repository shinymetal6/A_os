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

extern int32_t call_svc(int8_t svc_index,int32_t param1 , int32_t param2 , int32_t param3);


#endif /* KERNEL_SVC_H_ */
