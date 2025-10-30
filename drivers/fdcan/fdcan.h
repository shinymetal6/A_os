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
 * fdcan.h
 *
 *  Created on: Jun 23, 2025
 *      Author: fil
 */

#ifndef DRIVERS_FDCAN_FDCAN_H_
#define DRIVERS_FDCAN_FDCAN_H_

#ifdef A_OS_CAN_ENABLED

extern	FDCAN_HandleTypeDef hfdcan1;
extern	FDCAN_HandleTypeDef hfdcan2;

typedef struct
{
	uint8_t 			process;
	uint8_t				status;
	uint8_t				flags;
	uint32_t 			*next_fdcan;
	//uint8_t				channel;
	uint32_t 			wakeup_id;
	FDCAN_HandleTypeDef	*hfdcan;
	FDCAN_TxHeaderTypeDef *TxHeader;
	FDCAN_RxHeaderTypeDef *RxHeader;
	FDCAN_FilterTypeDef *FilterConfig;
	uint8_t 			*TxData;
	uint8_t 			tx_len;
	uint8_t 			*RxData;
	uint8_t 			rx_len;
}FDCAN_Drv_TypeDef;

/* status */
#define	FDCAN_DRIVER_STATUS_IN_USE		0x80
#define	FDCAN_DRIVER_STATUS_INITIALIZED	0x40
#define	FDCAN_DRIVER_STATUS_INITPEND	0x20
#define	FDCAN_DRIVER_STATUS_REQUESTED	0x10
#define	FDCAN_DRIVER_STATUS_FAILED		0x01
#define	FDCAN_DRIVER_STATUS_UNUSED		0

/* flags */
#define	FDCAN_WAKEUP_ON_RX0		0x80
#define	FDCAN_WAKEUP_ON_RX1		0x40
#define	FDCAN_WAKEUP_ON_TX		0x20
#define	FDCAN_WAKEUP_ON_RXCHAR	0x10
#define	FDCAN_WAKEUP_ON_RXFULL	0x08
#define	FDCAN_WAKEUP_ON_TIMEOUT	0x04
#define	FDCAN_WAKEUP_ON_ERRORS	0x02


extern	uint32_t	can_register(FDCAN_Drv_TypeDef *private_data);
extern 	uint32_t 	can_update_header(FDCAN_Drv_TypeDef *private_data, FDCAN_TxHeaderTypeDef *TxHeader);
extern 	uint32_t 	can_send(FDCAN_Drv_TypeDef *private_data);

#endif // #ifdef A_OS_CAN_ENABLED
#endif /* DRIVERS_FDCAN_FDCAN_H_ */
