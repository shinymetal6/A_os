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
 * usbd_midi_if.c
 *
 *  Created on: Jan 24, 2024
 *      Author: fil
 */


#include "main.h"
#ifdef	STM32H743xx
#include "../../../../kernel/A.h"
#include "../../../../kernel/A_exported_functions.h"
#ifdef	USB_DEVICE_ENABLED

#include "usbd_midi_if.h"

uint8_t UserMIDIRxBufferFS[APP_MIDI_RX_DATA_SIZE];
uint8_t UserMIDITxBufferFS[APP_MIDI_TX_DATA_SIZE];

extern USBD_HandleTypeDef hUsbDeviceFS;

static int8_t MIDI_Init_FS(void);
static int8_t MIDI_DeInit_FS(void);
static int8_t MIDI_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t MIDI_Receive_FS(uint8_t* pbuf, uint32_t *Len);
static int8_t MIDI_TransmitCplt_FS(uint8_t *pbuf, uint32_t *Len, uint8_t epnum);

USBD_MIDI_ItfTypeDef USBD_MIDI_Interface_fops_FS =
{
  MIDI_Init_FS,
  MIDI_DeInit_FS,
  MIDI_Control_FS,
  MIDI_Receive_FS,
  MIDI_TransmitCplt_FS
};

static int8_t MIDI_Init_FS(void)
{
  USBD_MIDI_SetTxBuffer(&hUsbDeviceFS, UserMIDITxBufferFS, 0);
  USBD_MIDI_SetRxBuffer(&hUsbDeviceFS, UserMIDIRxBufferFS);
  return (USBD_OK);
}

static int8_t MIDI_DeInit_FS(void)
{
  return (USBD_OK);
}

static int8_t MIDI_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
	return (USBD_OK);
}

void	(*MidiRx_CallbackPtr)(uint8_t* buf, uint16_t len);

static int8_t MIDI_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
uint16_t	len = *Len;
	if ( MidiRx_CallbackPtr != NULL )
		MidiRx_CallbackPtr(Buf,len);
	USBD_MIDI_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
	USBD_MIDI_ReceivePacket(&hUsbDeviceFS);
	return (USBD_OK);
}

uint8_t MIDI_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
  uint8_t result = USBD_OK;
  USBD_MIDI_HandleTypeDef *hcdc = (USBD_MIDI_HandleTypeDef*)hUsbDeviceFS.pClassData;
  if (hcdc->TxState != 0){
    return USBD_BUSY;
  }
  USBD_MIDI_SetTxBuffer(&hUsbDeviceFS, Buf, Len);
  result = USBD_MIDI_TransmitPacket(&hUsbDeviceFS);
  return result;
}

static int8_t MIDI_TransmitCplt_FS(uint8_t *Buf, uint32_t *Len, uint8_t epnum)
{
  uint8_t result = USBD_OK;
  UNUSED(Buf);
  UNUSED(Len);
  UNUSED(epnum);
  return result;
}
#endif // #ifdef	USB_DEVICE_ENABLED

#endif // #ifdef	STM32H743xx

