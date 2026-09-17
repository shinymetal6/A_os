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
 * ws2812.h
 *
 *  Created on: Feb 9, 2026
 *      Author: fil
 */

#ifndef DRIVERS_TIMERS_WS2812_WS2812_H_
#define DRIVERS_TIMERS_WS2812_WS2812_H_

#define	WS2812_LEDBPP		24
#define	WS2812_MAX_NUMLEDS	10

// Some WS2812 variants (especially clones or long strips) require ≥256 µs reset pulse instead of the standard 50 µs spec
#define	WS2812_SYNCLEN		500

#define	WS2812_MAX_BUFLEN	((WS2812_SYNCLEN*WS2812_LEDBPP)+(WS2812_MAX_NUMLEDS*WS2812_LEDBPP))

// Reset Head: 250 bits * 1.25µs = 312.5µs (Safely > 280µs for modern LEDs)
#define WS2812_RESET_HEAD 250
// Reset Tail: 250 bits * 1.25µs = 312.5µs (Safely > 280µs for modern LEDs)
#define WS2812_RESET_TAIL 250

#define WS2812_TARGET_FREQ_HZ  800000UL      // Fixed protocol frequency (800 kHz)
#define DRV_CALC_PSC           0
#define DRV_CALC_ARR           ((HSI_CLOCK / WS2812_TARGET_FREQ_HZ) - 1UL)
#define DRV_TOTAL_COUNTS       (DRV_CALC_ARR + 1UL)
#define DRV_CALC_T0H           (((DRV_TOTAL_COUNTS * 28UL) + 50UL) / 100UL)
#define DRV_CALC_T1H           (((DRV_TOTAL_COUNTS * 56UL) + 50UL) / 100UL)

typedef struct {
    uint8_t R;
    uint8_t G;
    uint8_t B;
}LED_Frame_Struct_t;

typedef struct
{
	/* timer header */
	uint8_t 			process;
	uint8_t				status;
	uint8_t				flags;
	uint32_t 			*next_timer;
	TIM_HandleTypeDef 	*ws2812_timer;
	uint8_t				timer_type;
	uint32_t			wakeup_id;
	/* timer internals */
#include "main.h"
/* The Master WS2812 Driver Configuration and Runtime Struct */
    /* 1. Hardware Object Interface Handles */
    DMA_HandleTypeDef*  hdma;            // Pointer to HAL DMA Handle (&hdma_tim2_ch4)

    /* 2. Low-Level Hardware Base Instances */
    TIM_TypeDef*        tim_instance;    // Raw hardware register address (TIM2)
    DMA_TypeDef*        dma_instance;    // Raw hardware DMA controller base address (DMA1)

    /* 3. Timer & DMA Configuration Properties */
    uint32_t            tim_channel;     // Output channel identifier (TIM_CHANNEL_4)
    uint32_t            tim_dma_cc_id;   // Timer DMA generation target flag (TIM_DMA_CC4)
    uint32_t            dma_isr_tc_mask; // DMA controller status check mask flag (DMA_ISR_TCIF1)
    uint32_t            dma_ifcr_clear_mask; // DMA clear flag bit manipulation mask (DMA_IFCR_CTCIF1)

    /* 4. Dynamic Protocol Timing Constants */
    uint16_t            t0h_duty;        // Logical 0 duty cycle value (60)
    uint16_t            t1h_duty;        // Logical 1 duty cycle value (120)
    uint16_t            arr_val;  		 // arr value
    uint16_t            psc_val;  		 // psc value

    /* 5. Strips Dimension Properties */
    uint16_t            num_leds;        // Physical count of LEDs in this strip
    uint16_t            dma_buf_size;    // Dynamically calculated total array allocation size

    /* 6. Runtime Allocation Memory Buffers */
    uint16_t*           dma_pwm_buffer;  // Pointer to localized 16-bit DMA output buffer array
    LED_Frame_Struct_t* led_strip_data;  // Pointer to localized RGB state structural data array

    /* 7. Asynchronous Handshake Flags */
    uint8_t    			transmitting;     // Transmission tracking handle flag (1 = transmitting, 0 = idle)
    uint8_t    			moe;
	void				(*irq_ws2812_callback)  (uint32_t parameter);
}WS2812_DriverStruct_t;

extern uint32_t	ws2812_register(WS2812_DriverStruct_t *private_data);
extern void 	ws2812_Show_Frame(WS2812_DriverStruct_t* ws2812_drv);

#endif /* DRIVERS_TIMERS_WS2812_WS2812_H_ */
