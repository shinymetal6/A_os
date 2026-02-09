mlx90614 driver usage

1) The control structure is:
typedef struct
{
	/* driver header */
	uint8_t				status;
	uint8_t				flags;
	uint8_t 			process;
	I2C_HandleTypeDef 	*bus;
	uint16_t 			device_address;
	uint32_t 			wakeup_id;
	I2C_DriverStruct_t	*next_drv;
	/* driver proprietary data */
	uint16_t			mlx90614_found;
	uint8_t				pec_enabled;
	uint16_t			device_id;
	uint16_t			device_version;
	uint8_t 			rx_data[3];
	uint8_t 			crc_buff[5];
	uint8_t				crc;
	float	 			t_ambient,t_object;
}I2C_Mlx90614_Drv_TypeDef;

2) Example:
a - define the control structure and the related handle:
I2C_Mlx90614_Drv_TypeDef	mlx90614_Drv =
{
		.wakeup_id = 1,
		.bus = &hi2c2,
		.device_address = MLX90614_I2C_ADDR,
};
	
b - register the driver and start it:

	mlx90614_register(&mlx90614_Drv);

c - Use cases:

	The call to mlx90614_read_ambient(&mlx90614_Drv);will write ambient temperature in mlx90614_Drv->t_ambient.
	The call to mlx90614_read_object(&mlx90614_Drv);will write ambient temperature in mlx90614_Drv->t_object.
	These 2 functions should be called every 100 mSec minimum, as 10 Hz is the maximum possible update rate.  
	