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
 * iso14443.h
 *
 *  Created on: Jan 28, 2026
 *      Author: fil
 */

#ifndef DRIVERS_SPI_NFC_ISO14443_H_
#define DRIVERS_SPI_NFC_ISO14443_H_

#define NFC_CMD_AUTHA_ISO14443		0x60U    /**< MIFARE Classic Authenticate A command byte */
#define NFC_CMD_AUTHB_ISO14443		0x61U    /**< MIFARE Classic Authenticate A command byte */
#define NFC_CMD_READ_ISO14443		0x30U    /**< MIFARE Classic Read command byte */
#define NFC_ISO14443_UID4			0x04
#define NFC_ISO14443_UID7			0x07

extern	uint8_t ISO14443_Discovery(SPI_NFC_DriverStruct_t *spi_nfc_Drv);
extern	uint8_t ISO14443_Authenticate(SPI_NFC_DriverStruct_t *spi_nfc_Drv, uint8_t *Key,uint8_t KeyType, uint8_t BlockNo);
extern	uint8_t ISO14443_BlockRead(SPI_NFC_DriverStruct_t *spi_nfc_Drv,  uint8_t BlockNo,uint8_t *Block);
extern	uint8_t ISO14443_BlockWrite(SPI_NFC_DriverStruct_t *spi_nfc_Drv, uint8_t BlockNo,uint8_t *Block);


#endif /* DRIVERS_SPI_NFC_ISO14443_H_ */
