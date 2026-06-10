Driver usage

1) The control structure is:
typedef struct
{
	uint8_t				status;
	uint8_t				flags;
	uint8_t				rx_char;
	uint16_t			rx_index;
	uint16_t			rx_max_len;
	uint16_t			rx_num_chars;
	uint8_t				*data;
	uint16_t			timeout;
	uint16_t			timeout_reload_value;
	uint8_t				sentinel_start;
	uint8_t				sentinel_end;
	uint8_t				sentinel_flags;
	uint32_t			uart_error;
	UART_HandleTypeDef 	*uart;
	GPIO_TypeDef	 	*tx_port;
	uint16_t			tx_bit;
	GPIO_TypeDef	 	*rx_port;
	uint16_t			rx_bit;
	uint32_t			param0;
	uint32_t 			wakeup_id;
}UART_DriverStruct_t;

2) Example:
a - define the data buffer, control structure and the related handle:

	#define	UART_RX_BUF_SIZE	512
	#define	UART_TX_BUF_SIZE	512
	uint8_t	uart3_rx_buffer[UART_RX_BUF_SIZE];
	uint8_t	uart3_tx_buffer[UART_TX_BUF_SIZE];
	
	UART_DriverStruct_t Uart3_Drv =
	{
		.data = uart3_rx_buffer,
		.rx_max_len = XMODEM_LINE_LEN,
		.uart = &huart3,
		.wakeup_id = WAKEUP_FROM_UART3_IRQ,
		.timeout = 250,
		.flags = UART_USES_DMA_TX | UART_USES_DMA_RX | UART_WAKEUP_ON_RXFULL | UART_WAKEUP_ON_TIMEOUT,
	};
	uint32_t	uart3_driver_handle;
	
b - register the driver and start it:

	uart3_driver_handle = uart_register(&Uart3_Drv);
	uart_start_receive(uart3_driver_handle);
	
c - transmit

	uart_send(uart3_driver_handle,buffer,buffer_len);
			
Notes:

The flags field is optional, defaults to WAKEUP_ON_RXFULL and wakes up on interupt.
There is no polling use for uart.
The flags field can be a or combination of:
	UART_USES_DMA_TX			
	UART_USES_DMA_RX			
	UART_WAKEUP_ON_TX			
	UART_WAKEUP_ON_RXCHAR		
	WAKEUP_ON_RXFULL		
	UART_WAKEUP_ON_TIMEOUT		
	UART_WAKEUP_ON_ERRORS		
	
The wakeup_id field is the way A_os has to wake up the process, and is used to identify the uart. Mandatory.

The timeout field is the timeout after which even if the packet len has not reached the required len specified 
 in rx_max_len field the driver will wake up the process. A 0 timeout means no timeout.
 
 Specifying the fields sentinel_start and / or sentinel_end will start reception after the sentinel_start 
  char is received and stop reception and wake up the calling process when sentinel_end is received.
  Specifyng only sentinel_start means the store op of the rxed chars start when the rxed char is equal to 
  sentinel_start and stops after rx_max_len chars are receiveds or a timeout occourred.
  
 There is a configurable depth queue in the uart management, so the user can que more than 1 message on a uart.
 The queue is a static array, the depth is configured as #define MAX_UARTS_QUEUE 8   in A.h 
 
 Remember to enable uart global irqs, as this initialization is not intended for circular buffers
  
  There are several examples in SampleProcesses
 


