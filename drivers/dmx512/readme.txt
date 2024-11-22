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
	uint32_t			break_length;
	uint32_t 			wakeup_id;
}DMX512_Drv_TypeDef;

2) Example:
a - define the data buffer, control structure and the related handle:

	#define	DMX512_BUF_SIZE		512
	uint8_t	dmx512_tx_buffer[DMX512_BUF_SIZE];
	
	DMX512_Drv_TypeDef dmx512_Drv =
	{
		.uart = &huart2,
		.tx_port = GPIOA,
		.tx_bit = 2,
		.wakeup_id = WAKEUP_FROM_UART2_IRQ,
		.flags = UART_USES_DMA_TX,
		.break_length = 120,
	};
	uint32_t		dmx512_driver_handle;

	
b - register the driver:

	dmx512_driver_handle = dmx512_register(&dmx512_Drv);

c - transmit

	dmx512_send(dmx512_driver_handle,dmx512_tx_buffer,DMX512_BUF_SIZE);
			
Notes:
The fields uart,tx_port and tx_bit depends on hw configuration, tx_port and tx_bit are related to the uart used. Mandatory.

The flags field is equivalent to the uart one, so the same flags can be set. Optional, defaults to WAKEUP_ON_RXFULL.:
The flags field can be a or combination of:
	UART_USES_DMA_TX			
	UART_USES_DMA_RX			
	UART_WAKEUP_ON_TX			
	UART_WAKEUP_ON_RXCHAR		
	WAKEUP_ON_RXFULL		
	UART_WAKEUP_ON_TIMEOUT		
	UART_WAKEUP_ON_ERRORS		
	
The wakeup_id field is the way A_os has to wake up the process, and is used to identify the uart connected 
 to the dmx512 driver. Mandatory.

The break_length field is used to define the length of the initial dmx512 break expressed in microsecond. Mandatory.



