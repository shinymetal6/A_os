Driver usage

1) The control structure is:
typedef struct
{
	/* timer header */
	uint8_t 			process;
	uint8_t				status;
	uint8_t				flags;
	uint32_t 			*next_timer;
	TIM_HandleTypeDef 	*timer;
	uint8_t				timer_type;
	/* timer internals */
	uint8_t				handle;
	uint32_t 			pwm_channel;
	GPIO_TypeDef	 	*enable_port;
	uint16_t			enable_bit;
	uint32_t 			prescaler;
	uint32_t 			period;
	uint32_t 			pulse_width[6];
	uint8_t 			pwm_direction;
}Pwm_Control_TypeDef;

2) Example:
a - define the control structure and the related handle:

Pwm_Control_TypeDef	Pwm_Control =
{
		.pwm_timer = &htim4,
		.pwm_channel = TIM_CHANNEL_4,
		.pulse_width = 1000,
};
	
b - register the driver and start it:

	pwm_register(&Pwm_Control);
	pwm_init(&Pwm_Control);
	pwm_start(&Pwm_Control);

c - when needed stop it:

	pwm_stop(&Pwm_Control);	
			
Notes:

There are no flags associated with this driver.

The driver will never wake up the calling process.

The pictures from Timer_Settings_1.png to Timer_Settings_3.png gives an example on how to set the timer 1 for dcc.
  
 


