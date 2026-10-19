/*
 * sample_processes_table.c
 *
 *  Created on: Sep 13, 2023
 *      Author: fil
 */


#include "main.h"
#include "sample_A_os_includes.h"

#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#include "sample_user_config.h"

#ifdef	SAMPLEPROCESS_1_BASIC
extern	void sample_process_1_basic(uint32_t process_id);	//This is process1
extern	void sample_process_1_basic_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_BASIC

#ifdef	SAMPLEPROCESS_1_USB_XMODEM
extern	void sample_process_1_usb_xmodem(uint32_t process_id);	//This is process1
extern	void sample_process_1_usb_xmodem_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_USB_XMODEM

#ifdef	SAMPLEPROCESS_1_USB_TEST
extern	void sample_process_1_usb_test(uint32_t process_id);	//This is process1
extern	void sample_process_1_usb_test_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_USB_TEST

#ifdef	SAMPLEPROCESS_1_BASIC_DAC
extern	void sample_process_1_basic_dac(uint32_t process_id);	//This is process1
extern	void sample_process_1_basic_dac_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_BASIC_DAC

#ifdef	SAMPLEPROCESS_1_SDCARD_WAV_READER
extern	void sample_process_1_sdcard_wav_reader(uint32_t process_id);	//This is process1
extern	void sample_process_1_sdcard_wav_reader_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_SDCARD_WAV_READER

extern	void sample_process_2(uint32_t process_id);	//This is process3
extern	void sample_process_3(uint32_t process_id);	//This is process3
extern	void sample_process_4(uint32_t process_id);	//This is process4 of the application

VERSIONING	uint8_t	app_name[32] 		= "Sample Processes";
VERSIONING	uint8_t	app_version[32] 	= "Sample Version";

USRprcs_t	UserProcesses[USR_PROCESS_NUMBER] =
{
		{
#ifdef	SAMPLEPROCESS_1_BASIC
				.user_process = sample_process_1_basic,
				.user_init = sample_process_1_basic_init,
#endif // #define	SAMPLEPROCESS_1_BASIC
#ifdef	SAMPLEPROCESS_1_BASIC_DAC
				.user_process = sample_process_1_basic_dac,
				.user_init = sample_process_1_basic_dac_init,
#endif // #define	SAMPLEPROCESS_1_BASIC_DAC
#ifdef	SAMPLEPROCESS_1_USB_XMODEM
				.user_process = sample_process_1_usb_xmodem,
				.user_init = sample_process_1_usb_xmodem_init,
#endif // #define	SAMPLEPROCESS_1_USB_XMODEM
#ifdef	SAMPLEPROCESS_1_USB_TEST
				.user_process = sample_process_1_usb_test,
				.user_init = sample_process_1_usb_test_init,
#endif // #define	SAMPLEPROCESS_1_USB_TEST
#ifdef	SAMPLEPROCESS_1_SDCARD_WAV_READER
				.user_process = sample_process_1_sdcard_wav_reader,
				.user_init = sample_process_1_sdcard_wav_reader_init,
#endif // #define	SAMPLEPROCESS_1_SDCARD_WAV_READER
				.stack_size = 1024,
		},
		{
				.user_process = sample_process_2,
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
