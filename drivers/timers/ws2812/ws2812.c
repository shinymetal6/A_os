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
 * ws2812.c
 *
 *  Created on: Feb 9, 2026
 *      Author: fil
 */

#include "main.h"

#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef A_OS_TIMERS_ENABLED

#include "ws2812.h"

ITCM_AREA_CODE void ws2812_SetLED(WS2812_DriverStruct_t* ws2812_drv,uint16_t index, uint8_t r, uint8_t g, uint8_t b)
{
    if (index < ws2812_drv->num_leds)
    {
    	ws2812_drv->led_strip_data[index].R = r;
    	ws2812_drv->led_strip_data[index].G = g;
    	ws2812_drv->led_strip_data[index].B = b;
    }
}

ITCM_AREA_CODE void ws2812_Show_Frame(WS2812_DriverStruct_t* ws2812_drv)
{
	// Wait safely if a prior frame transmission is still in progress
    while (ws2812_drv->transmitting)
    	task_delay(1);

    uint32_t indx = 0;
    uint32_t color;

    for (int i = 0; i < ws2812_drv->num_leds; i++)
    {
        // WS2812 data format protocol uses GRB layout
    	uint16_t	bR = (ws2812_drv->led_strip_data[i].R * ws2812_drv->brightness) >> 8;
    	uint16_t	bG = (ws2812_drv->led_strip_data[i].G * ws2812_drv->brightness) >> 8;
    	uint16_t	bB = (ws2812_drv->led_strip_data[i].B * ws2812_drv->brightness) >> 8;
    	color = (bG << 16) | (bR << 8) | bB;

        for (int b = 23; b >= 0; b--) {
            if (color & (1 << b)) {
            	ws2812_drv->dma_pwm_buffer[indx++] = ws2812_drv->t1h_duty;
            } else {
            	ws2812_drv->dma_pwm_buffer[indx++] = ws2812_drv->t0h_duty;
            }
        }
    }

    // Append safe low logic outputs to create the final reset packet
    for (int i = 0; i < WS2812_LATCH_BITS; i++) {
    	ws2812_drv->dma_pwm_buffer[indx++] = 0;
    }
    ws2812_drv->transmitting = 1;
    HAL_TIM_PWM_Start_DMA(ws2812_drv->ws2812_timer, ws2812_drv->tim_channel, (uint32_t *)ws2812_drv->dma_pwm_buffer, indx);
}

extern	void WS2812_DMA_XferCpltCallback(TIM_HandleTypeDef *htim);

ITCM_AREA_CODE uint32_t ws2812_register(WS2812_DriverStruct_t *ws2812_drv)
{
    TIMER_DriverStruct_t *eptr;
    if ( ws2812_drv->ws2812_timer == NULL)
        return DRIVER_REQUEST_FAILED;
    if ( ws2812_drv->dma_pwm_buffer == NULL)
        return DRIVER_REQUEST_FAILED;
    if ( ws2812_drv->led_strip_data == NULL)
        return DRIVER_REQUEST_FAILED;
    if (( ws2812_drv->dma_buf_size == 0 ) || ( ws2812_drv->dma_buf_size > WS2812_MAX_BUFLEN ) )
        return DRIVER_REQUEST_FAILED;
    if (( ws2812_drv->num_leds == 0 ) || ( ws2812_drv->num_leds > WS2812_MAX_NUMLEDS))
        return DRIVER_REQUEST_FAILED;

    if ( timer_drv_ptr == NULL)
    {
        timer_drv_ptr = (TIMER_DriverStruct_t *)ws2812_drv;
        ws2812_drv->next_timer = NULL;
    }
    else
    {
        eptr = timer_drv_ptr;
        while(eptr->next_timer != NULL)
            eptr = (TIMER_DriverStruct_t *)eptr->next_timer;
        eptr->next_timer = (uint32_t *)ws2812_drv;
        ws2812_drv->next_timer = NULL;
    }

    ws2812_drv->process = get_current_process();
    ws2812_drv->timer_type = TIM_TYPE_PWM;

    // Set up brightness structural default to maximum scale if unassigned
    if ( ws2812_drv->brightness == 0 )
        ws2812_drv->brightness = 255;

    // Check if the chosen timer belongs to the APB2 bus domain (TIM1, TIM8, TIM15, TIM16, TIM17)
    uint32_t timer_clk;
    uint32_t ws2812_arr = 299;
    if (ws2812_drv->ws2812_timer->Instance == TIM1 || ws2812_drv->ws2812_timer->Instance == TIM8 ||	ws2812_drv->ws2812_timer->Instance == TIM15 || ws2812_drv->ws2812_timer->Instance == TIM16 || ws2812_drv->ws2812_timer->Instance == TIM17)
    {
        timer_clk = HAL_RCC_GetPCLK2Freq() * 2;
    }
    else
    {
        timer_clk = HAL_RCC_GetPCLK1Freq() * 2;
    }
    // Dynamic Period Calculation for 800 kHz (1.25 µs)
    ws2812_arr = (timer_clk / 800000UL) - 1;
    ws2812_drv->ARR = ws2812_arr;
	// Dynamic Pulse Width Calculations in Nanoseconds
    ws2812_drv->t0h_duty = (uint16_t)((350ULL * timer_clk) / 1000000000ULL); // target: 84 - 85
    ws2812_drv->t1h_duty = (uint16_t)((700ULL * timer_clk) / 1000000000ULL); // target: 168

	// Update the physical timer hardware Auto-Reload Register directly
	ws2812_drv->PSC = 0;
	__HAL_TIM_SET_PRESCALER(ws2812_drv->ws2812_timer,ws2812_drv->PSC);
	__HAL_TIM_SET_AUTORELOAD(ws2812_drv->ws2812_timer, ws2812_drv->ARR);
    return 0;
}
#endif // #ifdef A_OS_TIMERS_ENABLED
