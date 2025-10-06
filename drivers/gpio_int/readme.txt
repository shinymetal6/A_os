Driver usage

1) The control structure is:
typedef struct
{
	uint8_t				status;
	uint8_t 			process;
	uint16_t			IRQ_bit;
	GPIO_TypeDef	 	*IRQ_port;
	uint16_t			IRQ_type;
	uint16_t			sampled_bit;
	uint32_t			wakeup_id;
	uint8_t				debounce;
	uint8_t				debounce_counter;
	void				(*irq_exti_callback)  (uint16_t GPIO_Pin);
	uint8_t				flags;
}GPIO_Interrupt_DriverStruct_t;

2) Example:
a - define the control structure and the related handle:
	GPIO_Interrupt_DriverStruct_t Button_Driver =
	{
		.IRQ_bit = ENC_BTN_Pin,
		.IRQ_port = ENC_BTN_GPIO_Port,
		.IRQ_type = GPIO_INT_TYPE_FALLING,
		.debounce = 100,
		.flags = GPIO_INT_WAKEUP_ON_EVENT,
	#ifdef USE_CALLBACK
		.irq_exti_callback = irq_encoderbtn_callback,
	#else
		.wakeup_id = WAKEUP_FROM_EXT_INT_IRQ,
	#endif // #ifdef USE_CALLBACK
	};
	uint32_t button_driver_handle;
	
b - register the driver and start it:

	button_driver_handle = gpio_int_register(&Button_Driver);

c - Use cases:

	For the callback you need to compile the pointer to the user function, 
		irq_encoderbtn_callback in the example above.
		The code for management mus be inside a timer like below:
		if (( wakeup & EVENT_TIMER) == EVENT_TIMER)
		{
			if (( Button_Driver.status & GPIO_INT_EVENT) == GPIO_INT_EVENT)
			{
				Button_Driver.status &= ~GPIO_INT_EVENT;
				sprintf((char *)uart_tx_buffer,"Callback Button\n\r");
				uart_send(uart_driver_handle, uart_tx_buffer,strlen((char * )uart_tx_buffer));
			}
		}
		
	For standard use you can check the event that wakes up the process as below:	
		if (( wakeup & WAKEUP_FROM_EXT_INT_IRQ) == WAKEUP_FROM_EXT_INT_IRQ)
		{
			sprintf((char *)uart_tx_buffer,"Event Encoder Button Pressed\n\r");
			uart_send(uart_driver_handle, uart_tx_buffer,strlen((char * )uart_tx_buffer));
		}
Notes:
	An appropriate example is in sample_process_1_encoder.c

  
 


