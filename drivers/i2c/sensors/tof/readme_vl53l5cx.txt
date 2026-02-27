mlx90614 driver usage

1) The control structure is:
/* Motion detector results */
#ifndef VL53L5CX_DISABLE_MOTION_INDICATOR
	struct
	{
		uint32_t global_indicator_1;
		uint32_t global_indicator_2;
		uint8_t	 status;
		uint8_t	 nb_of_detected_aggregates;
		uint8_t	 nb_of_aggregates;
		uint8_t	 spare;
		uint32_t motion[32];
	} motion_indicator;
#endif
} VL53L5CX_ResultsData_TypeDef;

typedef struct
{
	/* driver header */
	uint8_t					status;
	uint8_t					flags;
	uint8_t 				process;
	I2C_HandleTypeDef 		*bus;
	uint16_t 				device_address;
	uint32_t 				wakeup_id;
	I2C_DriverStruct_t		*next_drv;
	/* driver proprietary data */
	GPIO_TypeDef	 		*lpn_port;
	uint16_t				lpn_bit;
	uint32_t				who_am_i;
	uint8_t	        		temp_buffer[VL53L5CX_MAX_RESULTS_SIZE];
	uint8_t		        	*default_configuration;
	uint8_t		        	*default_xtalk;
	uint8_t					is_auto_stop_enabled;
	uint8_t		        	offset_data[VL53L5CX_OFFSET_BUFFER_SIZE];
	uint8_t		        	xtalk_data[VL53L5CX_XTALK_BUFFER_SIZE];
	uint32_t	        	data_read_size;
	uint8_t		        	streamcount;
	VL53L5CX_ResultsData_TypeDef	*VL53L5CX_ResultsData;
}I2C_vl53l5cx_Drv_TypeDef;

2) Example:
a - define the control structure:
VL53L5CX_ResultsData_TypeDef	VL53L5CX_ResultsData =
{

};
I2C_vl53l5cx_Drv_TypeDef I2C_vl53l5cx_Drv =
{
		.bus = &hi2c2,
		.lpn_port = TOF_LPN_GPIO_Port,
		.lpn_bit = TOF_LPN_Pin,
		.wakeup_id = WAKEUP_FROM_I2C2_IRQ,
		.VL53L5CX_ResultsData = &VL53L5CX_ResultsData,
};
	
b - register the driver and start it:

	vl53l5cx_register(&I2C_vl53l5cx_Drv);

c - Use cases:

	The call to vl53l5cx_start_ranging(&I2C_vl53l5cx_Drv); will start measurement, results are stored in I2C_vl53l5cx_Drv->VL53L5CX_ResultsData.
	