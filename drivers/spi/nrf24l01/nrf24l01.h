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
 * nrf24l01.h
 *
 *  Created on: Jan 12, 2026
 *      Author: fil
 */

#ifndef DRIVERS_SPI_NRF24L01_NRF24L01_H_
#define DRIVERS_SPI_NRF24L01_NRF24L01_H_

/* nucleo L053
 * nrf24_irq	CN10-34		PC4
 * spi2_sck		CN10-30		PB13
 * spi2_miso	CN10-28		PB14
 * spi2-mosi	CN10-26		PB15
 * spi2-ss		CN10-24		PB1
 * nrf_ce		CN10-22		PB2
 * pwr_3.3V		CN7-16
 * gnd			CN7-20
 *
 * lnrf24l01+pa+lna module or lnrf24l01 module
 * gnd			1	2		vcc
 * ce			3	4		spi-ss
 * sck			5	6		mosi
 * miso			7	8		irq
 *
 * CN10-22 --> module->3
 * CN10-24 --> module->4
 * CN10-26 --> module->6
 * CN10-28 --> module->7
 * CN10-30 --> module->5
 * CN10-34 --> module->8
 * CN7-16  --> module->2
 * CN7-20  --> module->1
 *
 */

#define NRF24L01_PAYLOAD_LENGTH				32     // 1 - 32bytes
#define NRF24L01_ADDRESS_LENGTH				5

typedef struct
{
	/* driver header */
	uint8_t				status;
	uint8_t				flags;
	uint8_t 			process;
	SPI_HandleTypeDef 	*bus;
	GPIO_TypeDef	 	*cs_port;
	uint16_t			cs_bit;
	uint32_t 			wakeup_id;
	uint32_t			*next_drv;
	/* driver proprietary data */
#ifdef A_OS_TIMERS_ENABLED
	TIM_HandleTypeDef	*backlight_timer;
	uint32_t			backlight_timer_channel;
#endif
	GPIO_TypeDef	 	*reset_port;
	uint16_t			reset_bit;
	uint16_t			reset_time;
	GPIO_TypeDef	 	*ce_port;
	uint16_t			ce_bit;
	GPIO_TypeDef	 	*irq_port;
	uint16_t			irq_bit;
	IRQn_Type		 	IRQ_number;
	GPIO_Interrupt_DriverStruct_t	*nrf24l01_irq_driver;
	uint32_t 			spi_timeout_ms;
	uint32_t 			device_id;
	uint8_t 			bps;
	uint8_t 			mode;
	uint16_t 			MHz;
	uint8_t 			nrf_status;
	uint8_t 			nrf_command;
	uint8_t				nrf_tx_address[NRF24L01_ADDRESS_LENGTH];
	uint8_t				nrf_rx_address[NRF24L01_ADDRESS_LENGTH];
	uint8_t				*TX_Buf;		// internal tx buffer
	uint8_t				tx_payloadLen; 	// tx len
	uint8_t				*RX_Buf;		// internal rx buffer
	uint8_t				rx_payloadLen; 	// rx len
	uint8_t				Packet_Buf[NRF24L01_PAYLOAD_LENGTH];
}SPI_NRF24L01_DriverStruct_t;

#define	DEFAULT_FREQ	2410
#define	DEFAULT_BPS		0

typedef enum
{
    _1Mbps   = 0,
    _2Mbps   = 1
} air_data_rate;

typedef enum
{
    _0dBm  = 3,
    _6dBm  = 2,
    _12dBm = 1,
    _18dBm = 0
} output_power;

/* nRF24L01+ Commands */
#define NRF24L01_CMD_R_REGISTER                 0b00000000
#define NRF24L01_CMD_W_REGISTER                 0b00100000
#define NRF24L01_CMD_R_RX_PAYLOAD               0b01100001
#define NRF24L01_CMD_W_TX_PAYLOAD               0b10100000
#define NRF24L01_CMD_FLUSH_TX                   0b11100001
#define NRF24L01_CMD_FLUSH_RX                   0b11100010
#define NRF24L01_CMD_REUSE_TX_PL                0b11100011
#define NRF24L01_CMD_R_RX_PL_WID                0b01100000
#define NRF24L01_CMD_W_ACK_PAYLOAD              0b10101000
#define NRF24L01_CMD_W_TX_PAYLOAD_NOACK         0b10110000
#define NRF24L01_CMD_NOP                        0b11111111

