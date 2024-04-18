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
 * lwip.h
 *
 *  Created on: Apr 13, 2024
 *      Author: fil
 */

#ifndef MODULES_LWIP_STM32H7_APP_LWIP_H_
#define MODULES_LWIP_STM32H7_APP_LWIP_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "../LwIp/src/include/lwip/opt.h"
#include "../LwIp/src/include/lwip/mem.h"
#include "../LwIp/src/include/lwip/memp.h"
#include "../LwIp/src/include/netif/etharp.h"
#include "../LwIp/src/include/lwip/dhcp.h"
#include "../LwIp/src/include/lwip/netif.h"
#include "../LwIp/src/include/lwip/timeouts.h"
#include "../Target/ethernetif.h"

extern ETH_HandleTypeDef heth;

extern	void MX_LWIP_Process(void);
extern	uint8_t MX_is_network_up(void);
extern	void MX_LWIP_HttpdStart(void);
extern	uint8_t MX_LWIP_GetIpAddress(uint8_t *hh,uint8_t *hl,uint8_t *lh,uint8_t *ll);

#ifdef __cplusplus
}
#endif


#endif /* MODULES_LWIP_STM32H7_APP_LWIP_H_ */
