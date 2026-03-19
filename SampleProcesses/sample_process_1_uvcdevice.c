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
 * sample_process_1_uvcdevice.c
 *
 *  Created on: Mar 13, 2026
 *      Author: fil
 */
#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_UVCDEVICE

extern	JPEG_HandleTypeDef hjpeg;
extern	void GenerateColorBars(uint16_t *rgb565_buf,uint16_t *yuv_buf, uint16_t width, uint16_t height);

/* --- Image Configuration (320x200) --- */
#define IMG_WIDTH       320
#define IMG_HEIGHT      240

/* Buffer Sizes (YCbCr 4:2:2 = 2 bytes per pixel) */
#define INPUT_SIZE      (IMG_WIDTH * IMG_HEIGHT * 2)
#define OUTPUT_SIZE     (IMG_WIDTH * IMG_HEIGHT * 2)

/* External Handles */
extern JPEG_HandleTypeDef hjpeg;
extern DMA2D_HandleTypeDef hdma2d;

uint8_t __attribute__((aligned(32))) rgb565_buf[INPUT_SIZE] __attribute__((section(".sram1")));
uint8_t __attribute__((aligned(32))) yuv_buf[OUTPUT_SIZE] __attribute__((section(".sram1")));
uint8_t __attribute__((aligned(32))) rgb_buffer[INPUT_SIZE] __attribute__((section(".sram1")));


#define	USB_BUF_LEN	1024
uint8_t	usb_rx_buffer[USB_BUF_LEN];
uint8_t	usb_tx_buffer[USB_BUF_LEN];

USB_Drv_TypeDef	USB_Drv =
{
		.data = usb_rx_buffer,
		.data_index = 0,
		.requested_len = USB_BUF_LEN,
		.usb_interface_class = USB_UVC_CLASS,
		.timeout = 250,
		.wakeup_id = WAKEUP_FROM_USB_DEVICE_IRQ,
};

void sample_process_1_init(uint32_t process_id)
{
	GenerateColorBars((uint16_t *)rgb565_buf,(uint16_t *)yuv_buf, IMG_WIDTH, IMG_HEIGHT);
	VIDEO_Itf_SetPtr(yuv_buf, INPUT_SIZE);
	usb_device_driver_register(&USB_Drv);
}

void sample_process_1_uvcdevice(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint8_t	stream_started = 0;
	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	while(1)
	{
		wait_event(EVENT_TIMER);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			if ( stream_started == 0 )
			{
				stream_started = 1;
				VIDEO_Itf_StartStreaming();
			}

			process_led();
		}
	}
}

#endif // #ifdef SAMPLEPROCESS_1_UVCDEVICE
#endif // #ifdef SAMPLE_PROCESSES_ENABLED

