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
	uint32_t 			timer_channel;
	GPIO_TypeDef	 	*tim_port;
	uint16_t			tim_bit;
	GPIO_TypeDef	 	*dir_port;
	uint16_t			dir_bit;
	GPIO_TypeDef	 	*enable_port;
	uint16_t			enable_bit;
	uint32_t 			prescaler;
	uint32_t 			period;
	uint32_t 			pulse_width;
	uint32_t			number_of_steps;
	uint32_t			stored_number_of_steps;
	uint32_t			steps_correction_factor;
	uint32_t			steps_per_rotation;
	uint32_t			number_of_rotation;
	void				(*stepper_callback)  (uint32_t param);
}Stepper_Control_DriverStruct_t;

2) Example:
a - define the control structure and the related handle for tim16:

Stepper_Control_DriverStruct_t	Stepper_Control =
{
		.timer = &htim16,
		.timer_channel = TIM_CHANNEL_1,
		.tim_port = PERI_STEP_TIM16CH1_GPIO_Port,
		.tim_bit = PERI_STEP_TIM16CH1_Pin,
		.dir_port = PERI_DIR_GPIO_Port,
		.dir_bit = PERI_DIR_Pin,
		.enable_port = SLEEP_3G_GPIO_Port,
		.enable_bit = SLEEP_3G_Pin,
		.pulse_width = STEPPER_DEFAULT_PW,
		.prescaler = 480,
		.steps_per_rotation = 200,
		.stepper_callback = stepper_callback,
};

b - register the driver and start it:

	#define	STEPPER_FORWARD		1
	#define	STEPPER_REVERSE		0
	#define	STEPPER_NUMBER_OF_ROTATION		2

	stepper_register(&Stepper_Control);
	stepper_init(&Stepper_Control);
	stepper_start(&Stepper_Control,TIM_CHANNEL_1,STEPPER_NUMBER_OF_ROTATION,STEPPER_FORWARD); // do STEPPER_NUMBER_OF_ROTATION rotation @Stepper_Control.steps_per_rotation , total 400 pulses FORWARD

	
c - when needed stop it:

	stepper_stop(&Stepper_Control,TIM_CHANNEL_1);	
			
Notes:
Passing 0 for rotation parameter to stepper_start will run the stepper forever
Use the "PWM Mode 2" to avoid the spike at the end of period
There are no flags associated with this driver.
The function pointed at .stepper_callback will be called at the end of the pulses needed to complete the rotations defined in the stepper_start call.
If not needed it can be left uninitialized.

The driver will never wake up the calling process through signals, the callback is the only indication the rotations have been done.
  
 


