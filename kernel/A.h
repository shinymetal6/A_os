/* 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Project : A_os
*/
/*
 * A.h
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */

#ifndef KERNEL_A_H_
#define KERNEL_A_H_

#include "main.h"
#include "system_default.h"

#define MAX_PROCESS					5
#define MAX_PROCESS_MASK			0x1e
#define USR_PROCESS_NUMBER			(MAX_PROCESS-1)
#define MAX_TIMERS					8
#define MAX_SEMAPHORES				8
#define MAX_UARTS_DRIVERS			8
#define MAX_UARTS_QUEUE				8
#define MAX_TIM_DRIVERS				8
#define MAX_ANALOG_DRIVERS			8
#define MAX_EXTMEM_DRIVERS			8
//#define MAX_SENSORS					16
#define MAX_I2C_DEVICES				16
#define MAX_SPI_DEVICES				16
#define MAX_GPIOINT_DRIVERS			16

#define	TIMER_CALLBACK_ARRAY_SIZE	8
#define	MAILBOX_NUM					8
#define	PERIPHERAL_NUM				32
#define	HWDEVICES_NUM				32
#define	MODULES_NUM					32

#define	DRIVER_STATUS_FAILED		0x40
#define	DRIVER_REQUEST_FAILED		0x80

#define DUMMY_XPSR  				0x01000000U

#ifdef	POOL_ENABLE
typedef struct
{
	uint8_t		*nxt_link;
	uint8_t		*pre_link;
	uint8_t		*mem_ptr;
	uint8_t		chunk_count;
	uint8_t		chunk_index;
	uint8_t		process;
	uint8_t		unused;
	uint8_t		flags;
}MEMpool_t;

/* flags */
#define		MEM_IN_USE			0x01
#define		MEM_CHANGED_OWNER	0x02
#define		MEM_IN_LIST			0x04
#define		LAST_IN_LIST		0x08
#define		INVALID				0x80
#endif	// #ifdef	POOL_ENABLE


#pragma pack(4)

typedef struct
{
	uint32_t	*osSysRam_start;
	uint32_t	osSysRam_size_word;
	uint32_t	g_tick_count;
	uint32_t	g_os_started;
#ifdef	POOL_ENABLE
	uint8_t		*first_mem;
	uint32_t	num_buf_in_use;
	uint32_t	first_data_address;
	uint32_t	mempool_available_size;
	uint32_t	mempool_requested_size;
	MEMpool_t 	*first_of_list;
	MEMpool_t 	*last_of_list;
#endif // #ifdef	POOL_ENABLE
	uint32_t	system_flags;
	uint8_t		general_flags;
	uint8_t		current_process;
	uint8_t		failed_process;
	uint8_t		fail_rsn;
	uint8_t		lwip_state;
	uint8_t		IP_ADDR[4],NETMASK_ADDR[4],GW_ADDR[4];
	uint8_t		process_hard_fault[MAX_PROCESS];
	uint8_t		process_bus_fault[MAX_PROCESS];
	uint8_t		process_mem_manage_fault[MAX_PROCESS];
	uint8_t		process_usage_fault[MAX_PROCESS];
	uint8_t		started_processes;
	char		version[40];
} Asys_t;

/* general_flags */
#define	LWIP_LOCK					0x40

/* system_flags */
/* this is set by a get_mem to signal the supervisor that
 * at least one buffer has been used, so defrag can start*/
#define	SYS_MEM_DEFRAG_REQUEST	0x80000000
/* qspi_status */
#define	ASYS_QSPI_BUSY				0x01
#define	ASYS_QSPI_WEL				0x02
#define	ASYS_QSPI_QE				0x04
#define	ASYS_QSPI_SUS				0x08
#define	ASYS_QSPI_ADS				0x10
#define	ASYS_QSPI_ADP				0x20
#define	ASYS_QSPI_SLEEP				0x40
#define	ASYS_QSPI_FOUND				0x80

/* user processes */
typedef struct
{
	void 		(*user_process)(uint32_t);
	uint16_t	stack_size;
	uint8_t		process_number;
}USRprcs_t;

typedef struct
{
	int32_t		(*svc_func)(int32_t,int32_t,int32_t,int32_t);
}SVCfunc_t;

typedef struct
{
	uint8_t		IP_ADDRHH;
	uint8_t		IP_ADDRHL;
	uint8_t		IP_ADDRLH;
	uint8_t		IP_ADDRLL;
	uint8_t		NETMASK_ADDRHH;
	uint8_t		NETMASK_ADDRHL;
	uint8_t		NETMASK_ADDRLH;
	uint8_t		NETMASK_ADDRLL;
	uint8_t		GW_ADDRHH;
	uint8_t		GW_ADDRHL;
	uint8_t		GW_ADDRLH;
	uint8_t		GW_ADDRLL;
}A_IpAddr_t;

typedef struct
{
	uint8_t 	process;
	uint8_t		status;
	uint32_t	flags;
}Modules_t;
/* status */
#define	MODULE_STATUS_FREE		0x00
#define	MODULE_STATUS_ALLOCATED	0x01


extern	void A_PreOS_Init(void);
extern	void A_Processor_Quirks(void);
extern	void A_PreScheduler_Init(void);
extern	void A_IrqPriority_Init(void);

extern	void A_bzero(uint8_t *ptr,uint16_t count);
extern	void A_memcpy(uint8_t *dest,uint8_t *source,uint16_t size);
extern	void schedule(void);
extern	void A_mem_init(void);

extern	void A_get_timelapse_start(void);
extern	uint32_t A_get_timelapse_end(void);
extern	void A_clear32(uint32_t	*ptr,uint32_t size);

#ifdef	POOL_ENABLE
extern	void defrag_mem(void);
extern	void reset_orphaned_chunks(uint8_t process);
#endif // #ifdef	POOL_ENABLE

extern	void MX_USB_DEVICE_Init(void);
extern	uint8_t MX_USB_Device_Init(void);
extern	void MX_LWIP_Init(A_IpAddr_t *A_IpAddr);

extern	void supervisor(void);
extern	void supervisor_process1(void);
extern	void supervisor_process2(void);
extern	void supervisor_process3(void);
extern	void supervisor_process4(void);

extern	Asys_t		Asys;

#endif /* KERNEL_A_H_ */
