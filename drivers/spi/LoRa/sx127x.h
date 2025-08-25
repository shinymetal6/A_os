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
 * sx127x.h
 *
 *  Created on: Aug 5, 2025
 *      Author: fil
 */

#ifndef DRIVERS_SPI_LORA_SX127X_H_
#define DRIVERS_SPI_LORA_SX127X_H_

// --- SPI & GPIO Configuration (Update to match your board) ---
/*
#define SX127X_SPI        &hspi1
#define SX127X_NSS_PORT   GPIOA
#define SX127X_NSS_PIN    GPIO_PIN_4
#define SX127X_RESET_PORT GPIOA
#define SX127X_RESET_PIN  GPIO_PIN_0
#define SX127X_DIO0_PORT  GPIOB
#define SX127X_DIO0_PIN   GPIO_PIN_1
*/
// --- SX127x Registers ---
#define REG_FIFO            0x00
#define REG_OP_MODE         0x01
#define REG_FRF_MSB         0x06
#define REG_FRF_MID         0x07
#define REG_FRF_LSB         0x08
#define REG_PA_CONFIG       0x09
#define REG_LNA             0x0C
#define REG_FIFO_ADDR_PTR   0x0D
#define REG_FIFO_RX_BASE    0x0F
#define REG_FIFO_RX_CURRENT 0x10
#define REG_IRQ_FLAGS       0x12
#define REG_RX_NB_BYTES     0x13
#define REG_MODEM_CONFIG1   0x1D
#define REG_MODEM_CONFIG2   0x20
#define REG_PAYLOAD_LENGTH  0x22
#define REG_RSSI_VALUE      0x2E
#define REG_DIO_MAPPING_1   0x40
#define REG_VERSION         0x42

// --- Modes ---
#define MODE_SLEEP          0x00
#define MODE_STDBY          0x01
#define MODE_TX             0x03
#define MODE_RX_CONTINUOUS  0x05

// --- IRQ Flags ---
#define SX127X_IRQ_RX_DONE         0x40
#define SX127X_IRQ_TX_DONE         0x08
#define SX127X_IRQ_CLEAR_ALL       0xFF

// --- LoRa Config ---
#define LORA                0x80

// --- Callback Typedef ---
typedef void (*sx127x_rx_callback_t)(uint8_t *data, uint8_t size, int16_t rssi);

// --- Functions ---
uint32_t sx127x_init(void);
void sx127x_reset(void);
void sx127x_set_mode(uint8_t mode);
void sx127x_set_frequency(uint32_t freq);
void sx127x_set_tx_power(int8_t power);
void sx127x_setup_lora(void);
void sx127x_transmit(uint8_t *data, uint8_t size);
void sx127x_set_rx_callback(sx127x_rx_callback_t callback);
void sx127x_start_receive(void);  // Enable RX with interrupt

// --- Called from EXTI ISR ---
void sx127x_handle_dio0_irq(uint16_t GPIO_Pin);


#endif /* DRIVERS_SPI_LORA_SX127X_H_ */
