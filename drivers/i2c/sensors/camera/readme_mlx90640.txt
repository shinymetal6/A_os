mlx90640 driver usage

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
	uint8_t				mlx90640_found;
	uint16_t			*mlx90640_eeprom;	//[832]
    uint16_t 			*frame_data; 	//[768] MLX90640_PIXEL_NUM
    uint16_t 			*aux_data; 	//[64] MLX90640_AUX_NUM
    float	 			*to_data; 	//[768] MLX90640_AUX_NUM
    uint16_t 			*resulting_image; 	//[768] MLX90640_PIXEL_NUM
	uint16_t			mlx90640_status_reg;
	uint16_t			mlx90640_control_reg;
	uint16_t			mlx90640_i2c_conf_reg;
	uint32_t			mlx90640_resolutionRAM;
	uint32_t			mlx90640_refresh_rate;
	/* */
    int16_t 			kVdd;
    int16_t 			vdd25;
    float 				KvPTAT;
    float 				KtPTAT;
    uint16_t 			vPTAT25;
    float 				alphaPTAT;
    int16_t 			gainEE;
    float 				tgc;
    float 				cpKv;
    float 				cpKta;
    uint8_t 			resolutionEE;
    uint8_t 			calibrationModeEE;
    float 				KsTa;
    float 				ksTo[5];
    int16_t 			ct[5];
    uint16_t 			*alpha; 	//[768] MLX90640_PIXEL_NUM
    uint8_t 			alphaScale;
    int16_t 			*offset; 	//[768] MLX90640_PIXEL_NUM
    int8_t 				*kta;		//[768] MLX90640_PIXEL_NUM
    uint8_t 			ktaScale;
    int8_t 				*kv;	//[768] MLX90640_PIXEL_NUM
    uint8_t 			kvScale;
    float 				cpAlpha[2];
    int16_t 			cpOffset[2];
    float 				ilChessC[3];
    uint16_t 			brokenPixels[5];
    uint16_t 			outlierPixels[5];
    float 				*alphaTemp;	//[768] MLX90640_PIXEL_NUM
}I2C_Mlx90640_DriverStruct_t;


2) Example:
a - Allocate the necessary buffers in user space:
uint16_t			mlx90640_eeprom[MLX90640_EEPROM_DUMP_NUM];
uint16_t 			frame_data[MLX90640_PIXEL_NUM];
uint16_t 			aux_data[MLX90640_AUX_NUM];
float	 			to_data[MLX90640_PIXEL_NUM];
uint16_t 			resulting_image[MLX90640_PIXEL_NUM];
uint16_t 			alpha[MLX90640_PIXEL_NUM];
int16_t 			offset[MLX90640_PIXEL_NUM];
int8_t 				kta[MLX90640_PIXEL_NUM];
int8_t 				kv[MLX90640_PIXEL_NUM];
float 				alphaTemp[MLX90640_PIXEL_NUM];

b - define the control structure and the related handle:
I2C_Mlx90640_DriverStruct_t	I2C_Mlx90640_Drv =
{
		.wakeup_id = 1,
		.bus = &hi2c2,
		.device_address = MLX90640_ADDRESS,
		.mlx90640_eeprom = mlx90640_eeprom,
		.frame_data = frame_data,
		.aux_data = aux_data,
		.to_data = to_data,
		.resulting_image = resulting_image,
		.alpha = alpha,
		.offset = offset,
		.kta = kta,
		.kv = kv,
		.alphaTemp = alphaTemp,
};
	
c - register the driver and start it:

	mlx90640_register(&I2C_Mlx90640_Drv);

d - Use cases:

	The call to	mlx90640_run(&I2C_Mlx90640_Drv,1.0,1.0); will create the thermal image in I2C_Mlx90640_Drv->resulting_image
