/*
 * dcc.c
 *
 *  Created on: Mar 7, 2024
 *      Author: fil
 */


#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"

#ifdef DDC_SYSTEM_ENABLE

#include "dcc.h"
#include <string.h>
#include <stdio.h>
extern	HW_Timers_t		HW_Timers[A_MAX_TIMERS];


DCC_Config_TypeDef	DCC_Config_Track[2];

DCC_BasePkt_TypeDef	DCC_StandardIdle_Pkt =
{
	.preamble = {
			DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,
			DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1
	},
	.packet_start_bit = DCC_START_BIT,
	.address = {
			DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1
	},
	.data_byte_start0_bit = DCC_START_BIT,
	.instruction = {
			DCC_0,DCC_0,DCC_0,DCC_0,DCC_0,DCC_0,DCC_0,DCC_0
	},
	.data_byte_start1_bit = DCC_START_BIT,
	.detection_short = {
			DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1
	},
	.packet_end_bit = DCC_END_BIT,
	.detection_long = {
			DCC_CUTOUT,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1
	},
	.long_packet_end_bit = DCC_END_BIT,
	.endpacket_long = DCC_END_BIT
};

DCC_BasePkt_TypeDef	DCC_ExtendedIdle_Pkt =
{
	.preamble = {
			DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,
			DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1
	},
	.packet_start_bit = DCC_START_BIT,
	.address = {
			DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1
	},
	.data_byte_start0_bit = DCC_START_BIT,
	.instruction = {
			DCC_0,DCC_0,DCC_0,DCC_0,DCC_0,DCC_0,DCC_0,DCC_0
	},
	.data_byte_start1_bit = DCC_START_BIT,
	.detection_short = {
			DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1
	},
	.packet_end_bit = DCC_END_BIT,
	.detection_long = {
			DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1,DCC_1
	},
	.long_packet_end_bit = DCC_END_BIT,
	.endpacket_long = DCC_CUTOUT
};

DCC_BasePkt_TypeDef	DCC_CutOutStandardPkt =
{
	.preamble = {
			DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,
			DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0
	},
	.packet_start_bit = DCC_CTF_0,
	.address = {
			DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0
	},
	.data_byte_start0_bit = DCC_CTF_0,
	.instruction = {
			DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0
	},
	.data_byte_start1_bit = DCC_CTF_0,
	.detection_short = {
			DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0
	},
	.packet_end_bit = DCC_CTF_0,
	.detection_long = {
			DCC_CTF_1,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0
	},
	.long_packet_end_bit = DCC_CTF_0,
	.endpacket_long = DCC_CTF_0
};

DCC_BasePkt_TypeDef	DCC_CutOutExtendedPkt =
{
	.preamble = {
			DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,
			DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0
	},
	.packet_start_bit = DCC_CTF_0,
	.address = {
			DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0
	},
	.data_byte_start0_bit = DCC_CTF_0,
	.instruction = {
			DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0
	},
	.data_byte_start1_bit = DCC_CTF_0,
	.detection_short = {
			DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0
	},
	.packet_end_bit = DCC_CTF_0,
	.detection_long = {
			DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0,DCC_CTF_0
	},
	.long_packet_end_bit = DCC_CTF_0,
	.endpacket_long = DCC_CTF_1
};

DCC_BasePkt_TypeDef	DCC_BasePkt_Track1[2];
DCC_BasePkt_TypeDef	DCC_WorkPkt_Track1;

DCC_BasePkt_TypeDef	DCC_CutOutPkt_Track1[2];
DCC_BasePkt_TypeDef	DCC_WorkCutOut_Track1;

DCC_BasePkt_TypeDef	DCC_BasePkt_Track2[2];
DCC_BasePkt_TypeDef	DCC_WorkPkt_Track2;

DCC_BasePkt_TypeDef	DCC_CutOutPkt_Track2[2];
DCC_BasePkt_TypeDef	DCC_WorkCutOut_Track2;

