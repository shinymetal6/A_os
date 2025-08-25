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
 * sx126x.h
 *
 *  Created on: Aug 5, 2025
 *      Author: fil
 */

#ifndef DRIVERS_SPI_LORA_SX126X_H_
#define DRIVERS_SPI_LORA_SX126X_H_

// --- Modify these to match your board ---
/*
#define SX126X_SPI        &hspi1
#define SX126X_NSS_PORT   GPIOA
#define SX126X_NSS_PIN    GPIO_PIN_4
#define SX126X_RESET_PORT GPIOA
#define SX126X_RESET_PIN  GPIO_PIN_0
#define SX126X_BUSY_PORT  GPIOB
#define SX126X_BUSY_PIN   GPIO_PIN_0
#define SX126X_DIO1_PORT  GPIOB
#define SX126X_DIO1_PIN   GPIO_PIN_1
*/
// --- SX126x Commands ---
#define RADIO_GET_STATUS           0xC0
#define RADIO_GET_IRQ_STATUS       0x1D
#define RADIO_CLEAR_IRQ_STATUS     0x97
#define RADIO_SET_MODEM            0x81
#define RADIO_SET_STANDBY          0x80
#define RADIO_SET_RX               0x82
#define RADIO_SET_TX               0x83
#define RADIO_SET_RFFREQUENCY      0x86
#define RADIO_SET_TXPARAMS         0x8E
#define RADIO_SET_PACKET_TYPE      0x89
#define RADIO_SET_MODULATION_PARAMS 0x8B
#define RADIO_SET_PACKET_PARAMS    0x8C
#define RADIO_SET_BUFFER_BASE_ADDRESSES 0x8F
#define RADIO_SET_DIO_IRQ_PARAMS   0x08
#define RADIO_GET_RSSI             0x15

// --- IRQ Masks ---
#define SX126X_IRQ_TX_DONE                (1 << 1)
#define SX126X_IRQ_RX_DONE                (1 << 0)
#define SX126X_IRQ_TIMEOUT                (1 << 2)
#define SX126X_IRQ_ALL                    (SX126X_IRQ_TX_DONE | SX126X_IRQ_RX_DONE | SX126X_IRQ_TIMEOUT)

// --- Packet Type ---
#define PACKET_TYPE_LORA           0x01

// --- Standby Modes ---
#define MODE_STDBY_RC              0x00
#define MODE_STDBY_XOSC            0x01

// --- Callback Typedef ---
typedef void (*sx126x_rx_callback_t)(uint8_t *data, uint8_t size, int16_t rssi);
typedef void (*sx126x_tx_callback_t)(void);

// --- Function Declarations ---
extern	uint32_t sx126x_init(void);
void sx126x_reset(void);
void sx126x_wakeup(void);
void sx126x_wait_for_busy(void);
void sx126x_write_command(uint8_t cmd, uint8_t *data, uint16_t size);
void sx126x_read_command(uint8_t cmd, uint8_t *data, uint16_t size);
void sx126x_write_register(uint16_t addr, uint8_t *data, uint8_t size);
extern	void sx126x_write_single_register(uint16_t addr, uint8_t data);

void sx126x_read_register(uint16_t addr, uint8_t *data, uint8_t size);
extern	uint8_t sx126x_read_single_register(uint16_t addr);

void sx126x_write_buffer(uint8_t *data, uint8_t size, uint8_t offset);
void sx126x_read_buffer(uint8_t *data, uint8_t size, uint8_t offset);
void sx126x_set_standby(void);
void sx126x_set_packet_type(uint8_t packetType);
void sx126x_set_rf_frequency(uint32_t frequency);
void sx126x_set_tx_params(int8_t power);
void sx126x_set_modulation_params(uint8_t sf, uint8_t bw, uint8_t cr, uint8_t ldro);
void sx126x_set_packet_params(uint16_t preambleLen, uint8_t headerType, uint8_t payloadLen, uint8_t crc, uint8_t iq);
void sx126x_set_buffer_base(uint8_t txBase, uint8_t rxBase);
void sx126x_set_dio_irq(uint16_t irqMask, uint16_t dio1Mask);
void sx126x_set_rx(uint32_t timeoutMs);
void sx126x_set_tx(uint32_t timeoutMs);
uint16_t sx126x_get_irq_status(void);
void sx126x_clear_irq_status(uint16_t flags);
void sx126x_transmit(uint8_t *data, uint8_t size);
void sx126x_start_receive(void);
void sx126x_set_rx_callback(sx126x_rx_callback_t cb);
void sx126x_set_tx_callback(sx126x_tx_callback_t cb);
extern	uint32_t sx126x_getRSSI(void);

// --- Call from EXTI ISR ---
void sx126x_handle_dio1_irq(uint16_t GPIO_Pin);


#endif /* DRIVERS_SPI_LORA_SX126X_H_ */
