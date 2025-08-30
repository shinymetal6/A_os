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
 * LoRa_cmt_2300a.h
 *
 *  Created on: Aug 5, 2025
 *      Author: fil
 */

#ifndef DRIVERS_SPI_LORA_LORA_CMT_2300A_H_
#define DRIVERS_SPI_LORA_LORA_CMT_2300A_H_
#ifdef LORA_ENABLED

// GPIO
#define CMT2300A_GDO0_PORT  LORA1_IRQPD1_GPIO_Port
#define CMT2300A_GDO0_PIN   LORA1_IRQPD1_Pin

// Commands
#define CMT2300A_CMD_WRITE_REG    0x00
#define CMT2300A_CMD_READ_REG     0x80
#define CMT2300A_CMD_WRITE_FIFO   0x40
#define CMT2300A_CMD_READ_FIFO    0xC0
#define CMT2300A_CMD_RESET        0x87
#define CMT2300A_CMD_NOP          0x00

// Registers
#define REG_MODEM_CONFIG1     0x09
#define REG_MODEM_CONFIG2     0x0A
#define REG_MODEM_CONFIG3     0x0B
#define REG_FREQ_10           0x10
#define REG_FREQ_11           0x11
#define REG_FREQ_12           0x12
#define REG_FREQ_DEV          0x0D
#define REG_SYNC_CFG          0x16
#define REG_SYNC_WORD3        0x17
#define REG_SYNC_WORD2        0x18
#define REG_SYNC_WORD1        0x19
#define REG_SYNC_WORD0        0x1A
#define REG_PKT_CFG           0x1B
#define REG_FIFO_THRESH       0x1F
#define REG_RSSI              0x1E
#define REG_STATE             0x14

// Packet config bits
#define PKT_CFG_CRC_EN        (1 << 3)
#define PKT_CFG_SYNC_EN       (1 << 2)
#define PKT_CFG_LEN_EN        (1 << 1)  // Length field in packet
#define PKT_CFG_AUTO_ACK      (1 << 0)

// Mode states
#define MODE_STANDBY          0x01
#define MODE_TX               0x03
#define MODE_RX               0x05

// Callback typedefs
typedef void (*LoRa_cmt2300a_RxCallback)(uint8_t *data, uint8_t len, int8_t rssi);
typedef void (*LoRa_cmt2300a_TxCallback)(void);

// Public functions
void LoRa_cmt2300a_Init(void);
void LoRa_cmt2300a_SetFrequency(uint32_t freq_hz);
void LoRa_cmt2300a_SetOutputPower(uint8_t power);
void LoRa_cmt2300a_Transmit(uint8_t *data, uint8_t len);
void LoRa_cmt2300a_Receive(void);
void LoRa_cmt2300a_SetSyncWord(uint32_t syncWord);  // 32-bit sync
void LoRa_cmt2300a_SetCallbacks(LoRa_cmt2300a_RxCallback rx_cb, LoRa_cmt2300a_TxCallback tx_cb);

// Interrupt handlers (called from HAL)
void LoRa_cmt2300a_EXTI_IRQHandler(void);
void LoRa_cmt2300a_ProcessInterrupt(void);

#endif // #ifdef LORA_ENABLED

#endif /* DRIVERS_SPI_LORA_LORA_CMT_2300A_H_ */