/* nRF24L01+ Registers */
#define NRF24L01_REG_CONFIG            			0x00
#define NRF24L01_REG_EN_AA             			0x01
#define NRF24L01_REG_EN_RXADDR         			0x02
#define NRF24L01_REG_SETUP_AW          			0x03
#define NRF24L01_REG_SETUP_RETR        			0x04
#define NRF24L01_REG_RF_CH             			0x05
#define NRF24L01_REG_RF_SETUP          			0x06
#define NRF24L01_REG_STATUS            			0x07
#define NRF24L01_REG_OBSERVE_TX        			0x08    // Read-Only
#define NRF24L01_REG_RPD               			0x09    // Read-Only
#define NRF24L01_REG_RX_ADDR_P0        			0x0A
#define NRF24L01_REG_RX_ADDR_P1        			0x0B
#define NRF24L01_REG_RX_ADDR_P2        			0x0C
#define NRF24L01_REG_RX_ADDR_P3        			0x0D
#define NRF24L01_REG_RX_ADDR_P4        			0x0E
#define NRF24L01_REG_RX_ADDR_P5        			0x0F
#define NRF24L01_REG_TX_ADDR           			0x10
#define NRF24L01_REG_RX_PW_P0          			0x11
#define NRF24L01_REG_RX_PW_P1          			0x12
#define NRF24L01_REG_RX_PW_P2          			0x13
#define NRF24L01_REG_RX_PW_P3          			0x14
#define NRF24L01_REG_RX_PW_P4          			0x15
#define NRF24L01_REG_RX_PW_P5          			0x16
#define NRF24L01_REG_FIFO_STATUS       			0x17
#define NRF24L01_REG_DYNPD             			0x1C
#define NRF24L01_REG_FEATURE           			0x1D

#define NRF24L01_1Mbps							0
#define NRF24L01_2Mbps							1
#define NRF24L01_MODE_TX						0
#define NRF24L01_MODE_RX						1
#define NRF24L01_IRQ_IS_TX_DR					0x40
#define NRF24L01_IRQ_IS_TX_DS					0x20
#define NRF24L01_IRQ_IS_MAX_RT					0x10

#define NRF24L01_ERROR							0xff
#define NRF24L01_SUCCESS						0x00

#define	NRF24L01_SPI_TIMEOUT					100

extern uint32_t spi_nrf24l01_write_register(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv,uint8_t reg, uint8_t value);
extern uint32_t spi_nrf24l01_write_multiple_register(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv,uint8_t reg, uint8_t *values,uint8_t reg_num);
extern uint32_t spi_nrf24l01_read_register(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv,uint8_t reg);
extern uint32_t spi_nrf24l01_flush_rx_fifo(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv);
extern uint32_t spi_nrf24l01_flush_tx_fifo(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv);
extern uint32_t spi_nrf24l01_read_rx_fifo(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv,uint8_t* rx_payload);
extern uint32_t spi_nrf24l01_write_tx_fifo(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv,uint8_t* tx_payload);
extern uint32_t spi_nrf24l01_rx(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv,uint8_t* rx_payload );
extern uint32_t spi_nrf24l01_set_rx_address(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv,uint8_t* rx_address );
extern uint32_t spi_nrf24l01_get_tx_irq_goto_rx(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv);
extern uint32_t spi_nrf24l01_get_status(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv);
extern uint32_t spi_nrf24l01_get_mode(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv);
extern uint32_t spi_nrf24l01_tx(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv,uint8_t* tx_payload , uint8_t* tx_address);
extern uint32_t	spi_nrf24l01_init(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv);
extern uint32_t	spi_nrf24l01_register(SPI_NRF24L01_DriverStruct_t *spi_nrf24l01_Drv);

#endif /* DRIVERS_SPI_NRF24L01_NRF24L01_H_ */
