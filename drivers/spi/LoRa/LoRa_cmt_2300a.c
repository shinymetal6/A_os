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
 * LoRa_cmt_2300a.c
 *
 *  Created on: Aug 5, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"

#ifdef A_OS_SPI_ENABLED
#ifdef LORA_ENABLED

#include "LoRa.h"
#include "LoRa_cmt_2300a.h"
#include <string.h>

extern	LORA_Drv_TypeDef			*lora_Drv;

void LoRa_cmt2300a_CSLow(void)
{
	HAL_GPIO_WritePin(lora_Drv->CS_port, (int)lora_Drv->CS_bit, GPIO_PIN_RESET);
}

void LoRa_cmt2300a_CSHigh(void)
{
	HAL_GPIO_WritePin(lora_Drv->CS_port, (int)lora_Drv->CS_bit, GPIO_PIN_SET);
}

static uint8_t LoRa_cmt2300a_SPI_Xfer(uint8_t byte) {
    uint8_t rx;
    HAL_SPI_TransmitReceive(lora_Drv->spi, &byte, &rx, 1, HAL_MAX_DELAY);
    return rx;
}

// Register access
void LoRa_cmt2300a_WriteReg(uint8_t reg, uint8_t value) {
    LoRa_cmt2300a_CSLow();
    LoRa_cmt2300a_SPI_Xfer(CMT2300A_CMD_WRITE_REG | (reg & 0x3F));
    LoRa_cmt2300a_SPI_Xfer(value);
    LoRa_cmt2300a_CSHigh();
}

uint8_t LoRa_cmt2300a_ReadReg(uint8_t reg) {
    LoRa_cmt2300a_CSLow();
    LoRa_cmt2300a_SPI_Xfer(CMT2300A_CMD_READ_REG | (reg & 0x7F));
    uint8_t val = LoRa_cmt2300a_SPI_Xfer(CMT2300A_CMD_NOP);
    LoRa_cmt2300a_CSHigh();
    return val;
}

void LoRa_cmt2300a_WriteFIFO(uint8_t *data, uint8_t len) {
    LoRa_cmt2300a_CSLow();
    LoRa_cmt2300a_SPI_Xfer(CMT2300A_CMD_WRITE_FIFO);
    for (int i = 0; i < len; i++) {
        LoRa_cmt2300a_SPI_Xfer(data[i]);
    }
    LoRa_cmt2300a_CSHigh();
}

void LoRa_cmt2300a_ReadFIFO(uint8_t *data, uint8_t len) {
    LoRa_cmt2300a_CSLow();
    LoRa_cmt2300a_SPI_Xfer(CMT2300A_CMD_READ_FIFO);
    for (int i = 0; i < len; i++) {
        data[i] = LoRa_cmt2300a_SPI_Xfer(CMT2300A_CMD_NOP);
    }
    LoRa_cmt2300a_CSHigh();
}

void LoRa_cmt2300a_Reset(void) {
    LoRa_cmt2300a_CSLow();
    LoRa_cmt2300a_SPI_Xfer(CMT2300A_CMD_RESET);
    LoRa_cmt2300a_CSHigh();
    HAL_Delay(10);
}

// --- Configuration Functions ---

void LoRa_cmt2300a_SetFrequency(uint32_t freq_hz) {
    uint32_t freq = (freq_hz * 65536UL) / 26000000UL;  // 26 MHz XTAL
    LoRa_cmt2300a_WriteReg(REG_FREQ_10, (freq >> 16) & 0xFF);
    LoRa_cmt2300a_WriteReg(REG_FREQ_11, (freq >> 8) & 0xFF);
    LoRa_cmt2300a_WriteReg(REG_FREQ_12, freq & 0xFF);
}

void LoRa_cmt2300a_SetOutputPower(uint8_t power) {
    power = (power > 15) ? 15 : power;
    LoRa_cmt2300a_WriteReg(0x13, power);  // Output power register
}

void LoRa_cmt2300a_SetSyncWord(uint32_t syncWord) {
    LoRa_cmt2300a_WriteReg(REG_SYNC_WORD3, (syncWord >> 24) & 0xFF);
    LoRa_cmt2300a_WriteReg(REG_SYNC_WORD2, (syncWord >> 16) & 0xFF);
    LoRa_cmt2300a_WriteReg(REG_SYNC_WORD1, (syncWord >> 8) & 0xFF);
    LoRa_cmt2300a_WriteReg(REG_SYNC_WORD0, syncWord & 0xFF);
}

