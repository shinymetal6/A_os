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

#ifdef	SAMPLEPROCESS_1_XMODEM
extern	void sample_process_1_xmodem(uint32_t process_id);	//This is process1
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
#ifdef	SAMPLEPROCESS_1_I2CMEM
extern	void sample_process_1_i2cmem(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_I2CMEM

extern	void sample_process_2(uint32_t process_id);	//This is process2
extern	void sample_process_3(uint32_t process_id);	//This is process3
extern	void sample_process_4(uint32_t process_id);	//This is process4 of the application

VERSIONING	uint8_t	app_name[32] 		= "H753 sample processes";
VERSIONING	uint8_t	app_version[32] 	= "h753";

USRprcs_t	UserProcesses[USR_PROCESS_NUMBER] =
{
		{
#ifdef	SAMPLEPROCESS_1_XMODEM
				.user_process = sample_process_1_xmodem,
#endif // #define	SAMPLEPROCESS_1_XMODEM
#ifdef	SAMPLEPROCESS_1_DCCPWM
				.user_process = sample_process_1_dccpwm,
#endif // #define	SAMPLEPROCESS_1_DCCPWM
#ifdef	SAMPLEPROCESS_1_DHTxx
				.user_process = sample_process_1_Dhtxx_am230x,
#endif // #define	SAMPLEPROCESS_1_DHTxx
#ifdef	SAMPLEPROCESS_1_QSPI
				.user_process = sample_process_1_qspi,
#endif // #define	SAMPLEPROCESS_1_QSPI
#ifdef	SAMPLEPROCESS_1_I2CMEM
				.user_process = sample_process_1_i2cmem,
#endif // #define	SAMPLEPROCESS_1_I2CMEM
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
