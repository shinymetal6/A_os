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
 * usbd_video_if.c
 *
 *  Created on: Mar 2, 2026
 *      Author: fil
 */

#include "main.h"
#ifdef	STM32H743xx
#include "../../../../kernel/A.h"
#include "../../../../kernel/A_exported_functions.h"
#ifdef	USB_DEVICE_ENABLED

#include "usbd_video_if.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

volatile uint8_t *usb_tx_buff;
volatile uint32_t usb_tx_size;
volatile uint32_t usb_tx_complete;
volatile uint32_t camera_buf_size;
volatile uint32_t camera_format;
volatile uint32_t camera_res;
volatile uint32_t camera_res_width;
volatile uint32_t camera_res_height;

/* Private function prototypes ---------------------------------------------- */
static int8_t VIDEO_Itf_Init(void);
static int8_t VIDEO_Itf_DeInit(void);
static int8_t VIDEO_Itf_Start(void);
static int8_t VIDEO_Itf_Stop(void);
static int8_t VIDEO_Itf_Control(USBD_VideoControlTypeDef *pctrl);
static int8_t VIDEO_Itf_Data(uint8_t** pbuf, uint16_t* psize, uint16_t* pcktidx);

USBD_VIDEO_ItfTypeDef USBD_VIDEO_Interface_fops_FS =
{
  VIDEO_Itf_Init,
  VIDEO_Itf_DeInit,
  VIDEO_Itf_Start,
  VIDEO_Itf_Stop,
  VIDEO_Itf_Control,
  VIDEO_Itf_Data,
};

static int8_t VIDEO_Itf_Init(void)
{
  return (0);
}

static int8_t VIDEO_Itf_DeInit(void)
{
  return (0);
}

static int8_t VIDEO_Itf_Start(void)
{
  return (0);
}


static int8_t VIDEO_Itf_Stop(void)
{
  return (0);
}


static int8_t VIDEO_Itf_Control(USBD_VideoControlTypeDef *pctrl)
{
  return (0);
}

/**
  * @brief  Manage the UVC data packets
  *
  * @param  pbuf: pointer to the buffer data to be filled
  * @param  psize: pointer to the current packet size to be filled
  * @param  pcktidx: pointer to the current packet index in the current image
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
uint8_t packet[UVC_ISO_DATA_PACKET_SIZE]={0x00};
uint8_t *jpeg_data;


static int8_t VIDEO_Itf_Data(uint8_t** pbuf, uint16_t* psize, uint16_t* pcktidx)
{
USBD_VIDEO_HandleTypeDef *hVIDEO =  (USBD_VIDEO_HandleTypeDef *)hUsbDeviceFS.pClassDataCmsit[hUsbDeviceFS.classId];

	if (hVIDEO->uvc_state != UVC_PLAY_STATUS_STREAMING)
	{
		return USBD_OK;
	}

static uint8_t payload_header[2] = {0x02, 0x00};
static uint8_t packet_index = 0;

const uint16_t data_payload_size = UVC_ISO_DATA_PACKET_SIZE - 2;
const uint16_t packet_count = JPEG_SIZE / data_payload_size;
const uint8_t packet_remainder = JPEG_SIZE % data_payload_size;

uint16_t packet_size = 2;  // Default to header only
uint8_t *src = NULL;

	if (packet_index == 0)
	{
		// Toggle Frame ID (bit 0) on new frame
		payload_header[1] ^= 0x01;
	}

	if (packet_index <= packet_count)
	{
		src = jpeg_data + packet_index * data_payload_size;

		uint16_t data_len = (packet_index < packet_count) ?	data_payload_size :	packet_remainder;

		memcpy(packet + 2, src, data_len);
		packet_size = data_len + 2;
	}

	// Write payload header
	packet[0] = payload_header[0];
	packet[1] = payload_header[1];

	// Transmit packet
	USBD_LL_Transmit(&hUsbDeviceFS, 0x81, packet, packet_size);

	// Update for next packet
	packet_index++;
	if (packet_index > packet_count)
	{
		packet_index = 0; // End of current frame
	}
  return (0);
}

void VIDEO_Itf_SetPtr(uint8_t *jpegdata_ptr)
{
	jpeg_data = jpegdata_ptr;
}
#endif // #ifdef	USB_DEVICE_ENABLED

#endif // #ifdef	STM32H743xx