// --- Callbacks ---
static LoRa_cmt2300a_RxCallback rx_callback = NULL;
static LoRa_cmt2300a_TxCallback tx_callback = NULL;

void LoRa_cmt2300a_SetCallbacks(LoRa_cmt2300a_RxCallback rx_cb, LoRa_cmt2300a_TxCallback tx_cb) {
    rx_callback = rx_cb;
    tx_callback = tx_cb;
}

// --- Interrupt Handler (called from EXTI) ---
void LoRa_cmt2300a_EXTI_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(CMT2300A_GDO0_PIN);
}

void LoRa_cmt2300a_ProcessInterrupt(void) {
    uint8_t state = LoRa_cmt2300a_ReadReg(REG_STATE);

    if (state == MODE_RX) {
        // Packet received
        uint8_t len = LoRa_cmt2300a_ReadReg(0x1B) & 0x7F;  // Assume length field
        if (len > 0 && len <= 64) {
            uint8_t data[64];
            int8_t rssi = -LoRa_cmt2300a_ReadReg(REG_RSSI);  // Approximate

            LoRa_cmt2300a_ReadFIFO(data, len);
            LoRa_cmt2300a_WriteReg(REG_STATE, MODE_STANDBY);  // Clear

            if (rx_callback) {
                rx_callback(data, len, rssi);
            }
        }
        // Re-enable RX
        LoRa_cmt2300a_Receive();
    }
    else if (state == MODE_TX) {
        // TX done
        LoRa_cmt2300a_WriteReg(REG_STATE, MODE_STANDBY);
        if (tx_callback) {
            tx_callback();
        }
        // Return to RX mode
        LoRa_cmt2300a_Receive();
    }
}

// --- Main Functions ---

void LoRa_cmt2300a_Init(void) {
    // Initialize CS and GDO0 as GPIO
	LoRa_cmt2300a_CSHigh();

    LoRa_cmt2300a_Reset();

    // Frequency: 868 MHz
    LoRa_cmt2300a_SetFrequency(868000000);

    // Data rate ~10 kbps
    LoRa_cmt2300a_WriteReg(0x0A, 0x30);
    LoRa_cmt2300a_WriteReg(0x0B, 0x20);

    // Fdev = 20 kHz
    LoRa_cmt2300a_WriteReg(REG_FREQ_DEV, 0x20);

    // Sync word: 0x2DD4158A (random but unique)
    LoRa_cmt2300a_SetSyncWord(0x2DD4158A);
    LoRa_cmt2300a_WriteReg(REG_SYNC_CFG, 0x04);  // 32-bit sync, auto-clear

    // Packet config: Enable CRC, length field, sync
    LoRa_cmt2300a_WriteReg(REG_PKT_CFG,
        PKT_CFG_CRC_EN | PKT_CFG_SYNC_EN | PKT_CFG_LEN_EN);

    // FIFO threshold
    LoRa_cmt2300a_WriteReg(REG_FIFO_THRESH, 0x08);

    // Output power
    LoRa_cmt2300a_SetOutputPower(15);

    // Enter standby
    LoRa_cmt2300a_WriteReg(REG_STATE, MODE_STANDBY);

    // Start listening
    LoRa_cmt2300a_Receive();
}

void LoRa_cmt2300a_Transmit(uint8_t *data, uint8_t len) {
    if (len == 0 || len > 64) return;

    // Enter standby before TX
    LoRa_cmt2300a_WriteReg(REG_STATE, MODE_STANDBY);
    HAL_Delay(1);

    // Clear FIFO
    LoRa_cmt2300a_WriteReg(REG_FIFO_THRESH, 0x01);
    LoRa_cmt2300a_WriteReg(REG_FIFO_THRESH, 0x00);

    // Write packet: [length][data]
    uint8_t packet[65];
    packet[0] = len;
    memcpy(packet + 1, data, len);

    LoRa_cmt2300a_WriteFIFO(packet, len + 1);

    // Set mode to TX
    LoRa_cmt2300a_WriteReg(REG_STATE, MODE_TX);
}

void LoRa_cmt2300a_Receive(void) {
    LoRa_cmt2300a_WriteReg(REG_STATE, MODE_STANDBY);
    HAL_Delay(1);
    LoRa_cmt2300a_WriteReg(REG_STATE, MODE_RX);
}

#endif // #ifdef LORA_ENABLED
#endif // #ifdef A_OS_SPI_ENABLED

