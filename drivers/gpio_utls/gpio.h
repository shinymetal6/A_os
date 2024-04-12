/*
 * gpio.h
 *
 *  Created on: Apr 12, 2024
 *      Author: Luigi F
 */

#ifndef DRIVERS_GPIO_UTLS_GPIO_H_
#define DRIVERS_GPIO_UTLS_GPIO_H_

//Pin Mode enum
typedef enum
{
	ONE_OUTPUT = 0,
	ONE_INPUT,
}OnePinMode_Typedef;

typedef enum
{
    GPIO_MODER_INPUT     = 0b00,
    GPIO_MODER_OUTPUT    = 0b01,
    GPIO_MODER_ALTERNATE = 0b10,
    GPIO_MODER_ANALOG    = 0b11,
}GPIO_MODER;


typedef enum
{
	GPIO_PUPDR_NPULLUP   = 0b00,       /*!< No Pull-up or Pull-down activation  */
	GPIO_PUPDR_PULLUP    = 0b01,       /*!< Pull-up activation                  */
	GPIO_PUPDR_PULLDOWN  = 0b10,       /*!< Pull-down activation                */
}GPIO_PUPDR;


typedef enum
{
	GPIO_OSPEEDR_Low_speed        = 0b00,
	GPIO_OSPEEDR_Medium_speed     = 0b01,
	GPIO_OSPEEDR_High_speed       = 0b10,
	GPIO_OSPEEDR_Very_high_speed  = 0b11,
}GPIO_OSPEEDR;



void GPIO_SetMODER(GPIO_TypeDef *gpio,  uint16_t DataPin, GPIO_MODER mode);
void GPIO_SetPUPDR(GPIO_TypeDef *gpio,  uint16_t DataPin, GPIO_PUPDR mode);
void GPIO_SetOSPEEDR(GPIO_TypeDef *gpio,  uint16_t DataPin, GPIO_OSPEEDR mode);


#endif /* DRIVERS_GPIO_UTLS_GPIO_H_ */
