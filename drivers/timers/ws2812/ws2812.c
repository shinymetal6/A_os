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

/*
 * Universal Bridge Callback:
 * Translates low-level raw DMA interrupts back into standard HAL Timer callbacks.
 */
void ws2812_DMA_Polymorphic_Bridge(DMA_HandleTypeDef *hdma)
{
    // Retrieve the parent Timer handle associated with this DMA stream
    TIM_HandleTypeDef *htim = (TIM_HandleTypeDef *)hdma->Parent;
    HAL_TIM_PeriodElapsedCallback(htim);
}
/*
 * Universal Polymorphic Bridge:
 * Resolves and forwards low-level raw DMA completion events back into the standard
 * HAL application callback vectors depending on active trigger configurations.
 */
/*
void ws2812_DMA_Polymorphic_Bridge(DMA_HandleTypeDef *hdma)
{
    TIM_HandleTypeDef *htim = (TIM_HandleTypeDef *)hdma->Parent;
    if (htim == NULL) return;

    if (htim->Instance->DIER & TIM_DIER_UDE)
    {
        HAL_TIM_PeriodElapsedCallback(htim);
    }
    else
    {
        HAL_TIM_PWM_PulseFinishedCallback(htim);
    }
}
*/
ITCM_AREA_CODE void ws2812_Show_Frame(WS2812_DriverStruct_t* ws2812_drv)
{
	uint32_t i, color_word, start_bit_index;
	TIM_TypeDef* TIMx = ws2812_drv->ws2812_timer->Instance;

	/* 1. Bit-pack structural RGB data items using native GRB sequence rules */
	for (i = 0; i < ws2812_drv->num_leds; i++)
	{
		/* High-speed bit-shifting (>> 8) replaces division execution clock overhead inside ITCM */
		uint16_t g_prod = (uint16_t)ws2812_drv->led_strip_data[i].G * ws2812_drv->brightness;
		uint16_t r_prod = (uint16_t)ws2812_drv->led_strip_data[i].R * ws2812_drv->brightness;
		uint16_t b_prod = (uint16_t)ws2812_drv->led_strip_data[i].B * ws2812_drv->brightness;

		uint8_t scaled_g = (uint8_t)((g_prod + (g_prod >> 8) + 1) >> 8);
		uint8_t scaled_r = (uint8_t)((r_prod + (r_prod >> 8) + 1) >> 8);
		uint8_t scaled_b = (uint8_t)((b_prod + (b_prod >> 8) + 1) >> 8);

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

	// Clear all possible DMA requests across any timer layout configuration
	TIMx->DIER &= ~(TIM_DIER_UDE | TIM_DIER_CC1DE | TIM_DIER_CC2DE | TIM_DIER_CC3DE | TIM_DIER_CC4DE);
	HAL_DMA_Abort(ws2812_drv->hdma);

	/*
	 * 4. UNIVERSAL TARGET REGISTER ADDRESS CALCULATION:
	 * In STM32 HAL, TIM_CHANNEL_1 = 0, TIM_CHANNEL_2 = 4, TIM_CHANNEL_3 = 8, TIM_CHANNEL_4 = 12.
	 * Registers are spaced exactly 4 bytes apart. This formula safely targets any CCR register on any MCU!
	 */
	uint32_t channel_idx = ws2812_drv->tim_channel / 4U;
	uint32_t ccr_target_address = (uint32_t)&(TIMx->CCR1) + (channel_idx * 4U);

	// Force clear target compare status register
	*(__IO uint32_t *)ccr_target_address = 0U;
	__HAL_TIM_CLEAR_FLAG(ws2812_drv->ws2812_timer, TIM_FLAG_UPDATE);

	/* 5. Set up our polymorphic bridge routing hooks */
	ws2812_drv->hdma->XferCpltCallback = ws2812_DMA_Polymorphic_Bridge;

	/* 6. Launch Transmission Engine Stream */
	ws2812_drv->transmitting = 1;
	HAL_DMA_Start_IT(ws2812_drv->hdma, (uint32_t)ws2812_drv->dma_pwm_buffer, ccr_target_address, ws2812_drv->dma_buf_size);

	/*
	 * 7. MACRO-FREE POLYMORPHIC HARDWARE GATING SWITCHBOARD:
	 * Detect if the timer is a single/dual channel advanced breakaway type (TIM15, TIM16, TIM17).
	 * These specific peripherals do not support dedicated hardware DMA channels for CH2-CH4 in silicon.
	 */
	uint8_t use_update_fallback = 0;

	// Direct address evaluation maps these special instances safely without requiring #ifdef guards
	if (((uint32_t)TIMx == TIM15_BASE || (uint32_t)TIMx == TIM16_BASE || (uint32_t)TIMx == TIM17_BASE)
	    && ws2812_drv->tim_channel != TIM_CHANNEL_1)
	{
		use_update_fallback = 1;
	}

	if (use_update_fallback)
	{
		TIMx->DIER |= TIM_DIER_UDE; // Fall back to the universal rollover Update gate
	}
	else
	{
		// Map the exact channel comparison register bit dynamically (CC1DE up to CC4DE)
		TIMx->DIER |= (TIM_DIER_CC1DE << channel_idx);
	}

	// Connect internal comparison logic circuitry straight out to the physical pin matrix
	TIMx->CCER |= (TIM_CCER_CC1E << (channel_idx * 4U));

	// Clear counter registry tracking fields and kick off core execution count clocks
	TIMx->CNT = 0U;
	TIMx->CR1 |= TIM_CR1_CEN;

	/* 8. Advanced Breakaway Gate Verification Checks (Executes for TIM1, TIM8, TIM15, TIM16, TIM17) */
	if (ws2812_drv->moe)
	{
		__HAL_TIM_MOE_ENABLE(ws2812_drv->ws2812_timer);
	}
	else
	{
		/*
		 * Safe Dynamic Validation: Detect advanced/breakaway functionality by comparing
		 * peripheral base boundaries to see if a BDTR register block physically exists on this instance.
		 * Basic and basic general-purpose timers (TIM2, TIM3, TIM4, TIM5) will bypass this layout safely.
		 */
		if ((uint32_t)TIMx == TIM1_BASE  || (uint32_t)TIMx == TIM8_BASE  ||
		    (uint32_t)TIMx == TIM15_BASE || (uint32_t)TIMx == TIM16_BASE || (uint32_t)TIMx == TIM17_BASE)
		{
			TIMx->BDTR |= TIM_BDTR_MOE;
		}
	}
}



ITCM_AREA_CODE void ws2812_Show_Frame_OK(WS2812_DriverStruct_t* ws2812_drv)
{
	uint32_t i, color_word, start_bit_index;
	/* 1. Bit-pack structural RGB data items using native GRB sequence rules */
	for (i = 0; i < ws2812_drv->num_leds; i++)
	{
		uint16_t g_prod = (uint16_t)ws2812_drv->led_strip_data[i].G * ws2812_drv->brightness;
		uint16_t r_prod = (uint16_t)ws2812_drv->led_strip_data[i].R * ws2812_drv->brightness;
		uint16_t b_prod = (uint16_t)ws2812_drv->led_strip_data[i].B * ws2812_drv->brightness;

		uint8_t scaled_g = (uint8_t)((g_prod + (g_prod >> 8) + 1) >> 8);
		uint8_t scaled_r = (uint8_t)((r_prod + (r_prod >> 8) + 1) >> 8);
		uint8_t scaled_b = (uint8_t)((b_prod + (b_prod >> 8) + 1) >> 8);

		color_word = ((uint32_t)scaled_g << 16) |
					 ((uint32_t)scaled_r << 8)  |
					 scaled_b;

		start_bit_index = i * 24;

		for (int b = 0; b < 24; b++)
		{
			if (color_word & (1UL << (23 - b)))
				ws2812_drv->dma_pwm_buffer[start_bit_index + b] = ws2812_drv->t1h_duty;
			else
				ws2812_drv->dma_pwm_buffer[start_bit_index + b] = ws2812_drv->t0h_duty;
		}
	}

	/* 2. Build trailing zero-duty latch region */
	for (i = (ws2812_drv->num_leds * 24); i < ws2812_drv->dma_buf_size; i++)
		ws2812_drv->dma_pwm_buffer[i] = 0U;

	/* 3. Safely flush stuck hardware states cleanly */
	// Turning off counters and clearing DIER registers prevents interrupt looping
	ws2812_drv->ws2812_timer->Instance->CR1 &= ~TIM_CR1_CEN;
	ws2812_drv->ws2812_timer->Instance->DIER &= ~(TIM_DIER_UDE | TIM_DIER_CC4DE | TIM_DIER_CC2DE);
	HAL_DMA_Abort(ws2812_drv->hdma);

	/* 4. Map the explicit comparison register target address */
	uint32_t ccr_target_address = 0U;
	switch(ws2812_drv->tim_channel)
	{
	case TIM_CHANNEL_1 :	ccr_target_address = (uint32_t)&ws2812_drv->ws2812_timer->Instance->CCR1; break;
	case TIM_CHANNEL_2 :	ccr_target_address = (uint32_t)&ws2812_drv->ws2812_timer->Instance->CCR2; break;
	case TIM_CHANNEL_3 :	ccr_target_address = (uint32_t)&ws2812_drv->ws2812_timer->Instance->CCR3; break;
	case TIM_CHANNEL_4 :	ccr_target_address = (uint32_t)&ws2812_drv->ws2812_timer->Instance->CCR4; break;
	case TIM_CHANNEL_5 :	ccr_target_address = (uint32_t)&ws2812_drv->ws2812_timer->Instance->CCR5; break;
	case TIM_CHANNEL_6 :	ccr_target_address = (uint32_t)&ws2812_drv->ws2812_timer->Instance->CCR6; break;
	}

	*(__IO uint32_t *)ccr_target_address = 0U;
	__HAL_TIM_CLEAR_FLAG(ws2812_drv->ws2812_timer, TIM_FLAG_UPDATE);

	/* 5. Stream data using raw low-level explicit address routing */
	ws2812_drv->transmitting = 1;
	HAL_DMA_Start_IT(ws2812_drv->hdma, (uint32_t)ws2812_drv->dma_pwm_buffer, ccr_target_address, ws2812_drv->dma_buf_size);

	/* 6. Dynamic Hardware Gating Routing (Forces proper G4 DMAMUX routing paths) */
	if (ws2812_drv->ws2812_timer->Instance == TIM2)
	{
		switch(ws2812_drv->tim_channel)
		{
		case TIM_CHANNEL_1 :	ws2812_drv->ws2812_timer->Instance->DIER |= TIM_DIER_CC1DE; ws2812_drv->ws2812_timer->Instance->CCER |= TIM_CCER_CC1E;break;
		case TIM_CHANNEL_2 :	ws2812_drv->ws2812_timer->Instance->DIER |= TIM_DIER_CC2DE; ws2812_drv->ws2812_timer->Instance->CCER |= TIM_CCER_CC2E;break;
		case TIM_CHANNEL_3 :	ws2812_drv->ws2812_timer->Instance->DIER |= TIM_DIER_CC3DE; ws2812_drv->ws2812_timer->Instance->CCER |= TIM_CCER_CC3E;break;
		case TIM_CHANNEL_4 :	ws2812_drv->ws2812_timer->Instance->DIER |= TIM_DIER_CC4DE; ws2812_drv->ws2812_timer->Instance->CCER |= TIM_CCER_CC4E;break;
		}
		/*
		ws2812_drv->ws2812_timer->Instance->DIER |= TIM_DIER_CC4DE; // Use standard CC4 channel for TIM2
		ws2812_drv->ws2812_timer->Instance->CCER |= TIM_CCER_CC4E;
		*/
	}
#ifdef TIM15
	else if (ws2812_drv->ws2812_timer->Instance == TIM15)
	{
		ws2812_drv->ws2812_timer->Instance->DIER |= TIM_DIER_UDE;   // Force UPDATE trigger routing for TIM15
		ws2812_drv->ws2812_timer->Instance->CCER |= TIM_CCER_CC2E;
	}
#endif

	// Clear counter tracking states and fire up the execution clocks
	ws2812_drv->ws2812_timer->Instance->CNT = 0U;
	ws2812_drv->ws2812_timer->Instance->CR1 |= TIM_CR1_CEN;

	/* 7. Advanced Breakaway Gate Verification Checks */
	if ( ws2812_drv->moe == 1)
		ws2812_drv->ws2812_timer->Instance->BDTR |= TIM_BDTR_MOE;
}




ITCM_AREA_CODE void ws2812_Show_Frame_OLD(WS2812_DriverStruct_t* ws2812_drv)
{
uint32_t i,	color_word,start_bit_index;
	/* 1. Bit-pack structural RGB data items using native GRB sequence rules */
	for (i = 0; i < ws2812_drv->num_leds; i++)
	{
		/* High-speed bit-shifting (>> 8) replaces division execution clock overhead inside ITCM */

			uint16_t g_prod = (uint16_t)ws2812_drv->led_strip_data[i].G * ws2812_drv->brightness;
			uint16_t r_prod = (uint16_t)ws2812_drv->led_strip_data[i].R * ws2812_drv->brightness;
			uint16_t b_prod = (uint16_t)ws2812_drv->led_strip_data[i].B * ws2812_drv->brightness;

			uint8_t scaled_g = (uint8_t)((g_prod + (g_prod >> 8) + 1) >> 8);
			uint8_t scaled_r = (uint8_t)((r_prod + (r_prod >> 8) + 1) >> 8);
			uint8_t scaled_b = (uint8_t)((b_prod + (b_prod >> 8) + 1) >> 8);

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

extern	void WS2812_DMA_XferCpltCallback(TIM_HandleTypeDef *htim);

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
	ws2812_drv->hdma->XferCpltCallback = ws2812_DMA_Polymorphic_Bridge;

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

    ws2812_drv->tim_instance = ws2812_drv->ws2812_timer->Instance;

	ws2812_drv->moe = 0;

    if ((ws2812_drv->tim_instance == TIM1) || (ws2812_drv->tim_instance == TIM8)  || (ws2812_drv->tim_instance == TIM15) || (ws2812_drv->tim_instance == TIM16) || (ws2812_drv->tim_instance == TIM17))
    {
        ws2812_drv->moe = 1;
    }

	if ((ws2812_drv->tim_instance==TIM1) ||
		(ws2812_drv->tim_instance==TIM2) ||
		(ws2812_drv->tim_instance==TIM3) ||
		(ws2812_drv->tim_instance==TIM4) ||
#if defined(TIM5)
		(ws2812_drv->tim_instance==TIM5) ||
#endif // #if defined(TIM5)
		(ws2812_drv->tim_instance==TIM8) )
	{
			switch(ws2812_drv->tim_channel)
			{
			case TIM_CHANNEL_1 :	ws2812_drv->tim_dma_cc_id = TIM_DMA_CC1; break;
			case TIM_CHANNEL_2 :	ws2812_drv->tim_dma_cc_id = TIM_DMA_CC2; break;
			case TIM_CHANNEL_3 :	ws2812_drv->tim_dma_cc_id = TIM_DMA_CC3; break;
			case TIM_CHANNEL_4 :	ws2812_drv->tim_dma_cc_id = TIM_DMA_CC4; break;
			}
	}
	else if (
#if defined(TIM12)
			 (ws2812_drv->tim_instance==TIM12) ||
#endif //#if defined(TIM12)
#if defined(TIM13)
			 (ws2812_drv->tim_instance==TIM13) ||
#endif //#if defined(TIM13)
#if defined(TIM14)
			 (ws2812_drv->tim_instance==TIM14) ||
#endif //#if defined(TIM14)
			 (ws2812_drv->tim_instance==TIM15) ||
			 (ws2812_drv->tim_instance==TIM16) ||
			 (ws2812_drv->tim_instance==TIM17) )
	{
		ws2812_drv->tim_dma_cc_id = TIM_DMA_UPDATE;
	}
	else
        return DRIVER_REQUEST_FAILED;

    return 0;
}
#endif // #ifdef A_OS_TIMERS_ENABLED
