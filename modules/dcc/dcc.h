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
 * dcc.h
 *
 *  Created on: Mar 1, 2024
 *      Author: fil
 */

#ifndef MODULES_DCC_DCC_H_
#define MODULES_DCC_DCC_H_

extern	ADC_HandleTypeDef hadc3;
extern	CRC_HandleTypeDef hcrc;
extern	DCMI_HandleTypeDef hdcmi;
extern	DMA2D_HandleTypeDef hdma2d;
extern	I2C_HandleTypeDef hi2c1;
extern	I2C_HandleTypeDef hi2c3;
extern	LTDC_HandleTypeDef hltdc;
extern	QSPI_HandleTypeDef hqspi;
extern	RTC_HandleTypeDef hrtc;
extern	SAI_HandleTypeDef hsai_BlockA2;
extern	SAI_HandleTypeDef hsai_BlockB2;
extern	SD_HandleTypeDef hsd1;
extern	SPDIFRX_HandleTypeDef hspdif;
extern	SPI_HandleTypeDef hspi2;
extern	TIM_HandleTypeDef htim1;
extern	TIM_HandleTypeDef htim2;
#define	DCC_CH2				htim2
extern	TIM_HandleTypeDef htim3;
#define	DCC_CH1				htim3
extern	TIM_HandleTypeDef htim5;
extern	TIM_HandleTypeDef htim7;
#define	TICK_TIMER			htim7
extern	TIM_HandleTypeDef htim8;
extern	TIM_HandleTypeDef htim12;
extern	DMA_HandleTypeDef hdma_tim1_ch1;
#define	DCC_DMA_CH1				hdma_tim1_ch1
extern	UART_HandleTypeDef huart1;
#define	DCC_HOST			huart1
extern	UART_HandleTypeDef huart6;
#define	HUART6			huart6
extern	SDRAM_HandleTypeDef hsdram1;

#define	DCC_HOST_MAX_LEN		64
#define	DCC_PREAMBLE_LEN		16
#define	DCC_ADDRESS_LEN			8
#define	DCC_DATA_LEN			8
#define	DCC_ECC_LEN				8
#define	DCC_FILL_LEN				8
#define	DCC_PACKET_LEN			(DCC_PREAMBLE_LEN+1+DCC_ADDRESS_LEN+1+DCC_DATA_LEN+1+DCC_ECC_LEN+1+DCC_FILL_LEN+1)
#define	PULSE100uS				99
#define	PULSE58uS				57
#define	PULSE200uS				199
#define	PULSE400uS				399
#define	DCC_0					PULSE100uS
#define	DCC_1					PULSE58uS
#define	DCC_TERM				PULSE400uS
// DCC_SEPARATOR should be DCC_0
#define	DCC_SEPARATOR			DCC_0

typedef struct {
	uint16_t		preamble[16];
	uint16_t		separator0;
	uint16_t		address[8];
	uint16_t		separator1;
	uint16_t		data[8];
	uint16_t		separator2;
	uint16_t		ecc[8];
	uint16_t		endpacket_short;
	uint16_t		fill[8];
	uint16_t		endpacket_long;
}DCC_Pkt_TypeDef;
extern	DCC_Pkt_TypeDef	DCC_Idle_Pkt;
extern	DCC_Pkt_TypeDef	DCC_Work_Pkt;

#define	NUM_DCC_PACKET		2

typedef struct _DCC_System_TypeDef
{
	uint8_t			system;
	//uint8_t			tick_flags;
	uint8_t			tick_counter;
	uint8_t			dcc_flags;
	/*
	uint8_t			uart1_flags;
	uint8_t			uart1_rxchar;
	uint8_t			uart1_rxbuf[DCC_HOST_MAX_LEN];
	uint8_t			uart1_rxindex;
	uint8_t			uart1_txbuf[DCC_HOST_MAX_LEN*4];
	uint8_t			uart1_txlen;
	*/
	uint16_t		dcc_ch1_packet[DCC_PACKET_LEN*NUM_DCC_PACKET];
	uint8_t			dcc_ch1_packet_index;
	uint16_t		dcc_ch2_packet[DCC_PACKET_LEN*NUM_DCC_PACKET];
	uint8_t			dcc_ch2_packet_index;
	uint8_t			dcc_ch1_repeat_cnt;
	uint8_t			dcc_ch1_repeat_number;
	uint8_t			dcc_ch2_repeat_cnt;
	uint8_t			dcc_ch2_repeat_number;
}DCC_System_TypeDef;
extern	DCC_System_TypeDef	DCC_System;

/* system */
#define	SYSTEM_DCC1_HALFDONE	0x80
#define	SYSTEM_DCC2_HALFDONE	0x40
#define	SYSTEM_DCC1_EXPIRED		0x20
#define	SYSTEM_DCC2_EXPIRED		0x10
#define	SYSTEM_DCC1_HPACKET		0x08
#define	SYSTEM_DCC2_HPACKET		0x04
#define	SYSTEM_DCC1_FPACKET		0x02
#define	SYSTEM_DCC2_FPACKET		0x01
/* tick_flags */
#define	TICK10ms_FLAG			0x80
/* uart1_flags */
#define	UART1_OPENFLAG			0x04
#define	UART1_BUF				0x02
#define	UART1_CH				0x01
/* dcc_flags */
#define	DCC_DCC1_POWER			0x80
#define	DCC_DCC1_RESET			0x40
#define	DCC_DCC2_POWER			0x20
#define	DCC_DCC2_RESET			0x10
#define	DCC_DCC1_PKTPEND		0x08
#define	DCC_DCC2_PKTPEND		0x04
#define	DCC_DCC1_TXDONE			0x02
#define	DCC_DCC2_TXDONE			0x01

/* errors definition */
#define	PARAMETER_ERROR			"#0001"
#define	TRACK_NOT_POWERED_ERROR	"#0002"

extern	void dcc_init(void);
extern	void dcc_loop(void);

extern	HAL_StatusTypeDef dcc_HAL_TIM_PWM_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, const uint32_t *pData, uint16_t Length);



#endif /* MODULES_DCC_DCC_H_ */
