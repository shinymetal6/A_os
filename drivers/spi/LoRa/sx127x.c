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
 * sx127x.c
 *
 *  Created on: Aug 5, 2025
 *      Author: fil
 */
#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"

#ifdef A_OS_SPI_ENABLED
#ifdef LORA_ENABLED

#include "LoRa.h"
#include "sx127x.h"
#include <string.h>

extern	LORA_Drv_TypeDef			*lora_Drv;

// Static function forward declarations
static void sx127x_read_fifo(uint8_t *data, uint8_t size);
static int16_t sx127x_get_rssi(void);

// Global callback
static sx127x_rx_callback_t rx_callback = NULL;

void sx127x_CSLow(void)
{
	HAL_GPIO_WritePin(lora_Drv->CS_port, (int)lora_Drv->CS_bit, GPIO_PIN_RESET);
}

void sx127x_CSHigh(void)
{
	HAL_GPIO_WritePin(lora_Drv->CS_port, (int)lora_Drv->CS_bit, GPIO_PIN_SET);
}

// Register read/write
uint8_t sx127x_read_register(uint8_t reg) {
    uint8_t data;
    sx127x_CSLow();
    HAL_SPI_Transmit(lora_Drv->spi, &reg, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(lora_Drv->spi, &data, 1, HAL_MAX_DELAY);
    sx127x_CSHigh();
    return data;
}
// Read FIFO
static void sx127x_read_fifo(uint8_t *data, uint8_t size) {
    for (uint8_t i = 0; i < size; i++) {
        data[i] = sx127x_read_register(REG_FIFO);
    }
}

// Get RSSI of last packet
static int16_t sx127x_get_rssi(void) {
    return (int16_t)(-137 + sx127x_read_register(REG_RSSI_VALUE));
}



void sx127x_write_register(uint8_t reg, uint8_t value) {
    uint8_t out[2] = {reg | 0x80, value};
    sx127x_CSLow();
    HAL_SPI_Transmit(lora_Drv->spi, out, 2, HAL_MAX_DELAY);
    sx127x_CSHigh();
}

// Reset
void sx127x_reset(void)
{
	sx127x_CSHigh();
	HAL_GPIO_WritePin(lora_Drv->RESET_port,lora_Drv->RESET_bit, GPIO_PIN_RESET);
	HAL_Delay(50);
	HAL_GPIO_WritePin(lora_Drv->RESET_port, lora_Drv->RESET_bit, GPIO_PIN_SET);
	HAL_Delay(100);
}

// Set mode
void sx127x_set_mode(uint8_t mode) {
    uint8_t reg = sx127x_read_register(REG_OP_MODE);
    reg = (reg & 0xF8) | mode;
    sx127x_write_register(REG_OP_MODE, reg);
}

// Set frequency (e.g. 915000000 Hz)
void sx127x_set_frequency(uint32_t freq) {
    uint64_t frf = ((uint64_t)freq << 19) / 32000000;  // Assuming 32MHz crystal
    sx127x_write_register(REG_FRF_MSB, (uint8_t)(frf >> 16));
    sx127x_write_register(REG_FRF_MID, (uint8_t)(frf >> 8));
    sx127x_write_register(REG_FRF_LSB, (uint8_t)frf);
}

// Set TX power (2–17 dBm)
void sx127x_set_tx_power(int8_t power) {
    if (power > 17) power = 17;
    if (power < 2) power = 2;
    sx127x_write_register(REG_PA_CONFIG, 0x80 | (power - 2));  // PA_BOOST
}

// Setup LoRa mode
void sx127x_setup_lora(void) {
    sx127x_set_mode(MODE_SLEEP);
    HAL_Delay(10);

    // Validate chip
    uint8_t version = sx127x_read_register(REG_VERSION);
    if (version == 0x00 || version == 0xFF) {
        // Invalid — check wiring
        return;
    }

    sx127x_write_register(REG_OP_MODE, LORA | MODE_STDBY);
    sx127x_write_register(REG_MODEM_CONFIG1, 0x72);  // BW125kHz, CR 4/5, Explicit Header
    sx127x_write_register(REG_MODEM_CONFIG2, 0x74);  // SF7, CRC enabled
    sx127x_write_register(REG_DIO_MAPPING_1, 0x00);  // DIO0 = RX Done
    sx127x_write_register(REG_IRQ_FLAGS, SX127X_IRQ_CLEAR_ALL);
}

// Initialize
uint32_t sx127x_init(void) {
    sx127x_reset();
    HAL_Delay(10);
    sx127x_setup_lora();
    sx127x_set_frequency(915000000);  // Change for EU: 868000000
    sx127x_set_tx_power(17);
    sx127x_set_rx_callback(NULL);  // No callback yet
}

// Transmit (blocking)
void sx127x_transmit(uint8_t *data, uint8_t size) {
    sx127x_set_mode(MODE_STDBY);
    while (((sx127x_read_register(REG_OP_MODE) & 0x07) != MODE_STDBY));

    // Write payload
    sx127x_write_register(REG_FIFO_ADDR_PTR, 0x00);
    for (uint8_t i = 0; i < size; i++) {
        sx127x_write_register(REG_FIFO, data[i]);
    }
    sx127x_write_register(REG_PAYLOAD_LENGTH, size);

    // Go to TX
    sx127x_set_mode(MODE_TX);

    // Wait for TX done
    while (!(sx127x_read_register(REG_IRQ_FLAGS) & SX127X_IRQ_TX_DONE)) {
        HAL_Delay(1);
    }
    sx127x_write_register(REG_IRQ_FLAGS, SX127X_IRQ_TX_DONE);

    // Return to RX
    sx127x_start_receive();
}

// Set callback function
void sx127x_set_rx_callback(sx127x_rx_callback_t callback) {
    rx_callback = callback;
}

// Start continuous receive mode
void sx127x_start_receive(void) {
    sx127x_set_mode(MODE_STDBY);
    sx127x_write_register(REG_FIFO_RX_BASE, 0x00);
    sx127x_write_register(REG_FIFO_ADDR_PTR, 0x00);
    sx127x_set_mode(MODE_RX_CONTINUOUS);
    sx127x_write_register(REG_IRQ_FLAGS, SX127X_IRQ_CLEAR_ALL);  // Clear any pending
}

// --- IRQ Handler (Call this from EXTI ISR) ---
void sx127x_handle_dio0_irq(uint16_t GPIO_Pin) {
    if (rx_callback != NULL) {
        uint8_t irq = sx127x_read_register(REG_IRQ_FLAGS);
        if (irq & SX127X_IRQ_RX_DONE) {
            uint8_t size = sx127x_read_register(REG_RX_NB_BYTES);
            uint8_t current_addr = sx127x_read_register(REG_FIFO_RX_CURRENT);
            sx127x_write_register(REG_FIFO_ADDR_PTR, current_addr);

            uint8_t data[255];
            sx127x_read_fifo(data, size);

            int16_t rssi = sx127x_get_rssi();
            sx127x_write_register(REG_IRQ_FLAGS, SX127X_IRQ_RX_DONE);

            // Call user callback
            rx_callback(data, size, rssi);
        }
    }

    // Optional: Restart RX if not called from loop
    sx127x_start_receive();
}
#endif // #ifdef LORA_ENABLED

#endif // #ifdef A_OS_SPI_ENABLED
