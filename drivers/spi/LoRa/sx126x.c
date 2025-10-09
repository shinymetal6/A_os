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
 * sx126x.c
 *
 *  Created on: Aug 5, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"

#ifdef A_OS_SPI_ENABLED
#ifdef LORA_ENABLED

#include "LoRa.h"
#include "sx126x.h"
#include <string.h>

extern	LORA_Drv_TypeDef			*lora_Drv;

static sx126x_rx_callback_t rx_callback = NULL;
static sx126x_tx_callback_t tx_callback = NULL;

void sx126x_CSLow(void)
{
	HAL_GPIO_WritePin(lora_Drv->CS_port, (int)lora_Drv->CS_bit, GPIO_PIN_RESET);
}

void sx126x_CSHigh(void)
{
	HAL_GPIO_WritePin(lora_Drv->CS_port, (int)lora_Drv->CS_bit, GPIO_PIN_SET);
}

void sx126x_reset(void)
{
	sx126x_CSHigh();
	HAL_GPIO_WritePin(lora_Drv->RESET_port,lora_Drv->RESET_bit, GPIO_PIN_RESET);
	HAL_Delay(50);
	HAL_GPIO_WritePin(lora_Drv->RESET_port, lora_Drv->RESET_bit, GPIO_PIN_SET);
	HAL_Delay(100);
}

void sx126x_wakeup(void) {
	sx126x_CSLow();
    HAL_Delay(1);
    sx126x_CSHigh();
    HAL_Delay(1);
}

void sx126x_wait_for_busy(void)
{
	while(HAL_GPIO_ReadPin(lora_Drv->RADIO_BUSY_port,lora_Drv->RADIO_BUSY_bit) == GPIO_PIN_SET);
}

void sx126x_write_command(uint8_t cmd, uint8_t *data, uint16_t size) {
    sx126x_wait_for_busy();
	sx126x_CSLow();
    HAL_SPI_Transmit(lora_Drv->spi, &cmd, 1, HAL_MAX_DELAY);
    if (size > 0 && data) {
        HAL_SPI_Transmit(lora_Drv->spi, data, size, HAL_MAX_DELAY);
    }
    sx126x_CSHigh();
}

