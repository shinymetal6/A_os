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
uint8_t *jpeg_data;
volatile uint32_t jpeg_size;

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
void VIDEO_Itf_SetPtr(uint8_t *jpegdata_ptr, uint32_t jpeg_len)
{
    __disable_irq();
    jpeg_data = jpegdata_ptr;
    jpeg_size = jpeg_len;              /* Store actual JPEG size */
    __enable_irq();
}

static int8_t VIDEO_Itf_Data(uint8_t** pbuf, uint16_t* psize, uint16_t* pcktidx)
{
    if (jpeg_data == NULL || jpeg_size == 0) {
        *pbuf = NULL;
        *psize = 0;
        return USBD_OK;
    }

    const uint16_t PAYLOAD_SIZE = UVC_ISO_FS_MPS - 2;  /* 510 bytes for FS */
    const uint16_t packet_count = (jpeg_size + PAYLOAD_SIZE - 1) / PAYLOAD_SIZE;

    if (*pcktidx < packet_count) {
        uint8_t *src = jpeg_data + (*pcktidx) * PAYLOAD_SIZE;
        uint16_t remaining = jpeg_size - (*pcktidx) * PAYLOAD_SIZE;
        uint16_t data_len = (remaining > PAYLOAD_SIZE) ? PAYLOAD_SIZE : remaining;

        *pbuf = src;
        *psize = data_len + 2;  /* +2 for UVC header */
    } else {
        *pbuf = NULL;
        *psize = 0;
    }

    (*pcktidx)++;
    return USBD_OK;
}

#endif // #ifdef	USB_DEVICE_ENABLED

#endif // #ifdef	STM32H743xx
