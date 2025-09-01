/*
 * processes_table.c
 *
 *  Created on: Sep 13, 2023
 *      Author: fil
 */


#include "main.h"
#include "A_os_includes.h"

#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"

#ifdef	SAMPLEPROCESS_1_BASIC
extern	void sample_process_1_basic(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_BASIC
#ifdef	SAMPLEPROCESS_1_XMODEM_RX_UART
extern	void sample_process_1_xmodem_rx_UART(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_XMODEM_RX_UART
#ifdef	SAMPLEPROCESS_1_XMODEM_RX_USB
extern	void sample_process_1_init(uint32_t process_id);
extern	void sample_process_1_xmodem_rx_USB(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_XMODEM
#ifdef	SAMPLEPROCESS_1_DCCPWM
extern	void sample_process_1_dccpwm(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_DCCPWM
#ifdef	SAMPLEPROCESS_1_DHTxx
extern	void sample_process_1_Dhtxx_am230x(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_DHTxx
#ifdef	SAMPLEPROCESS_1_QSPI
extern	void sample_process_1_qspi(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_QSPI
#ifdef	SAMPLEPROCESS_1_SERVO
extern	void sample_process_1_servo(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_SERVO
#ifdef	SAMPLEPROCESS_1_I2CMEM
extern	void sample_process_1_i2cmem(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_I2CMEM
#ifdef	SAMPLEPROCESS_1_ADC
extern	void sample_process_1_adc(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_ADC
#ifdef	SAMPLEPROCESS_UARTQUEUES
extern	void sample_process_1_queuetest(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_ADC
#ifdef	SAMPLEPROCESS_INTFLASH
extern	void sample_process_1_intflash(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_INTFLASH
#ifdef	SAMPLEPROCESS_I2CSENSORS
extern	void sample_process_1_i2csensors(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_I2CSENSORS
#ifdef	SAMPLEPROCESS_1_USBAUDIO
extern	void sample_process_1_usbaudio(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_USBAUDIO
#ifdef	SAMPLEPROCESS_1_CAN
extern	void sample_process_1_can(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_CAN
#ifdef	SAMPLEPROCESS_1_LORA
extern	void sample_process_1_lora(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_LORA
#ifdef	SAMPLEPROCESS_1_LORA_SCANNER
extern	void sample_process_1_lora_scanner(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_LORA_SCANNER
#ifdef	SAMPLEPROCESS_MBX
extern	void sample_process_1_mbxToPrc2(uint32_t process_id);	//This is process1
extern	void sample_process_2_mbxFromPrc1(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_TIMDELAY_DEBUG
#ifdef	SAMPLEPROCESS_1_LCD7735
extern	void sample_process_1_lcd7735(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_LCD7735


extern	void sample_process_2(uint32_t process_id);	//This is process3
extern	void sample_process_3(uint32_t process_id);	//This is process3
extern	void sample_process_4(uint32_t process_id);	//This is process4 of the application

VERSIONING	uint8_t	app_name[32] 		= "H753 sample processes";
VERSIONING	uint8_t	app_version[32] 	= "h753";

USRprcs_t	UserProcesses[USR_PROCESS_NUMBER] =
{
		{
#ifdef	SAMPLEPROCESS_1_BASIC
				.user_process = sample_process_1_basic,
#endif // #define	SAMPLEPROCESS_1_BASIC
#ifdef	SAMPLEPROCESS_1_XMODEM_RX_UART
				.user_process = sample_process_1_xmodem_rx_UART,
#endif // #define	SAMPLEPROCESS_1_XMODEM_RX_UART
#ifdef	SAMPLEPROCESS_1_XMODEM_RX_USB
				.user_process = sample_process_1_xmodem_rx_USB,
				.user_init = sample_process_1_init,
#endif // #define	SAMPLEPROCESS_1_XMODEM_RX_USB
#ifdef	SAMPLEPROCESS_1_DCCPWM
				.user_process = sample_process_1_dccpwm,
#endif // #define	SAMPLEPROCESS_1_DCCPWM
#ifdef	SAMPLEPROCESS_1_DHTxx
				.user_process = sample_process_1_Dhtxx_am230x,
#endif // #define	SAMPLEPROCESS_1_DHTxx
#ifdef	SAMPLEPROCESS_1_QSPI
				.user_process = sample_process_1_qspi,
#endif // #define	SAMPLEPROCESS_1_QSPI
#ifdef	SAMPLEPROCESS_1_SERVO
				.user_process = sample_process_1_servo,
#endif // #define	SAMPLEPROCESS_1_SERVO
#ifdef	SAMPLEPROCESS_1_I2CMEM
				.user_process = sample_process_1_i2cmem,
#endif // #define	SAMPLEPROCESS_1_I2CMEM
#ifdef	SAMPLEPROCESS_1_ADC
				.user_process = sample_process_1_adc,
#endif // #define	SAMPLEPROCESS_1_ADC
#ifdef	SAMPLEPROCESS_UARTQUEUES
				.user_process = sample_process_1_queuetest,
#endif // #define	SAMPLEPROCESS_UARTQUEUES
#ifdef	SAMPLEPROCESS_INTFLASH
				.user_process = sample_process_1_intflash,
#endif // #define	SAMPLEPROCESS_INTFLASH
#ifdef	SAMPLEPROCESS_I2CSENSORS
				.user_process = sample_process_1_i2csensors,
#endif // #define	SAMPLEPROCESS_I2CSENSORS
#ifdef	SAMPLEPROCESS_1_USBAUDIO
				.user_process = sample_process_1_usbaudio,
#endif // #define	SAMPLEPROCESS_1_USBAUDIO
#ifdef	SAMPLEPROCESS_1_CAN
				.user_process = sample_process_1_can,
#endif // #define	SAMPLEPROCESS_1_CAN
#ifdef	SAMPLEPROCESS_1_LORA
				.user_process = sample_process_1_lora,
#endif // #define	SAMPLEPROCESS_1_LORA
#ifdef	SAMPLEPROCESS_1_LORA_SCANNER
				.user_process = sample_process_1_lora_scanner,
#endif // #define	SAMPLEPROCESS_1_LORA_SCANNER
#ifdef	SAMPLEPROCESS_MBX
				.user_process = sample_process_1_mbxToPrc2,
#endif // #define	SAMPLEPROCESS_MBX
#ifdef	SAMPLEPROCESS_1_LCD7735
				.user_process = sample_process_1_lcd7735,
#endif // #define	SAMPLEPROCESS_1_LCD7735
				.stack_size = 1024,
		},
		{
#ifdef	SAMPLEPROCESS_MBX
				.user_process = sample_process_2_mbxFromPrc1,
#else
				.user_process = sample_process_2,
#endif // #define	SAMPLEPROCESS_MBX
				.stack_size = 1024,
		},
		{
				.user_process = sample_process_3,
				.stack_size = 1024,
		},
		{
				.user_process = sample_process_4,
				.stack_size = 1024,
		}
};

#endif // #ifdef SAMPLE_PROCESSES_ENABLED
