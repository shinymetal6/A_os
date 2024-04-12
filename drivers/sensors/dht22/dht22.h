


//#include "stm32h7xx_hal.h"
#include <stdbool.h>
#include <string.h>
#include <math.h>

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
	GPIO_OSPEEDR_Very_high_speed  = 0b10,
}GPIO_OSPEEDR;

//*** Functions prototypes ***//
//REgister Pin
void GPIO_SetMODER(GPIO_TypeDef *gpio, int pin_iDX, GPIO_MODER mode);
void GPIO_SetPUPDR(GPIO_TypeDef *gpio, int pin_iDX, GPIO_PUPDR  mode);
void GPIO_SetOSPEEDR(GPIO_TypeDef *gpio, int pin_iDX, GPIO_OSPEEDR  mode);


//OneWire Initialise
void DHT22_Init(GPIO_TypeDef* DataPort, uint16_t DataPin);
//Change pin mode
void ONE_WIRE_PinMode(OnePinMode_Typedef mode);
//One Wire pin HIGH/LOW Write
void ONE_WIRE_Pin_Write(bool state);
bool ONE_WIRE_Pin_Read(void);
//Microsecond delay
//static void DelayMicroSeconds(uint32_t uSec);
//Begin function
void DHT22_StartAcquisition(void);
//Read 5 bytes
void DHT22_ReadRaw(uint8_t *data);

//Get Temperature and Humidity data
uint8_t DHT22_GetTemp_Humidity(float *Temp, float *Humidity);

