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
 * dcc.c
 *
 *  Created on: Mar 1, 2024
 *      Author: fil
 */
#include "main.h"
#include "../../kernel/system_default.h"	/* for BOARD_NAME variable only */

#ifdef DDC_SYSTEM

#include "dcc.h"
#include "dcc_parse_command.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

DCC_System_TypeDef	DCC_System;

DCC_Pkt_TypeDef	DCC_Idle_Pkt =
{
	.preamble = {
			DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,
			DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1
	},
	.separator0 = DCC_SEPARATOR,
	.address = {
			DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1
	},
	.separator1 = DCC_SEPARATOR,
	.data = {
			DCC_0,DCC_0,DCC_0,DCC_0,DCC_0,DCC_0,DCC_0,DCC_0
	},
	.separator2 = DCC_SEPARATOR,
	.ecc = {
			DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1
	},
	.endpacket_short = DCC_SEPARATOR,
	.fill = {
			DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1
	},
	.endpacket_long = DCC_TERM
};

DCC_Pkt_TypeDef	DCC_Work_Pkt =
{
	.preamble = {
			DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,
			DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1
	},
	.separator0 = DCC_SEPARATOR,
	.address = {
			DCC_0,DCC_0,DCC_0,DCC_0,DCC_0,DCC_0,DCC_0,DCC_0
	},
	.separator1 = DCC_SEPARATOR,
	.data = {
			DCC_0,DCC_0,DCC_0,DCC_0,DCC_0,DCC_0,DCC_0,DCC_0
	},
	.separator2 = DCC_SEPARATOR,
	.ecc = {
			DCC_0,DCC_0,DCC_0,DCC_0,DCC_0,DCC_0,DCC_0,DCC_0
	},
	.endpacket_short = DCC_SEPARATOR,
	.fill = {
			DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1
	},
	.endpacket_long = DCC_TERM
};


void set_trig(uint8_t val )
{
	if ( val )
		  HAL_GPIO_WritePin(ARDUINO_D2_GPIO_Port, ARDUINO_D2_Pin, GPIO_PIN_SET);
	else
		  HAL_GPIO_WritePin(ARDUINO_D2_GPIO_Port, ARDUINO_D2_Pin, GPIO_PIN_RESET);
}

void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim)
{
	DCC_System.system |= SYSTEM_DCC1_HALFDONE;
	DCC_System.system |= SYSTEM_DCC1_EXPIRED;

	if ((DCC_System.system & SYSTEM_DCC1_HPACKET) == SYSTEM_DCC1_HPACKET)
	{
		DCC_System.dcc_flags |= DCC_DCC1_TXDONE;
	}
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	DCC_System.system &= ~SYSTEM_DCC1_HALFDONE;
	DCC_System.system |= SYSTEM_DCC1_EXPIRED;

	if ((DCC_System.system & SYSTEM_DCC1_FPACKET) == SYSTEM_DCC1_FPACKET)
	{
		DCC_System.dcc_flags |= DCC_DCC1_TXDONE;
	}
}

void init_dcc_pkt(void)
{
	memcpy(&DCC_System.dcc_ch1_packet,&DCC_Idle_Pkt,DCC_PACKET_LEN*2);
	memcpy(&DCC_System.dcc_ch1_packet[DCC_PACKET_LEN],&DCC_Idle_Pkt,DCC_PACKET_LEN*2);
}

void dcc_init(void)
{
	DBGMCU->APB1FZ = 0xFFFFFFFF;
	DBGMCU->APB2FZ = 0xFFFFFFFF;

	HAL_TIM_Base_Start_IT(&TICK_TIMER);
	init_dcc_pkt();
	DCC_System.dcc_ch1_repeat_cnt = DCC_System.dcc_ch1_repeat_number = 1;
	dcc_HAL_TIM_PWM_Start_DMA(&DCC_CH1,TIM_CHANNEL_1,(uint32_t *)DCC_System.dcc_ch1_packet,DCC_PACKET_LEN*NUM_DCC_PACKET );
	HAL_Delay(20);
}

void dcc_process(void)
{
	if ((DCC_System.dcc_flags & DCC_DCC1_POWER) == DCC_DCC1_POWER)
		DCC_CH1.Instance->CR1 |= TIM_CR1_CEN;
	else
		DCC_CH1.Instance->CR1 &= ~TIM_CR1_CEN;
	if ((DCC_System.dcc_flags & DCC_DCC2_POWER) == DCC_DCC2_POWER)
		DCC_CH2.Instance->CR1 |= TIM_CR1_CEN;
	else
		DCC_CH2.Instance->CR1 &= ~TIM_CR1_CEN;

	if ((DCC_System.system & SYSTEM_DCC1_EXPIRED) == SYSTEM_DCC1_EXPIRED)
	{
		DCC_System.system &= ~SYSTEM_DCC1_EXPIRED;
		if ((DCC_System.dcc_flags & DCC_DCC1_PKTPEND) == DCC_DCC1_PKTPEND)
		{
			if ((DCC_System.system & SYSTEM_DCC1_HALFDONE) == SYSTEM_DCC1_HALFDONE)
			{
				memcpy(&DCC_System.dcc_ch1_packet,&DCC_Work_Pkt,DCC_PACKET_LEN*2);
				DCC_System.system |= SYSTEM_DCC1_HPACKET;
			}
			else
			{
				memcpy(&DCC_System.dcc_ch1_packet[DCC_PACKET_LEN],&DCC_Work_Pkt,DCC_PACKET_LEN*2);
				DCC_System.system |= SYSTEM_DCC1_FPACKET;
			}
			set_trig(1);
			DCC_System.dcc_flags &= ~DCC_DCC1_PKTPEND;
		}
	}
	if ((DCC_System.dcc_flags & DCC_DCC1_TXDONE) == DCC_DCC1_TXDONE)
	{
		if ( DCC_System.dcc_ch1_repeat_cnt == 0 )
		{
			if ((DCC_System.system & SYSTEM_DCC1_HPACKET) == SYSTEM_DCC1_HPACKET)
			{
				memcpy(&DCC_System.dcc_ch1_packet,&DCC_Idle_Pkt,DCC_PACKET_LEN*2);
				DCC_System.system &= ~SYSTEM_DCC1_HPACKET;
			}
			if ((DCC_System.system & SYSTEM_DCC1_FPACKET) == SYSTEM_DCC1_FPACKET)
			{
				memcpy(&DCC_System.dcc_ch1_packet[DCC_PACKET_LEN],&DCC_Idle_Pkt,DCC_PACKET_LEN*2);
				DCC_System.system &= ~SYSTEM_DCC1_FPACKET;
			}
			DCC_System.dcc_ch1_repeat_cnt = DCC_System.dcc_ch1_repeat_number;
			DCC_System.dcc_flags &= ~DCC_DCC1_PKTPEND;
			set_trig(0);
		}
		else
		{
			DCC_System.dcc_ch1_repeat_cnt--;
			DCC_System.dcc_flags |= DCC_DCC1_PKTPEND;
		}
		DCC_System.dcc_flags &= ~DCC_DCC1_TXDONE;
	}
}

#endif // #ifdef DDC_SYSTEM


