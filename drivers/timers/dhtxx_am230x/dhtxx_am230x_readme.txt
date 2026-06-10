Driver usage

1) The control structure is:
typedef struct
{
	uint8_t				status;
	uint8_t				flags;
	uint8_t				handle;
	uint8_t				device_index;
	uint8_t				state_machine;
	uint32_t			ticks;
	uint32_t			samples_number;
	uint32_t			dhtxx_am230x_samples[DHTXX_AM230X_MAX_SAMPLES_LEN];
	uint32_t			dhtxx_am230x_bitbytes[DHTXX_AM230X_MAX_BITBYTES_LEN];
	uint32_t			dhtxx_am230x_decoded[DHTXX_AM230X_BYTES_NUM];
	uint8_t				checksum;
	uint32_t			errors;
	TIM_HandleTypeDef 	*dht_timer;
	uint16_t 			dht_timer_channel;
	GPIO_TypeDef	 	*one_wire_port;
	uint16_t			one_wire_bit;
}Dhtxx_am230x_DriverStruct_t;

2) Example:
a - define the control structure, the related handle and the data buffer:

	Dhtxx_am230x_DriverStruct_t	Dhtxx_am230x_Drv =
	{
			.device_index = 0x00,
			.dht_timer = &htim2,
			.dht_timer_channel = TIM_CHANNEL_4,
			.one_wire_port = GPIOA,
			.one_wire_bit = 3,
	};
	uint32_t		dht_driver_handle;
	uint8_t			dht_data[DHTXX_AM230X_BYTES_NUM];
	
b - register the driver and start it:

	dht_driver_handle = dhtxx_am230x_register(&Dhtxx_am230x_Drv,0,0,Dhtxx_am230x_Drv.device_index);
	dhtxx_am230x_init(dht_driver_handle);
	dhtxx_am230x_start(dht_driver_handle);

c - when needed stop it:

	dhtxx_am230x_stop(dht_driver_handle);	
			
Notes:

There are no flags associated with this driver.

The driver will never wake up the calling process.

  
 


