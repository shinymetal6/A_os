Driver description
This driver drives an external servo using timer with no intervention from processor.
The timer emits a number of pulses ( see repetition var in structure ) within a defined cycle time ( see cycle_time var in structure ).

Driver usage

1) The control structure is:
typedef struct
{
	uint8_t					status;
	uint8_t					flags;
	uint8_t					handle;
	uint8_t					repetition;
	uint32_t				cycle_time;
	TIM_HandleTypeDef 		*servo_timer;
	uint32_t 				servo_channel;

}SERVO_Control_DriverStruct_t;

2) Useful defines

#define	SERVO_TIMER			htim16
#define	SERVO_CHANNEL		TIM_CHANNEL_1
#define	SERVO_PULSES		8
extern	TIM_HandleTypeDef 	SERVO_TIMER;

3) Example:
a - define the control structure, the related handle and the data buffer:

SERVO_Control_DriverStruct_t	servo_ch1 =
{
	.servo_timer = &SERVO_TIMER,
	.servo_channel = SERVO_CHANNEL,
	.cycle_time = 20000,
	.repetition = 8,
};
uint32_t		servo_ch1_driver_handle;
	
b - register the driver and start it:

	servo_ch1_driver_handle = servo_register(&servo_ch1);
	servo_start(servo_ch1_driver_handle);

c - when needed generate pulses:

	servo_set_position(servo_ch1_driver_handle,<position>,SERVO_PULSES);
	where <position> is in the range 0 - 100
	
See sample_process_1_servo.c in examples.
		
Notes:

Some servos accept pulses less than 1 mSec. and greater than 2 mSec, notably some SG90.
At now the driver handles standard 1-2 mSec. pulses servos.

There are no flags associated with this driver.
The driver will never wake up the calling process.

An appropriate example is in sample_process_1_servo.c

  
 


