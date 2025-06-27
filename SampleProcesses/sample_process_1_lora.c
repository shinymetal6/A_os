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
 * sample_process_1_lora.c
 *
 *  Created on: Jun 26, 2025
 *      Author: fil
 */

/* Please note :
 * The pins DIO1 and DIO4 of the RA01S are programmed as input in the ioc.
 * No special functions ( irq ) are present, so they should be polled
 */

#include "main.h"
#include "A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_LORA

#define	PRC1_TICK				10
extern	SPI_HandleTypeDef hspi1;

RA01S_UserConfig_TypeDef	RA01S_UserConfig =
{
		.op_mode_range = SX127X_OP_MODE_RANGE_LORA,
		.lna_boost_gain = SX127X_LNA_GAIN_12DB,
		.is_rx_payload_crc_on = true,
		.bandwidth_khz = SX127X_BANDWIDTH_KHZ_125,
		.header_mode = SX127X_OP_MODE_EXPLICIT_HEADER,
		.spreading_factor = SX127X_SPREADING_FACTOR_8,
		.coding_rate = SX127X_CODING_RATE_4_6,
		.frequency_hz = 434000000,
		.frequency_mode = SX127X_LOW_FREQUENCY_MODE,
		.is_auto_agc_on = true,
		.tx_power_level_dbm = 17,
		.pa_output_pin = SX127X_PA_SELECT_PA_BOOST_PIN,
};

RA01S_Drv_TypeDef	RA01S_Drv =
{
		.spi = &hspi1,
		.CS_port = LORA_SS_GPIO_Port,
		.CS_bit = LORA_SS_Pin,
		.RESET_port = LORA_RESET_GPIO_Port,
		.RESET_bit = LORA_RESET_Pin,
		.RA01S_UserConfig = &RA01S_UserConfig,
		.wakeup_id = WAKEUP_FROM_SPI1_IRQ,
		.flags = 0,
};

uint8_t	can_pkt = 0;

void sample_process_1_lora(uint32_t process_id)
{
uint32_t	wakeup,flags;

	ra01s_register(&RA01S_Drv);
	create_timer(TIMER_ID_0,PRC1_TICK,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	while(1)
	{
		wait_event(EVENT_TIMER);
		get_wakeup_flags(&wakeup,&flags);


		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			if (( flags & TIMER_ID_0) == TIMER_ID_0)
				process_led();
		}
	}
}
#endif // #ifdef SAMPLEPROCESS_1_LORA
#endif // #ifdef SAMPLE_PROCESSES_ENABLED

