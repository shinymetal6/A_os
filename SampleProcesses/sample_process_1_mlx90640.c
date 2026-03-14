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
 * sample_process_1_mlx90640.c
 *
 *  Created on: Feb 25, 2026
 *      Author: fil
 */

#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_MLX90640

uint16_t			mlx90640_eeprom[MLX90640_EEPROM_DUMP_NUM];
uint16_t 			frame_data[MLX90640_PIXEL_NUM];
uint16_t 			aux_data[MLX90640_AUX_NUM];
float	 			to_data[MLX90640_PIXEL_NUM];
uint16_t 			resulting_image[MLX90640_PIXEL_NUM];
uint16_t 			alpha[MLX90640_PIXEL_NUM];
int16_t 			offset[MLX90640_PIXEL_NUM];
int8_t 				kta[MLX90640_PIXEL_NUM];
int8_t 				kv[MLX90640_PIXEL_NUM];
float 				alphaTemp[MLX90640_PIXEL_NUM];

uint8_t 			jpeg_image[MLX90640_PIXEL_NUM];
uint16_t 			jpeg_image_size;


extern				I2C_HandleTypeDef hi2c1;
extern				JPEG_HandleTypeDef hjpeg;

I2C_Mlx90640_Drv_TypeDef	I2C_Mlx90640_Drv =
{
		.wakeup_id = 1,
		.bus = &hi2c1,
		.device_address = MLX90640_ADDRESS<<1,
		.mlx90640_eeprom = mlx90640_eeprom,
		.frame_data = frame_data,
		.aux_data = aux_data,
		.to_data = to_data,
		.resulting_image = resulting_image,
		.alpha = alpha,
		.offset = offset,
		.kta = kta,
		.kv = kv,
		.alphaTemp = alphaTemp,
};


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

HAL_StatusTypeDef I2C_Recover_Bus(I2C_HandleTypeDef* hi2c)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_TypeDef* SCL_Port = GPIOB;  // Adjust per your setup
    GPIO_TypeDef* SDA_Port = GPIOB;
    uint16_t SCL_Pin = GPIO_PIN_8;
    uint16_t SDA_Pin = GPIO_PIN_9;

    // DeInit I2C
    HAL_I2C_DeInit(hi2c);

    // Configure as GPIO
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Pin = SCL_Pin | SDA_Pin;
    HAL_GPIO_Init(SCL_Port, &GPIO_InitStruct);
    HAL_GPIO_Init(SDA_Port, &GPIO_InitStruct);

    // Ensure both high
    HAL_GPIO_WritePin(SCL_Port, SCL_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SDA_Port, SDA_Pin, GPIO_PIN_SET);
    HAL_Delay(1);

    // If SDA still low, clock it out
    //if (HAL_GPIO_ReadPin(SDA_Port, SDA_Pin) == GPIO_PIN_RESET)
    {
        for (int i = 0; i < 9; i++) {
            HAL_GPIO_WritePin(SCL_Port, SCL_Pin, GPIO_PIN_SET);
            HAL_Delay(1);
            HAL_GPIO_WritePin(SCL_Port, SCL_Pin, GPIO_PIN_RESET);
            HAL_Delay(1);
        }

        // STOP condition
        HAL_GPIO_WritePin(SDA_Port, SDA_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(SCL_Port, SCL_Pin, GPIO_PIN_SET);
        HAL_Delay(1);
        HAL_GPIO_WritePin(SDA_Port, SDA_Pin, GPIO_PIN_SET);
        HAL_Delay(1);
    }

    // Reconfigure for I2C
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(SCL_Port, &GPIO_InitStruct);
    HAL_GPIO_Init(SDA_Port, &GPIO_InitStruct);

    // Reinitialize I2C
    return HAL_I2C_Init(hi2c);
}

void sample_process_1_init(uint32_t process_id)
{
	I2C_Recover_Bus(&hi2c1);
	if ( mlx90640_register(&I2C_Mlx90640_Drv) )
#ifdef	STM32H743xx
		  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
#else
	return;
#endif
	usb_device_driver_register(&USB_Drv);

}
#include "../../A_os/drivers/USB_Device/STM32H743/App/usbd_video_if.h"

volatile uint32_t camera_capture_complete = 0;

void sample_process_1_mlx90640(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint32_t	count=0;
	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	while(1)
	{
		wait_event(EVENT_TIMER);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			process_led();
			count++;
			if ( count == 10)
			{
#ifdef	STM32H743xx
				HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
#endif
				count = 0;
				if  (I2C_Mlx90640_Drv.mlx90640_found )
				{
					mlx90640_run(&I2C_Mlx90640_Drv,1.0,1.0);
				}
			}
		}
	}
}

#endif // #ifdef SAMPLEPROCESS_1_MLX90640
#endif // #ifdef SAMPLE_PROCESSES_ENABLED
