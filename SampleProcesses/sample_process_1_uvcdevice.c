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

#define TEST_PATTERN_WIDTH  320
#define TEST_PATTERN_HEIGHT 240
#define TEST_PATTERN_SIZE   (TEST_PATTERN_WIDTH * TEST_PATTERN_HEIGHT * 2)  /* RGB565 */

__ALIGN_BEGIN static uint8_t jpeg_buffer[TEST_PATTERN_WIDTH * TEST_PATTERN_HEIGHT * 2] __ALIGN_END;  /* Compressed output */
__ALIGN_BEGIN static uint8_t rgb_buffer[TEST_PATTERN_WIDTH * TEST_PATTERN_HEIGHT * 3] __ALIGN_END;   /* RGB input */

static void Generate_ColorBars_RGB(uint8_t *p)
{
    uint32_t x, y;
    uint32_t bar_width = TEST_PATTERN_WIDTH / 8;

    /* RGB888 color bars */
    static const uint8_t colors[8][3] = {
        {255, 255, 255},  /* White */
        {255, 255, 0},    /* Yellow */
        {0, 255, 255},    /* Cyan */
        {0, 255, 0},      /* Green */
        {255, 0, 255},    /* Magenta */
        {255, 0, 0},      /* Red */
        {0, 0, 255},      /* Blue */
        {0, 0, 0}         /* Black */
    };

    for (y = 0; y < TEST_PATTERN_HEIGHT; y++) {
        for (x = 0; x < TEST_PATTERN_WIDTH; x++) {
            uint32_t bar = x / bar_width;
            *p++ = colors[bar][0];  /* R */
            *p++ = colors[bar][1];  /* G */
            *p++ = colors[bar][2];  /* B */
        }
    }
}

static void Generate_MJPEG_Frame(uint8_t *p)
{
#ifdef NOTNOW
    Generate_ColorBars_RGB(p);
    HAL_JPEG_Encode(&hjpeg, input_buffer, TEST_PATTERN_SIZE, output_buffer, timeout);
    /* Use TinyJPEG or similar library */
    /* jpeg_size = tjpg_encode(rgb_buffer, 320, 240, jpeg_buffer, 80); */
    /* jpeg_data = jpeg_buffer; */

    /* For now, use a placeholder */
    jpeg_data = jpeg_buffer;
    jpeg_size = 1;  /* Replace with actual encoded size */
#endif
}


__ALIGN_BEGIN uint8_t test_pattern_buffer[TEST_PATTERN_SIZE] __ALIGN_END;
uint32_t test_frame_count = 0;
uint8_t use_test_pattern = 1;  /* Set to 1 for test pattern, 0 for camera */

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
	Generate_MJPEG_Frame(rgb_buffer);
	VIDEO_Itf_SetPtr(rgb_buffer, TEST_PATTERN_SIZE);
	usb_device_driver_register(&USB_Drv);
}

void sample_process_1_uvcdevice(uint32_t process_id)
{
uint32_t	wakeup,flags;
	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	while(1)
	{
		wait_event(EVENT_TIMER);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			process_led();
		}
	}
}

#endif // #ifdef SAMPLEPROCESS_1_UVCDEVICE
#endif // #ifdef SAMPLE_PROCESSES_ENABLED

