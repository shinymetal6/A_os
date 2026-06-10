Driver usage

1) The control structure is:
typedef struct
{
	uint8_t				status;
	uint8_t				flags;
	uint16_t			rx_num_chars;
	uint16_t			requested_len;
	uint16_t			data_index;
	uint8_t				*data;
	uint16_t			timeout;
	uint16_t			timeout_reload_value;
	uint16_t			usb_interface_class;
	void				(*Rx_CallbackPtr)(uint8_t* buf, uint16_t len);
	uint32_t 			wakeup_id;
}USB_DriverStruct_t;

2) Example:
a - define the data buffer, control structure and the related handle:

	#define	USB_RX_BUF_SIZE	256
	#define	USB_TX_BUF_SIZE	256
	uint8_t	usb_rx_buffer[USB_RX_BUF_SIZE];
	uint8_t	usb_tx_buffer[USB_TX_BUF_SIZE];
	
	USB_DriverStruct_t	USB_Drv =
	{
			.data = usb_rx_buffer,
			.data_index = 0,
			.requested_len = XMODEM_LINE_LEN,
			.timeout = 250,
			.wakeup_id = WAKEUP_FROM_USB_DEVICE_IRQ,
	};
	uint32_t		usb_driver_handle;
	
b - register the driver:

	usb_driver_handle = usb_device_driver_register(&USB_Drv);
	
c - transmit

	usb_send(usb_driver_handle,usb_tx_buffer,32);
			
Notes:

There are no flags associated to this driver.
	
The wakeup_id field is the way A_os has to wake up the process, and is used to identify the irq source.
	Mandatory.

The timeout filed is the timeout after which even if the packet len has not reached the required len specified 
 in requested_len field the driver will wake up the process. 
 
 If the field requested_len is not specified or specified as 0 the driver will wake up the process at each 
  packet received. In this case the timeout value is ignored and is disabled.
 Conversely, if requested_len is specified the driver will wake up the process when the requested_len is reached
  or a timeout occours.  
 
 If the Rx_CallbackPtr field is compiled the referenced function will be called under the usb irq, so is
  quicker than the standard way of passing through the system and wake up the process.
 This makes the irq time longer but it ehxibits a faster execution.
  
 There are several examples in SampleProcesses
  
 


