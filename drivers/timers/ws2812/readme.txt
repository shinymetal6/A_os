Driver usage

1) The control structure is:
typedef struct
{
	/* timer header */
	uint8_t 			process;
	uint8_t				status;
	uint8_t				flags;
	uint32_t 			*next_timer;
	TIM_HandleTypeDef 	*ws2812_timer;
	uint8_t				timer_type;
	/* timer internals */
	uint8_t				handle;
	uint32_t 			ws2812_timer_channel;
	uint32_t 			ws2812_numleds;
	uint32_t 			*ws2812_work_buf;
	uint32_t 			ws2812_work_buf_buflen;
	uint32_t			wakeup_id;
}WS2812_Drv_TypeDef;

2) Example:
a - define the control structure and the related handle:
#define	NUM_LEDS		8
#define	WS2812_WORK_BUF_LEN		((NUM_LEDS*WS2812_LEDBPP)+WS2812_SYNCLEN)
uint32_t ws2812_work_buf[WS2812_WORK_BUF_LEN];

WS2812_Drv_TypeDef	WS2812_Drv =
{
		.ws2812_timer = &htim3,
		.ws2812_timer_channel = TIM_CHANNEL_1,
		.ws2812_numleds = 8,
		.ws2812_work_buf = &ws2812_work_buf[0],
		.ws2812_work_buf_buflen = WS2812_WORK_BUF_LEN,
		.wakeup_id = WAKEUP_FROM_TIM_IRQ,
};
	
b - register the driver and start it:

	ws2812_register(&WS2812_Drv);

c - Use cases:

	To set a pixel color call ws2812_SetPixel(WS2812_Drv_TypeDef *ws2812_drv,uint32_t location, uint8_t r,uint8_t g,uint8_t b) 
	  with the appropriate parameters : location is the pixel number, r-g-b are the colors.
	To copy a user frame buffer to the work buffer call ws2812_UserFB_to_WorkBuf(WS2812_Drv_TypeDef *ws2812_drv,uint8_t *user_fb,uint32_t user_fb_len)
	  with the appropriate parameters.
	  
As an example for Cube initialization the two images TimerSettings1.png and TimerSettings2.png are an example for standard timing initialization. 
The driver will initialize the PSC and ARR registers of the selected timer, so there is no need to do calculation for Cube.
	

  
 


