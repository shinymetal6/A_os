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
	uint8_t 	dac_out_cntr;
	uint8_t 	dac_level0;
	uint8_t 	dac_level1;
	uint8_t 	dac_level_done;
	uint16_t	dac_measure_value;
	uint8_t 	dac_flag;
	uint16_t 	*user_table;
}ControlDacDef;

extern	uint8_t IntDac_Init(uint16_t *user_table);
extern	uint8_t IntDac_Start(uint8_t dac_level0,uint8_t dac_level1 , uint8_t dac_level_done, uint16_t dac_measure_value);
extern	uint8_t IntDac_Stop(void);
extern	uint16_t *IntDac_Get_SineTab(void);

#endif /* DRIVERS_INTERNAL_DAC_INTERNAL_DAC_H_ */
