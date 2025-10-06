Driver usage

1) The control structure is:
typedef struct
{
	uint8_t					status;
	uint8_t					flags;
	uint8_t					handle;
	TIM_HandleTypeDef 		*ws2812_timer;
	uint32_t 				ws2812_channel;
}WS2812_Drv_TypeDef;

2) Example:
a - define the control structure and the related handle:
	WS2812_Drv_TypeDef	WS2812_Drv =
	{
			.ws2812_timer = &htim1,
			.ws2812_channel = TIM_CHANNEL_1,
			.wakeup_id = WAKEUP_FROM_TIM_IRQ,
	};
	uint32_t WS2812_driver_handle;
	
b - register the driver and start it:

	WS2812_driver_handle = ws2812_register(&WS2812_Drv);

c - Use cases:

	To set the pixel color call ws2812_CompilePixel with the appropriate parameters.

  
 


