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

ITCM_AREA_CODE void ws2812_Show_Frame(WS2812_DriverStruct_t* ws2812_drv)
{
uint32_t i,	color_word,start_bit_index;
	/* 1. Bit-pack structural RGB data items using native GRB sequence rules */
	for (i = 0; i < ws2812_drv->num_leds; i++)
	{
		/* High-speed bit-shifting (>> 8) replaces division execution clock overhead inside ITCM */
			uint8_t scaled_g = (uint8_t)(((uint16_t)ws2812_drv->led_strip_data[i].G * ws2812_drv->brightness) >> 8);
			uint8_t scaled_r = (uint8_t)(((uint16_t)ws2812_drv->led_strip_data[i].R * ws2812_drv->brightness) >> 8);
			uint8_t scaled_b = (uint8_t)(((uint16_t)ws2812_drv->led_strip_data[i].B * ws2812_drv->brightness) >> 8);

			color_word = ((uint32_t)scaled_g << 16) |
						 ((uint32_t)scaled_r << 8)  |
						 scaled_b;

			start_bit_index = i * 24; // 24 bits per LED

			for (int b = 0; b < 24; b++)
			{
				if (color_word & (1UL << (23 - b)))
					ws2812_drv->dma_pwm_buffer[start_bit_index + b] = ws2812_drv->t1h_duty;
				else
					ws2812_drv->dma_pwm_buffer[start_bit_index + b] = ws2812_drv->t0h_duty;
			}
	}

	/* 2. Build trailing zero-duty latch region (from end of active data to end of allocated buffer) */
	for (i = (ws2812_drv->num_leds * 24); i < ws2812_drv->dma_buf_size; i++)
		ws2812_drv->dma_pwm_buffer[i] = 0U;

	/* 3. Flush stuck peripheral engine states cleanly */
	HAL_TIM_PWM_Stop(ws2812_drv->ws2812_timer, ws2812_drv->tim_channel);
	__HAL_TIM_DISABLE_DMA(ws2812_drv->ws2812_timer, ws2812_drv->tim_dma_cc_id);
	HAL_DMA_Abort(ws2812_drv->hdma);

	// Clear register completely to prevent duplicate APB bus reflections
	*(__IO uint32_t *)((uint32_t)&ws2812_drv->tim_instance->CCR1 + ws2812_drv->tim_channel) = 0U;

	__HAL_TIM_CLEAR_FLAG(ws2812_drv->ws2812_timer, TIM_FLAG_UPDATE);
	/* 4. Stream 16-bit RAM entries explicitly to the full 32-bit register address */
	uint32_t ccr_target_address = (uint32_t)&ws2812_drv->tim_instance->CCR1 + ws2812_drv->tim_channel;
	HAL_DMA_Start_IT(ws2812_drv->hdma, (uint32_t)ws2812_drv->dma_pwm_buffer, ccr_target_address, ws2812_drv->dma_buf_size);

	/* 5. Initialize the hardware background data pump */
	__HAL_TIM_ENABLE_DMA(ws2812_drv->ws2812_timer, ws2812_drv->tim_dma_cc_id);
	HAL_TIM_PWM_Start(ws2812_drv->ws2812_timer, ws2812_drv->tim_channel);

	__HAL_TIM_ENABLE_IT(ws2812_drv->ws2812_timer, TIM_IT_UPDATE);
	ws2812_drv->transmitting = 1;

	if (ws2812_drv->moe)
		__HAL_TIM_MOE_ENABLE(ws2812_drv->ws2812_timer);
}

ITCM_AREA_CODE uint32_t ws2812_register(WS2812_DriverStruct_t *ws2812_drv)
{
    TIMER_DriverStruct_t *eptr;
    if ( ws2812_drv->ws2812_timer == NULL)
        return DRIVER_REQUEST_FAILED;
    if ( ws2812_drv->dma_instance == NULL)
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

    if ( ws2812_drv->t1h_duty == 0 )
        ws2812_drv->t1h_duty = DRV_CALC_T1H;
    if ( ws2812_drv->t0h_duty == 0 )
        ws2812_drv->t0h_duty = DRV_CALC_T0H;
    if ( ws2812_drv->arr_val != 0 )
        ws2812_drv->ws2812_timer->Instance->ARR = ws2812_drv->arr_val;
    else
        ws2812_drv->ws2812_timer->Instance->ARR = ws2812_drv->arr_val = DRV_CALC_ARR;
    __HAL_TIM_SET_AUTORELOAD(ws2812_drv->ws2812_timer, ws2812_drv->arr_val);
    if ( ws2812_drv->psc_val != 0 )
        ws2812_drv->ws2812_timer->Instance->PSC = ws2812_drv->psc_val;
    else
        ws2812_drv->ws2812_timer->Instance->PSC = ws2812_drv->psc_val= DRV_CALC_PSC;

    ws2812_drv->moe = 0;
    if ((ws2812_drv->tim_instance == TIM1) || (ws2812_drv->tim_instance == TIM8)  || (ws2812_drv->tim_instance == TIM15) || (ws2812_drv->tim_instance == TIM16) || (ws2812_drv->tim_instance == TIM17))
        ws2812_drv->moe = 1;

    return 0;
}
#endif // #ifdef A_OS_TIMERS_ENABLED
