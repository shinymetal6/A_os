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
 * Project : bb1xx_743_00 
*/
/*
 * internal_dac.c
 *
 *  Created on: Jan 3, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"

#ifdef INTERNAL_DAC_ENABLED
#include "../../kernel/scheduler.h"
#include "../../kernel/hwmanager.h"
#include "../../kernel/A_exported_functions.h"
#include "internal_dac.h"

extern	HWMngr_t	HWMngr[PERIPHERAL_NUM];
uint16_t	dac_value=0;
ControlDacDef	ControlDac;

#ifdef STEADY_DAC_VALUE
void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac)
{
	dac_value+=10;
	dac_value &=0xfff;
	HAL_DAC_SetValue(&DAC_HANDLE, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_value);
}
uint8_t IntDac_Start(void)
{
	return HAL_DAC_SetValue(&DAC_HANDLE, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_value);
}
#else

__attribute__ ((aligned (32))) const uint16_t dac_sine_tab[DAC_WAVETABLE_SIZE] =
{
		0x800, 0x832, 0x864, 0x896, 0x8c8, 0x8fa, 0x92c, 0x95e,
		0x98f, 0x9c0, 0x9f1, 0xa22, 0xa52, 0xa82, 0xab1, 0xae0,
		0xb0f, 0xb3d, 0xb6b, 0xb98, 0xbc5, 0xbf1, 0xc1c, 0xc47,
		0xc71, 0xc9a, 0xcc3, 0xceb, 0xd12, 0xd39, 0xd5f, 0xd83,
		0xda7, 0xdca, 0xded, 0xe0e, 0xe2e, 0xe4e, 0xe6c, 0xe8a,
		0xea6, 0xec1, 0xedc, 0xef5, 0xf0d, 0xf24, 0xf3a, 0xf4f,
		0xf63, 0xf76, 0xf87, 0xf98, 0xfa7, 0xfb5, 0xfc2, 0xfcd,
		0xfd8, 0xfe1, 0xfe9, 0xff0, 0xff5, 0xff9, 0xffd, 0xffe,
		0xfff, 0xffe, 0xffd, 0xff9, 0xff5, 0xff0, 0xfe9, 0xfe1,
		0xfd8, 0xfcd, 0xfc2, 0xfb5, 0xfa7, 0xf98, 0xf87, 0xf76,
		0xf63, 0xf4f, 0xf3a, 0xf24, 0xf0d, 0xef5, 0xedc, 0xec1,
		0xea6, 0xe8a, 0xe6c, 0xe4e, 0xe2e, 0xe0e, 0xded, 0xdca,
		0xda7, 0xd83, 0xd5f, 0xd39, 0xd12, 0xceb, 0xcc3, 0xc9a,
		0xc71, 0xc47, 0xc1c, 0xbf1, 0xbc5, 0xb98, 0xb6b, 0xb3d,
		0xb0f, 0xae0, 0xab1, 0xa82, 0xa52, 0xa22, 0x9f1, 0x9c0,
		0x98f, 0x95e, 0x92c, 0x8fa, 0x8c8, 0x896, 0x864, 0x832,
		0x800, 0x7cd, 0x79b, 0x769, 0x737, 0x705, 0x6d3, 0x6a1,
		0x670, 0x63f, 0x60e, 0x5dd, 0x5ad, 0x57d, 0x54e, 0x51f,
		0x4f0, 0x4c2, 0x494, 0x467, 0x43a, 0x40e, 0x3e3, 0x3b8,
		0x38e, 0x365, 0x33c, 0x314, 0x2ed, 0x2c6, 0x2a0, 0x27c,
		0x258, 0x235, 0x212, 0x1f1, 0x1d1, 0x1b1, 0x193, 0x175,
		0x159, 0x13e, 0x123, 0x10a, 0xf2, 0xdb, 0xc5, 0xb0,
		0x9c, 0x89, 0x78, 0x67, 0x58, 0x4a, 0x3d, 0x32,
		0x27, 0x1e, 0x16, 0xf, 0xa, 0x6, 0x2, 0x1,
		0x0, 0x1, 0x2, 0x6, 0xa, 0xf, 0x16, 0x1e,
		0x27, 0x32, 0x3d, 0x4a, 0x58, 0x67, 0x78, 0x89,
		0x9c, 0xb0, 0xc5, 0xdb, 0xf2, 0x10a, 0x123, 0x13e,
		0x159, 0x175, 0x193, 0x1b1, 0x1d1, 0x1f1, 0x212, 0x235,
		0x258, 0x27c, 0x2a0, 0x2c6, 0x2ed, 0x314, 0x33c, 0x365,
		0x38e, 0x3b8, 0x3e3, 0x40e, 0x43a, 0x467, 0x494, 0x4c2,
		0x4f0, 0x51f, 0x54e, 0x57d, 0x5ad, 0x5dd, 0x60e, 0x63f,
		0x670, 0x6a1, 0x6d3, 0x705, 0x737, 0x769, 0x79b, 0x7cd,
};

uint8_t IntDac_Init(uint16_t *user_table)
{
uint32_t	i;

	if ( HWMngr[HW_DAC].process != Asys.current_process )
		return HW_DAC_ERROR_HW_NOT_OWNED;

	if (user_table != NULL )
	{
		ControlDac.user_table = user_table;
		for(i=0;i<DAC_WAVETABLE_SIZE;i++)
			ControlDac.dac_table[i] = ControlDac.user_table[i];
	}
	else
	{
		ControlDac.user_table = NULL;
		for(i=0;i<DAC_WAVETABLE_SIZE;i++)
			ControlDac.dac_table[i] = dac_sine_tab[i];
	}
	if ( HAL_DAC_Start_DMA(&DAC_HANDLE, DAC_CHANNEL_1, (uint32_t *)ControlDac.dac_table, DAC_WAVETABLE_SIZE,DAC_ALIGN_12B_R) == 0 )
		return HW_DAC_ERROR_NONE;
	return HW_DAC_ERROR_INIT;
}


uint8_t IntDac_Start(uint32_t wakeup_at_cycle_end)
{
	if ( HWMngr[HW_DAC].process != Asys.current_process )
		return HW_DAC_ERROR_HW_NOT_OWNED;
	if ((ControlDac.dac_flag & DAC_LOCKED) == DAC_LOCKED)
		return HW_DAC_ERROR_HW_NOT_OWNED;
	ControlDac.dac_flag = DAC_LOCKED;
	if ( wakeup_at_cycle_end )
		ControlDac.dac_flag |= DAC_WAKEUP_AT_CYCLE;
	HAL_TIM_Base_Start(&DAC_TIMER);
	return HW_DAC_ERROR_NONE;
}

uint8_t IntDac_2sequence_Start(uint32_t cycle_count_table,uint32_t cycle_count_value , uint32_t output_value, uint32_t three_state_at_end)
{
	if ( HWMngr[HW_DAC].process != Asys.current_process )
		return HW_DAC_ERROR_HW_NOT_OWNED;
	if ((ControlDac.dac_flag & DAC_LOCKED) == DAC_LOCKED)
		return HW_DAC_ERROR_HW_NOT_OWNED;

	ControlDac.cycle_count_table = cycle_count_table;
	ControlDac.cycle_count_value = cycle_count_value;
	ControlDac.output_value = output_value;
	ControlDac.dac_out_cntr = 0;
	ControlDac.dac_flag = DAC_TWO_SEQUENCE | DAC_LOCKED | DAC_TABLE1_OUT;
	if ( three_state_at_end )
		ControlDac.dac_flag |= DAC_3ST_AT_END;
	HAL_TIM_Base_Start(&DAC_TIMER);
	return HW_DAC_ERROR_NONE;
}

uint8_t IntDac_Stop(void)
{
	if ( HWMngr[HW_DAC].process != Asys.current_process )
		return HW_DAC_ERROR_HW_NOT_OWNED;
	HAL_TIM_Base_Stop(&DAC_TIMER);
	ControlDac.dac_flag = 0;
	ControlDac.dac_out_cntr = 0;
	return HW_DAC_ERROR_NONE;
}

uint16_t *IntDac_Get_SineTab(void)
{
	return (uint16_t *)dac_sine_tab;
}

uint16_t *IntDac_Current_Tab(void)
{
	return (uint16_t *)ControlDac.dac_table;
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac)
{
uint16_t	i;
	ControlDac.dac_out_cntr++;
	if ( ControlDac.dac_flag & DAC_TWO_SEQUENCE)
	{
		if ( ControlDac.dac_flag & DAC_TABLE1_OUT )
		{
			if ( ControlDac.dac_out_cntr >= ControlDac.cycle_count_table)
			{
				for(i=0;i<DAC_WAVETABLE_SIZE;i++)
					ControlDac.dac_table[i] = ControlDac.output_value;
				activate_process(HWMngr[HW_DAC].process,EVENT_DAC_IRQ,ControlDac.cycle_count_table);
				ControlDac.dac_flag &= ~DAC_TABLE1_OUT;
				ControlDac.dac_flag |=  DAC_TABLE2_OUT;
				ControlDac.dac_out_cntr = 0;
			}
		}
		else
		{
			if ( ControlDac.dac_out_cntr >= ControlDac.cycle_count_value )
			{
				HAL_TIM_Base_Stop(&DAC_TIMER);
				activate_process(HWMngr[HW_DAC].process,EVENT_DAC_IRQ,ControlDac.cycle_count_value);
				if ( ControlDac.dac_flag & DAC_3ST_AT_END )
				{

				}
				ControlDac.dac_flag = 0;
				ControlDac.dac_out_cntr = 0;
			}
		}
	}
	else
	{
		activate_process(HWMngr[HW_DAC].process,EVENT_DAC_IRQ,0xffffffff);
	}
}

#endif // #ifdef STEADY_DAC_VALUE

#endif // #ifdef DAC_ENABLED

