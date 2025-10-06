Driver usage

1) The control structure is:
typedef struct
{
	uint8_t				status;
	uint8_t				flags;
	uint8_t				handle;
	uint32_t			wakeup_id;
	TIM_HandleTypeDef 	*encoder_timer;
	GPIO_TypeDef	 	*button_port;
	uint16_t			button_bit;
	uint32_t 			encoder_last_value;
	uint32_t 			encoder_value;
	void				(*irq_encoder_callback)  (uint32_t encoder_value);
}Encoder_Drv_TypeDef;

2) Example:
a - define the control structure and the related handle:
	Encoder_Drv_TypeDef	Encoder_Drv =
	{
			.encoder_timer = &htim1,
	#ifdef USE_CALLBACK
			.irq_encoder_callback = irq_encoder_callback,
	#else
			.wakeup_id = WAKEUP_FROM_TIM_IRQ,
	#endif // #ifdef USE_CALLBACK
	};
	uint32_t encoder_driver_handle;
	
b - register the driver and start it:

	encoder_driver_handle = encoder_register(&Encoder_Drv);

c - Use cases:

	For the callback you need to compile the pointer to the user function, 
		irq_encoder_callback in the example above.
		The code for management mus be inside a timer like below:
		if (( wakeup & EVENT_TIMER) == EVENT_TIMER)
		{
			if (( Encoder_Drv.status & ENCODER_READY) == ENCODER_READY)
			{
				Encoder_Drv.status &= ~ENCODER_READY;
				if (( Encoder_Drv.status & ENCODER_UP) == ENCODER_UP)
					sprintf((char *)uart_tx_buffer,"Callback Encoder : %d UP\n\r",(int )Encoder_Drv.encoder_value);
				else
					sprintf((char *)uart_tx_buffer,"Callback Encoder : %d DOWN\n\r",(int )Encoder_Drv.encoder_value);
				uart_send(uart_driver_handle, uart_tx_buffer,strlen((char * )uart_tx_buffer));
			}
		}
		
	For standard use you can check the event that wakes up the process as below:	
		if (( wakeup & EVENT_TIM_IRQ) == EVENT_TIM_IRQ)
		{
			if (( Encoder_Drv.status & ENCODER_UP) == ENCODER_UP)
				sprintf((char *)uart_tx_buffer,"Event Encoder : %d UP\n\r",(int )Encoder_Drv.encoder_value);
			else
				sprintf((char *)uart_tx_buffer,"Event Encoder : %d DOWN\n\r",(int )Encoder_Drv.encoder_value);
		}	
Notes:
	The appropriate example is in sample_process_1_encoder.c

  
 


