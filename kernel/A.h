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

#define MAX_PROCESS				5
#define USR_PROCESS_NUMBER		(MAX_PROCESS-1)
#define MAX_TIMERS				8
#define	MAILBOX_NUM				8
#define	PERIPHERAL_NUM			32

#define DUMMY_XPSR  				0x01000000U

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
#define		LAST_IN_LIST		0x80


typedef struct
{
	uint8_t 	process;
	uint8_t		status;
	uint16_t	rxlen;
	uint32_t	flags;
	uint8_t		*rx_buf;
}HWMngr_t;

#define	HWMAN_STATUS_FREE		0
#define	HWMAN_STATUS_ALLOCATED	1

#define	HWMAN_QUEUE_LEN			16

typedef struct
{
	uint32_t	peripheral;
	uint8_t 	process;
	uint8_t		*ptr[HWMAN_QUEUE_LEN];
	uint16_t	len[HWMAN_QUEUE_LEN];
	uint8_t		insertion_index;
	uint8_t		extraction_index;
	uint8_t		queued_bufs;
}HWMngr_queue_t;
#define	HW_MNGR_QUEUE_LEN	( sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint8_t *)*HWMAN_QUEUE_LEN + sizeof(uint16_t)*HWMAN_QUEUE_LEN +sizeof(uint8_t) + sizeof(uint8_t) )
#define	HW_MNGR_QUEUE_NOT_OWNED	0x10000000
#define	HW_MNGR_QUEUE_FULL		0x20000000
#define	HW_MNGR_QUEUE_WAS_EMPTY	0x80000000


typedef struct
{
	uint8_t		current_process;
	uint32_t	g_tick_count;
	uint32_t	g_os_started;
	uint8_t		*first_mem;
	uint32_t	num_buf_in_use;
	uint32_t	first_data_address;
	MEMpool_t 	*first_of_list;
	MEMpool_t 	*last_of_list;
	uint32_t	system_flags;
	uint8_t		failed_process;
	uint8_t		fail_rsn;
	uint8_t		lwip_state;
	uint8_t		IP_ADDR[4],NETMASK_ADDR[4],GW_ADDR[4];
	uint8_t		process_hard_fault[MAX_PROCESS];
	uint8_t		process_bus_fault[MAX_PROCESS];
	uint8_t		process_mem_manage_fault[MAX_PROCESS];
	uint8_t		process_usage_fault[MAX_PROCESS];
}Asys_t;

/* system_flags */
#define	SYS_MEM_DEFRAG_REQUEST	0x00000001

/* user processes */
typedef struct
{
	void 		(*user_process)(uint32_t);
	uint16_t	stack_size;
	uint16_t	process_number;
}USRprcs_t;

typedef struct
{
	int32_t		(*svc_func)(int32_t,int32_t,int32_t,int32_t);
}SVCfunc_t;

typedef struct
{
	uint8_t		IP_ADDR0;
	uint8_t		IP_ADDR1;
	uint8_t		IP_ADDR2;
	uint8_t		IP_ADDR3;
	uint8_t		NETMASK_ADDR0;
	uint8_t		NETMASK_ADDR1;
	uint8_t		NETMASK_ADDR2;
	uint8_t		NETMASK_ADDR3;
	uint8_t		GW_ADDR0;
	uint8_t		GW_ADDR1;
	uint8_t		GW_ADDR2;
	uint8_t		GW_ADDR3;
}A_IpAddr_t;

extern	void A_bzero(uint8_t *ptr,uint16_t count);
extern	void A_memcpy(uint8_t *dest,uint8_t *source,uint16_t size);
extern	void schedule(void);
extern	void A_mem_init(void);
extern	void defrag_mem(void);
extern	void reset_orphaned_chunks(uint8_t process);

extern	uint32_t queue_insert(HWMngr_queue_t *queue, uint8_t *buf, uint16_t len);
extern	uint8_t *queue_extract(HWMngr_queue_t *queue, uint8_t *numbuf, uint16_t *len);
extern	uint32_t create_queue(HWMngr_queue_t *queue,uint32_t peripheral);
extern	uint32_t destroy_queue(HWMngr_queue_t *queue,uint32_t peripheral);
extern	uint32_t get_queue_len(HWMngr_queue_t *queue);


extern	void MX_USB_DEVICE_Init(void);
extern	void MX_LWIP_Init(A_IpAddr_t *A_IpAddr);

extern	void supervisor(void);
extern	void supervisor_process1(void);
extern	void supervisor_process2(void);
extern	void supervisor_process3(void);
extern	void supervisor_process4(void);

extern	Asys_t		Asys;

#endif /* KERNEL_A_H_ */
