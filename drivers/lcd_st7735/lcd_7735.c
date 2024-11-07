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
 * lcd_7735.c
 *
 *  Created on: Jan 3, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"
#ifdef OLD
#ifdef	LCD_096_ENABLED

#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/hwmanager.h"
#include "../../kernel/kernel_opt.h"
#include "lcd_7735.h"
#include "st7735.h"

Video_t	Video;

extern	uint16_t 		logo[];
extern	HWDevices_t		HWDevices[HWDEVICES_NUM];

uint32_t LcdSetBrightness(uint16_t brightness)
{
	if ( HWDevices[HWDEV_SPILCD].process != Asys.current_process )
		return LCD_NOT_OWNED;
	if ( brightness <= FULL_BRIGHTNESS)
	{
		BACKLIGHT_TIMER.Instance->CCR1 = brightness;
		Video.current_brightness = brightness;
	}
	return LCD_OK;
}

void LcdInit(void)
{
    LcdSetBrightness(ZERO_BRIGHTNESS);
	HAL_TIM_PWM_Start(&BACKLIGHT_TIMER,TIM_CHANNEL_1);
    ST7735_Unselect();
    HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_RESET);
	ST7735_Init();
	HWDevices[HWDEV_SPILCD].flags = 0;
	HWDevices[HWDEV_SPILCD].status = 0;
}

uint32_t LcdClearScreen(void)
{
	if ( HWDevices[HWDEV_SPILCD].process != Asys.current_process )
		return LCD_NOT_OWNED;
	ST7735_ClearScreen();
	return LCD_OK;
}


uint32_t Draw_Logo(uint16_t *logo)
{
	if ( HWDevices[HWDEV_SPILCD].process != Asys.current_process )
		return LCD_NOT_OWNED;
	ST7735_DrawImage(0,0,ST7735_WIDTH, ST7735_HEIGHT,logo);
	return LCD_OK;
}
#endif
#else
/**********************************************************************************************/
#ifdef	LCD_096_ENABLED

#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"
#include "../../kernel/kernel_opt.h"

#include "lcd_7735.h"
#include <string.h>

extern	void ST7735_SetVars(Lcd_Control_096_Drv_TypeDef	*Lcd_Control_096_Drv);

extern	DriverStruct_t	*DriverStruct[MAX_DRIVERS];
extern	uint16_t 		logo[];

static uint32_t lcd_control_096_start(uint8_t handle)
{
Lcd_Control_096_Drv_TypeDef	*Lcd_Control_096_Drv = (Lcd_Control_096_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;

	Lcd_Control_096_Drv->status |= LCD_096_RUNNING;
	return 0;
}

static uint32_t lcd_control_096_stop(uint8_t handle)
{
Lcd_Control_096_Drv_TypeDef	*Lcd_Control_096_Drv = (Lcd_Control_096_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
	Lcd_Control_096_Drv->status &= ~LCD_096_RUNNING;
	return 0;
}

static uint32_t lcd_control_096_get_status(uint8_t handle)
{
Lcd_Control_096_Drv_TypeDef	*Lcd_Control_096_Drv = (Lcd_Control_096_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
	return Lcd_Control_096_Drv->status;
}

static uint32_t lcd_control_096_get_values(uint8_t handle,uint8_t *values,uint8_t values_number)
{
	return 0;
}

static uint32_t lcd_control_096_set_values(uint8_t handle,uint8_t *values,uint8_t values_number)
{
	return 0;
}

static uint32_t lcd_control_096_extended_actions(uint32_t handle,uint32_t *action)
{
Lcd_Control_096_Drv_TypeDef		*Lcd_Control_096_Drv = (Lcd_Control_096_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
Lcd_Control_096_Actions_TypeDef	*action_struct = (Lcd_Control_096_Actions_TypeDef *)action;

	switch ( action_struct->action )
	{
	case LCD_096_SET_BRIGHTNESS:
		Lcd_Control_096_Drv->backlight_timer->Instance->CCR1 = Lcd_Control_096_Drv->brightness;
		break;
	case LCD_096_START_BACKLIGHT:
		HAL_TIM_PWM_Start(Lcd_Control_096_Drv->backlight_timer,Lcd_Control_096_Drv->backlight_timer_channel);
		break;
	case LCD_096_STOP_BACKLIGHT:
		HAL_TIM_Base_Stop(Lcd_Control_096_Drv->backlight_timer);
		break;
	case LCD_096_CLEAR_SCREEN:
		ST7735_ClearScreen();
		break;
	case LCD_096_DRAW_LOGO:
		ST7735_DrawImage(0,0,ST7735_WIDTH, ST7735_HEIGHT,logo);
		break;
	case LCD_096_WRITE_STRING:
		ST7735_WriteString(action_struct->pos_x,action_struct->pos_y,action_struct->string,action_struct->font,action_struct->color, action_struct->bgcolor);
		break;
	}
	return 0;
}

extern	const DriverStruct_t	Lcd_Control_096_Drv_S;

uint32_t lcd_control_096_deinit(uint8_t handle)
{
	return driver_unregister(&Lcd_Control_096_Drv_S);
}

static uint32_t lcd_control_096_init(uint8_t handle)
{
Lcd_Control_096_Drv_TypeDef	*Lcd_Control_096_Drv = (Lcd_Control_096_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;

	if ( gpio_driver_allocate_gpio(Lcd_Control_096_Drv->ss_port,Lcd_Control_096_Drv->ss_bit) == PIN_ALREADY_ALLOCATED )
		return 1;
	if ( gpio_driver_allocate_gpio(Lcd_Control_096_Drv->dc_port,Lcd_Control_096_Drv->dc_bit) == PIN_ALREADY_ALLOCATED )
		return 1;
	if ( gpio_driver_allocate_gpio(Lcd_Control_096_Drv->reset_port,Lcd_Control_096_Drv->reset_bit) == PIN_ALREADY_ALLOCATED )
		return 1;
	ST7735_SetVars(Lcd_Control_096_Drv);
	ST7735_Init();

	return 0;
}

const DriverStruct_t	Lcd_Control_096_Drv_S =
{
	.init = lcd_control_096_init,
	.deinit = lcd_control_096_deinit,
	.start = lcd_control_096_start,
	.stop = lcd_control_096_stop,
	.extended_action = lcd_control_096_extended_actions,
	.get_status = lcd_control_096_get_status,
	.get_values = lcd_control_096_get_values,
	.set_values = lcd_control_096_set_values,
	.periodic_before_check_timers_callback = NULL,
	.periodic_after_check_timers_callback = NULL,
	.driver_name = "lcd_control_096",
};

uint32_t lcd_control_096_allocate_driver(DriverStruct_t *new_struct)
{
	memcpy(new_struct,&Lcd_Control_096_Drv_S,sizeof(Lcd_Control_096_Drv_S));
	return 0;
}
#endif // #ifdef	LCD_096_ENABLED

#endif // #ifdef OLD

