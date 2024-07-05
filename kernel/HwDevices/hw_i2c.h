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
 * hw_i2c.h
 *
 *  Created on: Mar 4, 2024
 *      Author: fil
 */

#ifndef KERNEL_HWDEVICES_HW_I2C_H_
#define KERNEL_HWDEVICES_HW_I2C_H_

#define HW_I2C_ERROR_NONE		0
#define HW_I2C_ERROR			1

#define	I2C_RETRY_CHECK			5
#define HW_I2C_TIMEOUT			1000U

extern	uint8_t hw_i2c_check_presence(uint32_t i2c,uint16_t device_address);
extern	int32_t hw_i2c_Send(uint32_t i2c,uint16_t device_address,uint8_t *pData, uint16_t Length);
extern	int32_t hw_i2c_Get(uint32_t i2c,uint16_t device_address,uint8_t *pData, uint16_t Length);
extern	int32_t hw_i2c_MemSend8(uint32_t i2c,uint16_t device_address,uint16_t internal_address,uint8_t *pData, uint16_t Length);
extern	int32_t hw_i2c_MemSend16(uint32_t i2c,uint16_t device_address,uint16_t internal_address,uint8_t *pData, uint16_t Length);
extern	int32_t hw_i2c_MemGet8(uint32_t i2c,uint16_t device_address,uint16_t internal_address,uint8_t *pData, uint16_t Length);
extern	int32_t hw_i2c_MemGet16(uint32_t i2c,uint16_t device_address,uint16_t internal_address,uint8_t *pData, uint16_t Length);
extern	uint32_t hw_i2c_Get_IT_State(uint32_t i2c);
extern	uint32_t hw_i2c_Get_TxDMA_State(uint32_t i2c);
extern	uint32_t hw_i2c_Get_RxDMA_State(uint32_t i2c);
extern	int32_t hw_i2c_MemSend16_IT(uint32_t i2c,uint16_t device_address,uint16_t internal_address,uint8_t *pData, uint16_t Length);
extern	int32_t hw_i2c_MemSend16_DMA(uint32_t i2c,uint16_t device_address,uint16_t internal_address,uint8_t *pData, uint16_t Length);
extern	int32_t hw_i2c_MemGet16_DMA(uint32_t i2c,uint16_t device_address,uint16_t internal_address,uint8_t *pData, uint16_t Length);

extern	void A_hw_i2c_init(void);



#endif /* KERNEL_HWDEVICES_HW_I2C_H_ */