static void dcc_TIM_DMADelayPulseHalfCplt(DMA_HandleTypeDef *hdma)
{
//TIM_HandleTypeDef *htim = (TIM_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
	if ( hdma == DCC_Config_Track[DCC_TRACK_1].dma_dcc_handle )
	{
		DCC_Config_Track[DCC_TRACK_1].dcc_system |= SYSTEM_DCC_HALFDONE;
		DCC_Config_Track[DCC_TRACK_1].dcc_system &= ~SYSTEM_DCC_DONE;

		if (( DCC_Config_Track[DCC_TRACK_1].dcc_system & SYSTEM_DCC_WORK_PH) == SYSTEM_DCC_WORK_PH )
		{
			memcpy((uint8_t *)&DCC_BasePkt_Track1[0],(uint8_t *)&DCC_WorkPkt_Track1,sizeof(DCC_WorkPkt_Track1));
			DCC_Config_Track[DCC_TRACK_1].wrk_repetition_counter = DCC_Config_Track[DCC_TRACK_1].repetition_counter;
			DCC_Config_Track[DCC_TRACK_1].dcc_system |= ( SYSTEM_DCC_WORK_PF | SYSTEM_DCC_REP_ACTIVE );
			DCC_Config_Track[DCC_TRACK_1].dcc_system &= ~SYSTEM_DCC_WORK_PH;
			if (( DCC_Config_Track[DCC_TRACK_1].dcc_system & SYSTEM_DCC_EXT_CUTOUT) == SYSTEM_DCC_EXT_CUTOUT )
				memcpy((uint8_t *)&DCC_CutOutPkt_Track1[0],(uint8_t *)&DCC_CutOutExtendedPkt,sizeof(DCC_CutOutExtendedPkt));
			else
				memcpy((uint8_t *)&DCC_CutOutPkt_Track1[0],(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
		}

		if (( DCC_Config_Track[DCC_TRACK_1].dcc_system & SYSTEM_DCC_REP_ACTIVE) == SYSTEM_DCC_REP_ACTIVE )
		{
			if ( DCC_Config_Track[DCC_TRACK_1].wrk_repetition_counter == 0 )
			{
				DCC_Config_Track[DCC_TRACK_1].dcc_system &= ~SYSTEM_DCC_REP_ACTIVE;
				DCC_Config_Track[DCC_TRACK_1].dcc_system |= SYSTEM_DCC_CLEAR;

				DCC_Config_Track[DCC_TRACK_1].wrk_repetition_counter = DCC_Config_Track[DCC_TRACK_1].repetition_counter;
				memcpy((uint8_t *)&DCC_BasePkt_Track1[0],(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_StandardIdle_Pkt));
				memcpy((uint8_t *)&DCC_CutOutPkt_Track1[0],(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
				DCC_Config_Track[DCC_TRACK_1].dcc_system &= ~SYSTEM_DCC_EXT_CUTOUT;
				HAL_GPIO_WritePin(CMD_PEND_GPIO_Port, CMD_PEND_Pin, GPIO_PIN_RESET);
			}
			if ( DCC_Config_Track[DCC_TRACK_1].wrk_repetition_counter > 0 )
				DCC_Config_Track[DCC_TRACK_1].wrk_repetition_counter--;
		}
	}
	if ( hdma == DCC_Config_Track[DCC_TRACK_2].dma_dcc_handle )
	{
		DCC_Config_Track[DCC_TRACK_2].dcc_system |= SYSTEM_DCC_HALFDONE;
		DCC_Config_Track[DCC_TRACK_2].dcc_system &= ~SYSTEM_DCC_DONE;

		if (( DCC_Config_Track[DCC_TRACK_2].dcc_system & SYSTEM_DCC_WORK_PH) == SYSTEM_DCC_WORK_PH )
		{
			memcpy((uint8_t *)&DCC_BasePkt_Track2[0],(uint8_t *)&DCC_WorkPkt_Track2,sizeof(DCC_WorkPkt_Track2));
			DCC_Config_Track[DCC_TRACK_2].wrk_repetition_counter = DCC_Config_Track[DCC_TRACK_2].repetition_counter;
			DCC_Config_Track[DCC_TRACK_2].dcc_system |= ( SYSTEM_DCC_WORK_PF | SYSTEM_DCC_REP_ACTIVE );
			DCC_Config_Track[DCC_TRACK_2].dcc_system &= ~SYSTEM_DCC_WORK_PH;
			if (( DCC_Config_Track[DCC_TRACK_2].dcc_system & SYSTEM_DCC_EXT_CUTOUT) == SYSTEM_DCC_EXT_CUTOUT )
				memcpy((uint8_t *)&DCC_CutOutPkt_Track2[0],(uint8_t *)&DCC_CutOutExtendedPkt,sizeof(DCC_CutOutExtendedPkt));
			else
				memcpy((uint8_t *)&DCC_CutOutPkt_Track2[0],(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
		}

		if (( DCC_Config_Track[DCC_TRACK_2].dcc_system & SYSTEM_DCC_REP_ACTIVE) == SYSTEM_DCC_REP_ACTIVE )
		{
			if ( DCC_Config_Track[DCC_TRACK_2].wrk_repetition_counter == 0 )
			{
				DCC_Config_Track[DCC_TRACK_2].dcc_system &= ~SYSTEM_DCC_REP_ACTIVE;
				DCC_Config_Track[DCC_TRACK_2].dcc_system |= SYSTEM_DCC_CLEAR;

				DCC_Config_Track[DCC_TRACK_2].wrk_repetition_counter = DCC_Config_Track[DCC_TRACK_2].repetition_counter;
				memcpy((uint8_t *)&DCC_BasePkt_Track2[0],(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_StandardIdle_Pkt));
				memcpy((uint8_t *)&DCC_CutOutPkt_Track2[0],(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
				DCC_Config_Track[DCC_TRACK_2].dcc_system &= ~SYSTEM_DCC_EXT_CUTOUT;
				HAL_GPIO_WritePin(CMD_PEND_GPIO_Port, CMD_PEND_Pin, GPIO_PIN_RESET);
			}
			if ( DCC_Config_Track[DCC_TRACK_2].wrk_repetition_counter > 0 )
				DCC_Config_Track[DCC_TRACK_2].wrk_repetition_counter--;
		}
	}
}

static void dcc_TIM_DMADelayPulseCplt(DMA_HandleTypeDef *hdma)
{
//TIM_HandleTypeDef *htim = (TIM_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
	if ( hdma == DCC_Config_Track[DCC_TRACK_1].dma_dcc_handle )
	{
		DCC_Config_Track[DCC_TRACK_1].dcc_system |= SYSTEM_DCC_DONE;
		DCC_Config_Track[DCC_TRACK_1].dcc_system &= ~SYSTEM_DCC_HALFDONE;
		if (( DCC_Config_Track[DCC_TRACK_1].dcc_system & SYSTEM_DCC_WORK_PF) == SYSTEM_DCC_WORK_PF )
		{
			memcpy((uint8_t *)&DCC_BasePkt_Track1[1],(uint8_t *)&DCC_WorkPkt_Track1,sizeof(DCC_WorkPkt_Track1));
			memcpy((uint8_t *)&DCC_CutOutPkt_Track1[1],(uint8_t *)&DCC_WorkCutOut_Track1,sizeof(DCC_WorkPkt_Track1));
			DCC_Config_Track[DCC_TRACK_1].dcc_system &= ~(SYSTEM_DCC_WORK_PF | SYSTEM_DCC_WORK_PH);
		}

		if (( DCC_Config_Track[DCC_TRACK_1].dcc_system & SYSTEM_DCC_CLEAR) == SYSTEM_DCC_CLEAR )
		{
			DCC_Config_Track[DCC_TRACK_1].dcc_system &= ~(SYSTEM_DCC_CLEAR | SYSTEM_DCC_EXT_CUTOUT);
			memcpy((uint8_t *)&DCC_BasePkt_Track1[1],(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_StandardIdle_Pkt));
			memcpy((uint8_t *)&DCC_CutOutPkt_Track1[1],(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
			activate_process(DCC_Config_Track[DCC_TRACK_1].owner,WAKEUP_FROM_TIM_IRQ,WAKEUP_FLAGS_FULL_DMA);
		}
	}
	if ( hdma == DCC_Config_Track[DCC_TRACK_2].dma_dcc_handle )
	{
		DCC_Config_Track[DCC_TRACK_2].dcc_system |= SYSTEM_DCC_DONE;
		DCC_Config_Track[DCC_TRACK_2].dcc_system &= ~SYSTEM_DCC_HALFDONE;
		if (( DCC_Config_Track[DCC_TRACK_2].dcc_system & SYSTEM_DCC_WORK_PF) == SYSTEM_DCC_WORK_PF )
		{
			memcpy((uint8_t *)&DCC_BasePkt_Track2[1],(uint8_t *)&DCC_WorkPkt_Track2,sizeof(DCC_WorkPkt_Track2));
			memcpy((uint8_t *)&DCC_CutOutPkt_Track2[1],(uint8_t *)&DCC_WorkCutOut_Track2,sizeof(DCC_WorkCutOut_Track2));
			DCC_Config_Track[DCC_TRACK_2].dcc_system &= ~(SYSTEM_DCC_WORK_PF | SYSTEM_DCC_WORK_PH);
		}

		if (( DCC_Config_Track[DCC_TRACK_2].dcc_system & SYSTEM_DCC_CLEAR) == SYSTEM_DCC_CLEAR )
		{
			DCC_Config_Track[DCC_TRACK_2].dcc_system &= ~(SYSTEM_DCC_CLEAR | SYSTEM_DCC_EXT_CUTOUT);
			memcpy((uint8_t *)&DCC_BasePkt_Track2[1],(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_StandardIdle_Pkt));
			memcpy((uint8_t *)&DCC_CutOutPkt_Track2[1],(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
			activate_process(DCC_Config_Track[DCC_TRACK_2].owner,WAKEUP_FROM_TIM_IRQ,WAKEUP_FLAGS_FULL_DMA);
		}
	}
}

uint8_t dcc_TIM_PWM_Start_DMA(TIM_HandleTypeDef *htim, uint32_t dcc_channel, uint32_t cutout_channel, uint32_t *prescaler_data, uint32_t *cutout_data, uint16_t Length)
{
uint32_t	dma_dcc_index,dma_cutout_index;

	switch(dcc_channel)
	{
	case TIM_CHANNEL_1 : dma_dcc_index = 1; break;
	case TIM_CHANNEL_2 : dma_dcc_index = 2; break;
	case TIM_CHANNEL_3 : dma_dcc_index = 3; break;
	case TIM_CHANNEL_4 : dma_dcc_index = 4; break;
	}
	switch(cutout_channel)
	{
	case TIM_CHANNEL_1 : dma_cutout_index = 1; break;
	case TIM_CHANNEL_2 : dma_cutout_index = 2; break;
	case TIM_CHANNEL_3 : dma_cutout_index = 3; break;
	case TIM_CHANNEL_4 : dma_cutout_index = 4; break;
	}

	htim->hdma[dma_dcc_index]->XferCpltCallback 		= dcc_TIM_DMADelayPulseCplt;
	htim->hdma[dma_dcc_index]->XferHalfCpltCallback 	= dcc_TIM_DMADelayPulseHalfCplt;

    if (TIM_DMA_Start_IT(htim->hdma[dma_dcc_index],    (uint32_t)prescaler_data, (uint32_t)&htim->Instance->PSC,Length) != HAL_OK)
		return 1;
    if (TIM_DMA_Start_IT(htim->hdma[dma_cutout_index], (uint32_t)cutout_data   , (uint32_t)&htim->Instance->CCR2,Length) != HAL_OK)
		return 1;
	htim->Instance->DIER = ((1<< 10 ) | ( 1 << 9));
	return 0;
}

uint8_t dcc_start(uint8_t track)
{
	if ( DCC_Config_Track[track].hwtimer_handle == NULL )
		return 1;

	DCC_Config_Track[track].hwtimer_handle->Instance->CR1 &= ~TIM_CR1_CEN;
	DCC_Config_Track[track].hwtimer_handle->Instance->CNT = 0;

	HAL_TIM_PWM_Start(DCC_Config_Track[track].hwtimer_handle, DCC_Config_Track[track].dcc_channel);
	HAL_TIMEx_PWMN_Start(DCC_Config_Track[track].hwtimer_handle, DCC_Config_Track[track].dcc_channel);
	HAL_TIM_PWM_Start(DCC_Config_Track[track].hwtimer_handle, DCC_Config_Track[track].cutout_channel);
	if ( track == DCC_TRACK_1 )
		dcc_TIM_PWM_Start_DMA(DCC_Config_Track[track].hwtimer_handle,DCC_Config_Track[track].dcc_channel,DCC_Config_Track[track].cutout_channel,(uint32_t *)&DCC_BasePkt_Track1[0],(uint32_t *)&DCC_CutOutPkt_Track1[0],sizeof(DCC_BasePkt_Track1) );
	if ( track == DCC_TRACK_2 )
		dcc_TIM_PWM_Start_DMA(DCC_Config_Track[track].hwtimer_handle,DCC_Config_Track[track].dcc_channel,DCC_Config_Track[track].cutout_channel,(uint32_t *)&DCC_BasePkt_Track2[0],(uint32_t *)&DCC_CutOutPkt_Track2[0],sizeof(DCC_BasePkt_Track2) );

	return 0;
}


void encode_byte(uint16_t *dest , uint8_t value)
{
uint8_t i,mask=0x80;
	for ( i=0;i<8;i++)
	{
		dest[i] = DCC_0;
		if (( value & mask) == mask)
			dest[i] = DCC_1;
		mask = mask >> 1;
	}
}

void start_dcc(uint8_t track,uint8_t pkt_type)
{
	DCC_Config_Track[track].dcc_system |= SYSTEM_DCC_WORK_PH;
	if ( pkt_type == DCC_PKT_STD)
		DCC_Config_Track[track].dcc_system &= ~SYSTEM_DCC_EXT_CUTOUT;
	else
	{
		DCC_Config_Track[track].dcc_system |= SYSTEM_DCC_EXT_CUTOUT;
	}
	HAL_GPIO_WritePin(CMD_PEND_GPIO_Port, CMD_PEND_Pin, GPIO_PIN_SET);
}

uint8_t dcc_enable_output(uint32_t track)
{
	if ( DCC_Config_Track[track].hwtimer_handle != NULL )
	{
		DCC_Config_Track[track].hwtimer_handle->Instance->CR1 |= TIM_CR1_CEN;
		return 0;
	}
	return 1;
}

uint8_t dcc_disable_output(uint32_t track)
{
	if ( DCC_Config_Track[track].hwtimer_handle != NULL )
	{
		DCC_Config_Track[track].hwtimer_handle->Instance->CR1 &= ~TIM_CR1_CEN;
		return 0;
	}
	return 1;
}

void compile_reset_packet(uint16_t track)
{
	if ( track == DCC_TRACK_1 )
	{
		memcpy((uint8_t *)&DCC_WorkPkt_Track1,(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_ExtendedIdle_Pkt));
		memcpy((uint8_t *)&DCC_WorkCutOut_Track1,(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
		encode_byte((uint16_t *)&DCC_WorkPkt_Track1.address,0);
		encode_byte((uint16_t *)&DCC_WorkPkt_Track1.instruction,0);
		encode_byte((uint16_t *)&DCC_WorkPkt_Track1.detection_short,0);
	}
	if ( track == DCC_TRACK_2 )
	{
		memcpy((uint8_t *)&DCC_WorkPkt_Track2,(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_ExtendedIdle_Pkt));
		memcpy((uint8_t *)&DCC_WorkCutOut_Track2,(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
		encode_byte((uint16_t *)&DCC_WorkPkt_Track2.address,0);
		encode_byte((uint16_t *)&DCC_WorkPkt_Track2.instruction,0);
		encode_byte((uint16_t *)&DCC_WorkPkt_Track2.detection_short,0);
	}
	DCC_Config_Track[track].cmd_sent++;
	start_dcc(track,DCC_PKT_STD);
}

uint8_t	temp_string[128];
uint8_t one_byte_commands(char cmd,uint8_t *reply_buf)
{
uint8_t	ret_val = 0;

	switch ( cmd)
	{
	case 'A' 	: 	sprintf((char *)reply_buf,"Track 1 Main On\n\r");
					DCC_Config_Track[DCC_TRACK_1].dcc_system |= SYSTEM_DCC_POWER;
					dcc_enable_output(DCC_TRACK_1);
					break;
	case 'a' 	: 	sprintf((char *)reply_buf,"Track 1 Main Off\n\r");
					DCC_Config_Track[DCC_TRACK_1].dcc_system &= ~SYSTEM_DCC_POWER;
					dcc_disable_output(DCC_TRACK_1);
					break;
	case 'B' 	: 	sprintf((char *)reply_buf,"Track 2 Main On\n\r");
					DCC_Config_Track[DCC_TRACK_2].dcc_system |= SYSTEM_DCC_POWER;
					dcc_enable_output(DCC_TRACK_2);
					break;
	case 'b' 	: 	sprintf((char *)reply_buf,"Track 2 Main Off\n\r");
					DCC_Config_Track[DCC_TRACK_2].dcc_system &= ~SYSTEM_DCC_POWER;
					dcc_disable_output(DCC_TRACK_2);
					break;
	case 'x' 	: 	sprintf((char *)reply_buf,"All Off\n\r");
					DCC_Config_Track[DCC_TRACK_1].dcc_system &= ~SYSTEM_DCC_POWER;
					DCC_Config_Track[DCC_TRACK_2].dcc_system &= ~SYSTEM_DCC_POWER;
					dcc_disable_output(DCC_TRACK_1);
					dcc_disable_output(DCC_TRACK_2);
					break;
	case 'X' 	: 	sprintf((char *)reply_buf,"All On\n\r");
					DCC_Config_Track[DCC_TRACK_1].dcc_system |= SYSTEM_DCC_POWER;
					DCC_Config_Track[DCC_TRACK_2].dcc_system |= SYSTEM_DCC_POWER;
					dcc_enable_output(DCC_TRACK_1);
					dcc_enable_output(DCC_TRACK_2);
					break;
	case 'R' 	: 	compile_reset_packet(DCC_TRACK_1);
					sprintf((char *)reply_buf,"Sent RESET\n\r");
					break;
	case 'r' 	: 	compile_reset_packet(DCC_TRACK_2);
					sprintf((char *)reply_buf,"Sent RESET\n\r");
					break;
	case 'S' 	: 	sprintf((char *)reply_buf,"Status\n\r");
					if ((DCC_Config_Track[DCC_TRACK_1].dcc_system & SYSTEM_DCC_POWER) == SYSTEM_DCC_POWER)
						strcat((char *)reply_buf,"Track1 ON\n\r");
					else
						strcat((char *)reply_buf,"Track1 OFF\n\r");
					if ((DCC_Config_Track[DCC_TRACK_2].dcc_system & SYSTEM_DCC_POWER) == SYSTEM_DCC_POWER)
						strcat((char *)reply_buf,"Track2 ON\n\r");
					else
						strcat((char *)reply_buf,"Track2 OFF\n\r");
					sprintf((char *)temp_string,"Repetition Track1 %d Track2 %d\n\r",(int )DCC_Config_Track[DCC_TRACK_1].repetition_counter*2,(int )DCC_Config_Track[DCC_TRACK_2].repetition_counter*2);
					strcat((char *)reply_buf,(char *)temp_string);
					break;
	case 's' 	: 	sprintf((char *)reply_buf,"Statistic\n\rCommands sent : Track1 %d Track2 %d\n\r",(int )DCC_Config_Track[DCC_TRACK_1].cmd_sent,(int )DCC_Config_Track[DCC_TRACK_2].cmd_sent);
					break;
	default:		ret_val = 1;
	}
	return ret_val;
}

uint8_t two_bytes_commands(char cmd,uint16_t track,uint16_t address,uint16_t data,uint8_t *reply_buf)
{
uint8_t ecc;
	if ( cmd == 'T' )
	{
		ecc = address ^ data;
		if ( track == DCC_TRACK_1 )
		{
			memcpy((uint8_t *)&DCC_WorkPkt_Track1,(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_ExtendedIdle_Pkt));
			memcpy((uint8_t *)&DCC_WorkCutOut_Track1,(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
			encode_byte((uint16_t *)&DCC_WorkPkt_Track1.address,address);
			encode_byte((uint16_t *)&DCC_WorkPkt_Track1.instruction,data);
			encode_byte((uint16_t *)&DCC_WorkPkt_Track1.detection_short,ecc);
		}
		if ( track == DCC_TRACK_2 )
		{
			memcpy((uint8_t *)&DCC_WorkPkt_Track2,(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_ExtendedIdle_Pkt));
			memcpy((uint8_t *)&DCC_WorkCutOut_Track2,(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
			encode_byte((uint16_t *)&DCC_WorkPkt_Track2.address,address);
			encode_byte((uint16_t *)&DCC_WorkPkt_Track2.instruction,data);
			encode_byte((uint16_t *)&DCC_WorkPkt_Track2.detection_short,ecc);
		}
		DCC_Config_Track[track].cmd_sent++;
		start_dcc(track,DCC_PKT_STD);
		return 0;
	}
	return 1;
}

uint8_t three_bytes_commands(char cmd,uint16_t track,uint16_t address,uint16_t datal,uint16_t datah,uint8_t *reply_buf)
{
uint8_t ecc;
	if ( cmd == 'T' )
	{
		ecc = address ^ datal ^ datah;
		if ( track == DCC_TRACK_1 )
		{
			memcpy((uint8_t *)&DCC_WorkPkt_Track1,(uint8_t *)&DCC_ExtendedIdle_Pkt,sizeof(DCC_StandardIdle_Pkt));
			memcpy((uint8_t *)&DCC_WorkCutOut_Track1,(uint8_t *)&DCC_CutOutExtendedPkt,sizeof(DCC_CutOutExtendedPkt));
			encode_byte((uint16_t *)&DCC_WorkPkt_Track1.address,address);
			encode_byte((uint16_t *)&DCC_WorkPkt_Track1.instruction,datal);
			encode_byte((uint16_t *)&DCC_WorkPkt_Track1.detection_short,datah);
			encode_byte((uint16_t *)&DCC_WorkPkt_Track1.detection_long,ecc);
		}

		if ( track == DCC_TRACK_2 )
		{
			memcpy((uint8_t *)&DCC_WorkPkt_Track2,(uint8_t *)&DCC_ExtendedIdle_Pkt,sizeof(DCC_StandardIdle_Pkt));
			memcpy((uint8_t *)&DCC_WorkCutOut_Track2,(uint8_t *)&DCC_CutOutExtendedPkt,sizeof(DCC_CutOutExtendedPkt));
			encode_byte((uint16_t *)&DCC_WorkPkt_Track2.address,address);
			encode_byte((uint16_t *)&DCC_WorkPkt_Track2.instruction,datal);
			encode_byte((uint16_t *)&DCC_WorkPkt_Track2.detection_short,datah);
			encode_byte((uint16_t *)&DCC_WorkPkt_Track2.detection_long,ecc);
		}
		DCC_Config_Track[track].cmd_sent++;
		start_dcc(track,DCC_PKT_EXTENDED);
		return 0;
	}
	return 1;
}

uint8_t dcc_init(uint32_t hwtimer_id , uint32_t track, uint32_t dcc_channel, uint32_t tx_number , uint32_t cutout_channel,DMA_HandleTypeDef *dma_dcc_handle,DMA_HandleTypeDef *dma_cutout_handle)
{
	if ( track == DCC_TRACK_1 )
	{
		DCC_Config_Track[DCC_TRACK_1].hwtimer_handle = HW_Timers[hwtimer_id].hwtimer_handle;
		DCC_Config_Track[DCC_TRACK_1].dma_dcc_handle = dma_dcc_handle;
		DCC_Config_Track[DCC_TRACK_1].dma_dcc_handle = dma_cutout_handle;
		DCC_Config_Track[DCC_TRACK_1].dcc_channel = dcc_channel;
		DCC_Config_Track[DCC_TRACK_1].cutout_channel = cutout_channel;
		DCC_Config_Track[DCC_TRACK_1].owner = Asys.current_process;
		DCC_Config_Track[DCC_TRACK_1].wrk_repetition_counter = DCC_Config_Track[DCC_TRACK_1].repetition_counter = tx_number;
		DCC_Config_Track[DCC_TRACK_1].cmd_sent=0;

		memcpy((uint8_t *)&DCC_BasePkt_Track1[0],(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_StandardIdle_Pkt));
		memcpy((uint8_t *)&DCC_BasePkt_Track1[1],(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_StandardIdle_Pkt));
		memcpy((uint8_t *)&DCC_CutOutPkt_Track1[0],(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
		memcpy((uint8_t *)&DCC_CutOutPkt_Track1[1],(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
		return 0;
	}

	if ( track == DCC_TRACK_2 )
	{
		DCC_Config_Track[DCC_TRACK_2].hwtimer_handle = HW_Timers[hwtimer_id].hwtimer_handle;
		DCC_Config_Track[DCC_TRACK_2].dma_dcc_handle = dma_dcc_handle;
		DCC_Config_Track[DCC_TRACK_2].dma_dcc_handle = dma_cutout_handle;
		DCC_Config_Track[DCC_TRACK_2].dcc_channel = dcc_channel;
		DCC_Config_Track[DCC_TRACK_2].cutout_channel = cutout_channel;
		DCC_Config_Track[DCC_TRACK_2].owner = Asys.current_process;
		DCC_Config_Track[DCC_TRACK_2].wrk_repetition_counter = DCC_Config_Track[DCC_TRACK_2].repetition_counter = tx_number;
		DCC_Config_Track[DCC_TRACK_2].cmd_sent=0;

		memcpy((uint8_t *)&DCC_BasePkt_Track2[0],(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_StandardIdle_Pkt));
		memcpy((uint8_t *)&DCC_BasePkt_Track2[1],(uint8_t *)&DCC_StandardIdle_Pkt,sizeof(DCC_StandardIdle_Pkt));
		memcpy((uint8_t *)&DCC_CutOutPkt_Track2[0],(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
		memcpy((uint8_t *)&DCC_CutOutPkt_Track2[1],(uint8_t *)&DCC_CutOutStandardPkt,sizeof(DCC_CutOutStandardPkt));
		return 0;
	}

	return 1;
}


#endif // #ifdef DDC_SYSTEM_ENABLE
