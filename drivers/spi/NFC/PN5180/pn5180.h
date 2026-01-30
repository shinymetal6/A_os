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
 * pn5180.h
 *
 *  Created on: Jan 12, 2026
 *      Author: fil
 */

#ifndef DRIVERS_SPI_NFC_PN5180_PN5180_H_
#define DRIVERS_SPI_NFC_PN5180_PN5180_H_

#define	PN5180_PAYLOAD_LENGTH					508
#define PN5180_SYSTEM_CONFIG_MFC_CRYPTO_ON_MASK	0x40000000
#define	PN5180_SEND_RECEIVE_DELAY	3

extern	uint32_t pn5180_ISO14443_init(uint32_t *spi_nfc_driver32);
extern	uint32_t pn5180_readEEprom(uint32_t *spi_nfc_driver32,uint32_t addr, uint8_t *buffer, uint32_t len);
extern	uint32_t pn5180_reset(uint32_t *spi_nfc_driver32);
extern	uint8_t  pn5180_send_ISO14443_REQA(uint32_t *spi_nfc_driver32);
extern	uint8_t  pn5180_send_ISO14443_AntiCollision(uint32_t *spi_nfc_driver32);
extern	uint8_t  pn5180_ISO14443_Authenticate(uint32_t *spi_nfc_driver32,uint8_t *Key,uint8_t KeyType, uint8_t BlockNo);

extern	uint32_t pn5180_set_rf_on(uint32_t *spi_nfc_driver32);
extern	uint32_t pn5180_set_rf_off(uint32_t *spi_nfc_driver32);

/*
 *
 * SPI2_MOSI	PB15	CN10-26
 * SPI2_MISO	PB14	CN10-28
 * SPI2_SCK		PB13	CN10-30
 * PN5180_SS	PB12	CN10-16
 * PN5180_BUSY	PB11	CN10-18
 * PN5180_IRQ	PB1		CN10-24
 * PN5180_RESET	PB2		CN10-22
 */
#endif /* DRIVERS_SPI_NFC_PN5180_PN5180_H_ */
