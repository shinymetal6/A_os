Driver usage

1) The control structure is:
typedef struct
{
	uint8_t 				process;
	uint8_t					status;
	uint8_t					flags;
	uint32_t 				wakeup_id;
	uint32_t 				*next_dcc;
	uint8_t					command_repeat_number;
	uint8_t					command_repeat_counter;
	uint32_t				dma_dcc_value;
	uint32_t				dma_cutout_value;
	uint32_t				dma_dcc_index;
	uint32_t				dma_cutout_index;
	TIM_HandleTypeDef 		*dcc_timer;
	DMA_HandleTypeDef 		*hdma[2];
	uint32_t				timer_180phase_dcc_channel;
	uint32_t				timer_dcc_channel;
	uint32_t				timer_cutout_channel;
	GPIO_TypeDef	 		*enable_port;
	uint16_t				enable_bit;
	DCC_Drv_Pkt_TypeDef		DCC_Pkt[2];
	DCC_Drv_Pkt_TypeDef		DCC_Cutout_Pkt[2];
	DCC_Drv_Pkt_TypeDef		DCC_WorkPkt;
}DCC_DriverStruct_t;

2) Example:
a - define the control structure and the related handle:

DCC_DriverStruct_t	DCC_Control =
{
	.dcc_timer = &htim1,
	.timer_dcc_channel = TIM_CHANNEL_3,
	.timer_cutout_channel = TIM_CHANNEL_4,
	.enable_port = DCC_ENABLE_GPIO_Port,
	.enable_bit = DCC_ENABLE_Pin,
};
	
b - register the driver and start it:

	dcc_register(&DCC_Control);
	dcc_start(&DCC_Control);	

c - when needed stop it:

	dcc_stop(&DCC_Control);	
			
Notes:

The only flags bit is DCC_TIMER_DUAL_PHASE and enables the dual phase output of the pwm. In this case the 
 timer_180phase_dcc_channel must be specified also.

The enable_port and enable_bit are used to enable the power on the dcc line.

The driver will never wake up the calling process.

The pictures from DCC_Timer_Settings_1.png to DCC_Timer_Settings_5.png gives an example on how to set the timer 1 for dcc.

An appropriate example is in sample_process_1_dccpwm.c
  
 


