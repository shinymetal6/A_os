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
 * internal_dac.h
 *
 *  Created on: Jan 3, 2024
 *      Author: fil
 */

#ifndef DRIVERS_INTERNAL_DAC_INTERNAL_DAC_H_
#define DRIVERS_INTERNAL_DAC_INTERNAL_DAC_H_

#define HW_DAC_ERROR_NONE			0
#define HW_DAC_ERROR_HW_NOT_OWNED	1
#define HW_DAC_ERROR_INIT			2

typedef struct _ControlDacDef
{
	uint32_t 	dac_out_cntr;
	uint32_t 	cycle_count_table;
	uint32_t 	cycle_count_value;
	uint32_t 	output_value;
	uint8_t 	dac_flag;
	uint16_t 	*user_table;
	__attribute__ ((aligned (32))) int16_t dac_table[DAC_WAVETABLE_SIZE];
}ControlDacDef;
/* dac_flag */

#define	DAC_TABLE1_OUT		0x01
#define	DAC_TABLE2_OUT		0x02
#define	DAC_3ST_AT_END		0x10
#define	DAC_WAKEUP_AT_CYCLE	0x20
#define	DAC_LOCKED			0x40
#define	DAC_TWO_SEQUENCE	0x80

extern	uint8_t IntDac_Init(uint16_t *user_table);
extern	uint8_t IntDac_Start(uint32_t wakeup_at_cycle_end);
extern	uint8_t IntDac_2sequence_Start(uint32_t cycle_count_table,uint32_t cycle_count_value , uint32_t output_value, uint32_t three_state_at_end);
extern	uint8_t IntDac_Stop(void);
extern	uint16_t *IntDac_Get_SineTab(void);
extern	uint16_t *IntDac_Current_Tab(void);

#endif /* DRIVERS_INTERNAL_DAC_INTERNAL_DAC_H_ */
