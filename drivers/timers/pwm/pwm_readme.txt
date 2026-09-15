Note : the pwm driver initializes a complete timer, and the channel used is based
 on the fact that the appropriated pwm_width filed is not 0.
The field pwm_width is a 6 uint32_t array, where the index 0 references to timer 1
 
structDriver usage

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
	uint32_t			wakeup_id;
	/* timer internals */
	uint32_t 			prescaler;
	uint32_t 			period;
	uint32_t 			pulse_width[6];
}Pwm_Control_DriverStruct_t;

2) Example:
a - define the control structure and the related handle:

Pwm_Control_DriverStruct_t	Pwm_Control =
{
		.timer = &htim4,
		.pulse_width = {1000,1000,0,0,0,0},
};
Here channel 1 and 2 of htim4 are initialized
	
b - register the driver and start it:

	pwm_register(&Pwm_Control);
	pwm_init(&Pwm_Control);
	pwm_start_all_enabled(&Pwm_Control); // starts channel 1 and channel 2, all the other remain stopped

c - when needed stop it:

	pwm_stop_all_enabled(&Pwm_Control);	
			
Notes:

There are no flags associated with this driver.

The driver will never wake up the calling process, the wakeup_id is here to maintain 
 compatibility with other timer modes.

The pictures from Timer_Settings_1.png to Timer_Settings_3.png gives an example on
 how to set the timer 1 for dcc.
  
The function pwm_start_all_enabled starts all enabled channels.
The function pwm_start starts the channel passed as parameter.
The functions pwm_stop_all_enabled and pwm_stop similarly stop the channels.

 


