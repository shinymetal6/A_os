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
 * hw_spi.h
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */

#ifndef KERNEL_HWDEVICES_HW_SPI_H_
#define KERNEL_HWDEVICES_HW_SPI_H_


#define HW_SPI_ERROR_NONE		0
#define HW_SPI_ERROR			1

#define HW_SPI_TIMEOUT        2000U /* baud rate of SPI1 = 5 Mbps*/


extern	int32_t hw_spi_Send(uint32_t spi,uint8_t *pData, uint16_t Length);
extern	int32_t hw_spi_Send_DMA(uint32_t spi,uint8_t *pData, uint16_t Length);
extern	int32_t hw_spi_Recv(uint32_t spi,uint8_t *pData,  uint16_t Length);
extern	int32_t hw_spi_Recv_DMA(uint32_t spi,uint8_t *pData,  uint16_t Length);
extern	int32_t hw_spi_SendRecv(uint32_t spi,const uint8_t * const pTxData, uint8_t * const pRxData, uint16_t Length);
extern	int32_t hw_spi_SendRecv_DMA(uint32_t spi,const uint8_t * const pTxData, uint8_t * const pRxData, uint16_t Length);
extern	void	wait_dmadone(uint32_t spi);

extern	void A_hw_spi_init(void);

#endif /* KERNEL_HWDEVICES_HW_SPI_H_ */
