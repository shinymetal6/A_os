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
	GPIO_TypeDef	 	*tim_port;
	uint16_t			tim_bit;
	GPIO_TypeDef	 	*dir_port;
	uint16_t			dir_bit;
	uint32_t 			prescaler;
	uint32_t 			period;
	uint32_t 			pulse_width;
	uint32_t			number_of_steps;
	uint32_t			stored_number_of_steps;
	uint32_t			steps_per_rotation;
	uint32_t			number_of_rotation;
	void				(*stepper_callback)  (uint32_t stepper_value);
}Stepper_Control_DriverStruct_t;

2) Example:
a - define the control structure and the related handle for tim16:


Stepper_Control_DriverStruct_t	Stepper_Control =
{
		.timer = &htim16,
		.tim_port = STEP_TIM_16_CH1_GPIO_Port,
		.tim_bit = STEP_TIM_16_CH1_Pin,
		.dir_port = STEP_DIR_GPIO_Port,
		.dir_bit = STEP_DIR_Pin,
		.pulse_width = STEPPER_DEFAULT_PW,
		.steps_per_rotation = 200,	//e.g. Nema motors
		.stepper_callback = stepper_callback,
};

b - register the driver and start it:

	#define	STEPPER_FORWARD		1
	#define	STEPPER_REVERSE		0

	stepper_register(&Stepper_Control);
	stepper_init(&Stepper_Control);
	stepper_start(&Stepper_Control,TIM_CHANNEL_1,2,STEPPER_FORWARD); // do 2 rotation @Stepper_Control.steps_per_rotation , total 400 pulses FORWARD

	
c - when needed stop it:

	stepper_stop(&Stepper_Control,TIM_CHANNEL_1);	
			
Notes:

Use the "PWM Mode 2" to avoid the spike at the end of period
There are no flags associated with this driver.
The function pointed at .stepper_callback will be called at the end of the pulses needed to complete the rotations defined in the stepper_start call.
If not needed it can be left uninitialized.

The driver will never wake up the calling process through signals, the callback is the only indication the rotations have been done.
  
 


