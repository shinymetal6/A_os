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
	GPIO_TypeDef	 	*dir_port;
	uint16_t			dir_bit;
	uint32_t 			prescaler;
	uint32_t 			period;
	uint32_t 			pulse_width;
	uint8_t				number_of_steps;
}Stepper_Control_TypeDef;

2) Example:
a - define the control structure and the related handle:

Stepper_Control_TypeDef	Stepper_Control =
{
		.timer = &htim16,
		.dir_port = STEP_DIR_GPIO_Port,
		.dir_bit = STEP_DIR_Pin,
		.pulse_width = STEPPER_DEFAULT_PW,
};
	
b - register the driver and start it:

	stepper_register(&Stepper_Control);
	stepper_init(&Stepper_Control);
	stepper_start(&Stepper_Control,TIM_CHANNEL_1,11);

c - when needed stop it:

	stepper_stop(&Stepper_Control,TIM_CHANNEL_1);	
			
Notes:

Set the "one pulse mode" in initialization
Use the "PWM Mode 2" to avoid the spike at the end of period
The driver subtract 1 to the value of the number of pulses at the stepper_start call
There are no flags associated with this driver.

The driver will never wake up the calling process.
  
 


