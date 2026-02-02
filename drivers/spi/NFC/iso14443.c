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
 * iso14443.c
 *
 *  Created on: Jan 28, 2026
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"

#ifdef A_OS_SPI_ENABLED
#ifdef NFC_ENABLED
#include "nfc.h"
#include "iso14443.h"

/* ATQA values :
		Mifare S50        : 0X0004
		Mifare S70        : 0X0002
		Mifare UltraLight : 0x0044
		Mifare Light      : 0X0010
		Mifare Desfire    : 0X0344
*/

uint8_t ISO14443_Discovery(SPI_NFC_DriverStruct_t *spi_nfc_Drv)
{
	spi_nfc_ISO14443_init(spi_nfc_Drv);
	spi_nfc_rf_on(spi_nfc_Drv);
	spi_nfc_Drv->ATQA = spi_nfc_ISO14443_send_REQA(spi_nfc_Drv);
	if ( spi_nfc_Drv->ATQA == 0 )
	{
		spi_nfc_rf_off(spi_nfc_Drv);
		return 0;
	}
	spi_nfc_send_ISO14443_AntiCollision(spi_nfc_Drv);
	return spi_nfc_Drv->UID_len;
}

uint8_t ISO14443_Authenticate(SPI_NFC_DriverStruct_t *spi_nfc_Drv, uint8_t *Key,uint8_t KeyType, uint8_t BlockNo)
{
uint8_t res = 0;
	if ( spi_nfc_Drv->UID_len == NFC_ISO14443_UID4 )
		res =  spi_ISO14443_Authenticate(spi_nfc_Drv,Key,KeyType,BlockNo);
	//spi_nfc_rf_off(spi_nfc_Drv);
	return res;
}

uint8_t ISO14443_BlockRead(SPI_NFC_DriverStruct_t *spi_nfc_Drv, uint8_t BlockNo,uint8_t *Block)
{
uint32_t res = 0;
	res = spi_ISO14443_BlockRead(spi_nfc_Drv,BlockNo,Block);
	spi_nfc_rf_off(spi_nfc_Drv);
	return res;
}


uint8_t ISO14443_BlockWrite(SPI_NFC_DriverStruct_t *spi_nfc_Drv, uint8_t BlockNo,uint8_t *Block)
{
uint32_t res = 0;
	return res;
}
#endif // #ifdef NFC_ENABLED
#endif // #ifdef A_OS_SPI_ENABLED