void sx126x_read_command(uint8_t cmd, uint8_t *data, uint16_t size) {
    sx126x_wait_for_busy();
	sx126x_CSLow();
    HAL_SPI_Transmit(lora_Drv->spi, &cmd, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(lora_Drv->spi, data, size, HAL_MAX_DELAY);
    sx126x_CSHigh();
}

void sx126x_write_register(uint16_t addr, uint8_t *data, uint8_t size) {
    uint8_t buffer[3] = {0x0D, (uint8_t)(addr >> 8), (uint8_t)(addr & 0xFF)};
    sx126x_wait_for_busy();
	sx126x_CSLow();
    HAL_SPI_Transmit(lora_Drv->spi, buffer, 3, HAL_MAX_DELAY);
    HAL_SPI_Transmit(lora_Drv->spi, data, size, HAL_MAX_DELAY);
    sx126x_CSHigh();
}


void sx126x_write_single_register(uint16_t addr, uint8_t data)
{
    uint8_t buffer[3] = {0x0D, (uint8_t)(addr >> 8), (uint8_t)(addr & 0xFF)};
    sx126x_wait_for_busy();
	sx126x_CSLow();
    HAL_SPI_Transmit(lora_Drv->spi, buffer, 3, HAL_MAX_DELAY);
    HAL_SPI_Transmit(lora_Drv->spi, &data, 1, HAL_MAX_DELAY);
    sx126x_CSHigh();
}

void sx126x_read_register(uint16_t addr, uint8_t *data, uint8_t size) {
    uint8_t cmd[3] = {0x1D, (uint8_t)(addr >> 8), (uint8_t)(addr & 0xFF)};
    sx126x_wait_for_busy();
	sx126x_CSLow();
    HAL_SPI_Transmit(lora_Drv->spi, cmd, 3, HAL_MAX_DELAY);
    HAL_SPI_Receive(lora_Drv->spi, data, size, HAL_MAX_DELAY);
    sx126x_CSHigh();
}

uint8_t sx126x_read_single_register(uint16_t addr) {
    uint8_t cmd[3] = {0x1D, (uint8_t)(addr >> 8), (uint8_t)(addr & 0xFF)};
    uint8_t data;
    sx126x_wait_for_busy();
	sx126x_CSLow();
    HAL_SPI_Transmit(lora_Drv->spi, cmd, 3, HAL_MAX_DELAY);
    HAL_SPI_Receive(lora_Drv->spi, &data, 1, HAL_MAX_DELAY);
    sx126x_CSHigh();
    return data;
}

void sx126x_write_buffer(uint8_t *data, uint8_t size, uint8_t offset) {
    uint8_t buffer[2] = {0x0E, offset};
    sx126x_wait_for_busy();
	sx126x_CSLow();
    HAL_SPI_Transmit(lora_Drv->spi, buffer, 2, HAL_MAX_DELAY);
    HAL_SPI_Transmit(lora_Drv->spi, data, size, HAL_MAX_DELAY);
    sx126x_CSHigh();
}

void sx126x_read_buffer(uint8_t *data, uint8_t size, uint8_t offset) {
    uint8_t cmd[2] = {0x1E, offset};
    sx126x_wait_for_busy();
	sx126x_CSLow();
    HAL_SPI_Transmit(lora_Drv->spi, cmd, 2, HAL_MAX_DELAY);
    HAL_SPI_Receive(lora_Drv->spi, data, size, HAL_MAX_DELAY);
    sx126x_CSHigh();
}

// --- High-level Functions ---
void sx126x_set_standby(void) {
    uint8_t mode = MODE_STDBY_RC;
    sx126x_write_command(RADIO_SET_STANDBY, &mode, 1);
}

void sx126x_set_packet_type(uint8_t packetType) {
    sx126x_write_command(RADIO_SET_PACKET_TYPE, &packetType, 1);
}

uint32_t sx126x_getRSSI(void)
{
uint8_t data[2];
    sx126x_read_command(RADIO_GET_RSSI, data, 2);
    return data[1];
}

void sx126x_set_rf_frequency(uint32_t frequency) {
    uint32_t freq = (frequency * (1UL << 25)) / 32000000;  // 32MHz crystal
    uint8_t buffer[4];
    buffer[0] = (freq >> 24) & 0xFF;
    buffer[1] = (freq >> 16) & 0xFF;
    buffer[2] = (freq >> 8) & 0xFF;
    buffer[3] = freq & 0xFF;
    sx126x_write_command(RADIO_SET_RFFREQUENCY, buffer, 4);
}

void sx126x_set_tx_params(int8_t power) {
    uint8_t paConfig = 0x01;  // Use PA_LUT
    uint8_t buffer[2] = {power, paConfig};
    sx126x_write_command(RADIO_SET_TXPARAMS, buffer, 2);
}

void sx126x_set_modulation_params(uint8_t sf, uint8_t bw, uint8_t cr, uint8_t ldro) {
    uint8_t buffer[4] = {sf, bw, cr, ldro};
    sx126x_write_command(RADIO_SET_MODULATION_PARAMS, buffer, 4);
}

void sx126x_set_packet_params(uint16_t preambleLen, uint8_t headerType, uint8_t payloadLen, uint8_t crc, uint8_t iq) {
    uint8_t buffer[6];
    buffer[0] = (preambleLen >> 8) & 0xFF;
    buffer[1] = preambleLen & 0xFF;
    buffer[2] = headerType;
    buffer[3] = payloadLen;
    buffer[4] = crc;
    buffer[5] = iq;
    sx126x_write_command(RADIO_SET_PACKET_PARAMS, buffer, 6);
}

void sx126x_set_buffer_base(uint8_t txBase, uint8_t rxBase) {
    uint8_t buffer[2] = {txBase, rxBase};
    sx126x_write_command(RADIO_SET_BUFFER_BASE_ADDRESSES, buffer, 2);
}

void sx126x_set_dio_irq(uint16_t irqMask, uint16_t dio1Mask) {
    uint8_t buffer[4];
    buffer[0] = (dio1Mask >> 8) & 0xFF;
    buffer[1] = dio1Mask & 0xFF;
    buffer[2] = 0x00;  // DIO2 mapping
    buffer[3] = 0x00;  // DIO3 mapping
    sx126x_write_command(RADIO_SET_DIO_IRQ_PARAMS, buffer, 4);
}

uint16_t sx126x_get_irq_status(void) {
    uint8_t data[2];
    sx126x_read_command(RADIO_GET_IRQ_STATUS, data, 2);
    return (data[0] << 8) | data[1];
}

void sx126x_clear_irq_status(uint16_t flags) {
    uint8_t buffer[2] = {(flags >> 8) & 0xFF, flags & 0xFF};
    sx126x_write_command(RADIO_CLEAR_IRQ_STATUS, buffer, 2);
}

void sx126x_set_rx(uint32_t timeoutMs) {
    uint32_t timeoutTicks = (timeoutMs * 1000) / 15.625;  // 1 tick = 15.625 µs
    uint8_t buffer[3];
    buffer[0] = (timeoutTicks >> 16) & 0xFF;
    buffer[1] = (timeoutTicks >> 8) & 0xFF;
    buffer[2] = timeoutTicks & 0xFF;
    sx126x_write_command(RADIO_SET_RX, buffer, 3);
}

void sx126x_set_tx(uint32_t timeoutMs) {
    uint32_t timeoutTicks = (timeoutMs * 1000) / 15.625;
    uint8_t buffer[3];
    buffer[0] = (timeoutTicks >> 16) & 0xFF;
    buffer[1] = (timeoutTicks >> 8) & 0xFF;
    buffer[2] = timeoutTicks & 0xFF;
    sx126x_write_command(RADIO_SET_TX, buffer, 3);
}

// --- Initialize and Configure ---
uint32_t sx126x_init(void) {
    sx126x_reset();
    sx126x_wakeup();
    sx126x_set_standby();
    HAL_Delay(10);

    sx126x_set_packet_type(PACKET_TYPE_LORA);
    sx126x_set_rf_frequency(433000000);  // Change to 868000000 for EU
    sx126x_set_tx_params(22);            // 22 dBm
    sx126x_set_modulation_params(0x0A, 0x01, 0x01, 0x00);  // SF10, BW125, CR4/5, LDRO=Auto
    sx126x_set_packet_params(0x0008, 0x00, 0xFF, 0x01, 0x00);  // Preamble=8, Variable, CRC
    sx126x_set_buffer_base(0x00, 0x00);

    // Enable DIO1 interrupts: RX done, TX done
    sx126x_set_dio_irq(SX126X_IRQ_ALL, SX126X_IRQ_RX_DONE | SX126X_IRQ_TX_DONE | SX126X_IRQ_TIMEOUT);

    // Start listening
    sx126x_start_receive();
    return 0;
}

// --- Transmit ---
void sx126x_transmit(uint8_t *data, uint8_t size) {
    sx126x_set_standby();
    HAL_Delay(10);
    sx126x_write_buffer(data, size, 0);
    sx126x_set_tx(0xFFFFFFFF);  // No timeout
}

// --- Receive Mode ---
void sx126x_start_receive(void) {
    sx126x_clear_irq_status(SX126X_IRQ_ALL);
    sx126x_set_rx(0xFFFFFF);  // Long timeout
}

// --- Callback Setters ---
void sx126x_set_rx_callback(sx126x_rx_callback_t cb) {
    rx_callback = cb;
}

void sx126x_set_tx_callback(sx126x_tx_callback_t cb) {
    tx_callback = cb;
}

// --- DIO1 Interrupt Handler ---
void sx126x_handle_dio1_irq(uint16_t GPIO_Pin) {
    uint16_t irq = sx126x_get_irq_status();
    sx126x_clear_irq_status(irq);

    if (irq & SX126X_IRQ_RX_DONE) {
        uint8_t size;
        uint8_t rxInfo[3];
        // Get packet info: [0]=size, [1]=rxAddr, [2]=rssiAvg
        sx126x_read_command(0x1D, rxInfo, 3);  // Get Rx packet status
        size = rxInfo[0];

        if (rx_callback && size > 0 && size <= 255) {
            uint8_t data[255];
            sx126x_read_buffer(data, size, 0);
            int16_t rssi = -rxInfo[2];  // Approximate
            rx_callback(data, size, rssi);
        }
    }

    if (irq & SX126X_IRQ_TX_DONE) {
        if (tx_callback) {
            tx_callback();
        }
    }

    if (irq & SX126X_IRQ_TIMEOUT) {
        // Optional: handle timeout
    }

    // Always re-enable RX after TX or RX
    sx126x_start_receive();
}
#endif // #ifdef LORA_ENABLED
#endif // #ifdef A_OS_SPI_ENABLED


