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
 * gpio.h
 *
 *  Created on: Jan 3, 2024
 *      Author: fil
 */

#ifndef DRIVERS_GPIO_UTLS_GPIO_H_
#define DRIVERS_GPIO_UTLS_GPIO_H_

#define	PULL_DOWN		0
#define	PULL_UP			1
#define	PULL_NONE		2

#define	SPEED_LOW		0
#define	SPEED_MEDIUM	1
#define	SPEED_HIGH		2

extern void GPIO_SetGpioOUT(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
extern void GPIO_SetGpioIN(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint16_t pullup);
extern void GPIO_SetGpioAlternate(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
extern void GPIO_SetPUPDR(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint16_t pullup);
extern void GPIO_SetOSPEEDR(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint16_t speed);

#endif /* DRIVERS_GPIO_UTLS_GPIO_H_ */
