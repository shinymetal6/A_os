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

#define	DAC_MAX_TABLE_SIZE			1024
typedef struct _ControlDacDef
{
	uint32_t 	dac_out_cntr;
	uint32_t 	wakeup_cycle_count;
	uint32_t 	wakeup_cycle_count_counter;
	uint8_t 	dac_flag;
	uint16_t 	*user_table;
	uint16_t 	dac_user_table_size;
	__attribute__ ((aligned (32))) int16_t dac_table[DAC_MAX_TABLE_SIZE];
}ControlDacDef;
/* dac_flag */

#define	DAC_STOP_AT_END		0x01
#define	DAC_3ST_AT_END		0x02
#define	DAC_PU_AT_END		0x04
#define	DAC_PD_AT_END		0x08
#define	DAC_WAKEUP_AT_CYCLE	0x10
// not used					0x20
#define	DAC_LOCKED			0x40
#define	DAC_RUNNING			0x80


#define	DAC_IS_3ST				0x11
#define	DAC_IS_RUNNING			0x10
#define	DAC_PULLED_UP			0x01
#define	DAC_PULLED_DOWN			0x10
#define	DAC_NO_PULL				0x00

#define	DAC_PORT_PIN			4

extern	uint8_t IntDac_Init(uint16_t *user_table,uint16_t dac_user_table_size);
extern	uint8_t IntDac_Start(uint32_t wakeup_cycle_count,uint8_t dac_flags);
//extern	uint8_t IntDac_2sequence_Start(uint32_t cycle_count_table,uint32_t cycle_count_value , uint32_t output_value, uint32_t three_state_at_end);
extern	uint8_t IntDac_Stop(void);
extern	uint16_t *IntDac_Current_Tab(void);
extern	uint8_t IntDac_UpdateCurrentTab(uint16_t *user_table,uint16_t dac_user_table_size);

#ifdef PROVIDE_SAMPLE_DAC_SINE
extern	uint16_t *IntDac_Get_SineTab(void);
#endif // #ifdef PROVIDE_SAMPLE_DAC_SINE

#endif /* DRIVERS_INTERNAL_DAC_INTERNAL_DAC_H_ */
