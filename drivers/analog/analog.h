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
 * analog.h
 *
 *  Created on: Nov 12, 2024
 *      Author: fil
 */

#ifndef DRIVERS_ANALOG_ANALOG_H_
#define DRIVERS_ANALOG_ANALOG_H_

#ifdef A_OS_ADC_ENABLED
#include "adc/adc_driver.h"
#endif

#ifdef A_OS_DAC_ENABLED
#include "dac/dac_driver.h"
#endif

#ifdef A_OS_I2S_ENABLED
#include "i2s/i2s_driver.h"
#endif // #ifdef A_OS_I2S_ENABLED

#endif /* DRIVERS_ANALOG_ANALOG_H_ */
