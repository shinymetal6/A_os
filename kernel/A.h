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

#define MAX_PROCESS				5
#define USR_PROCESS_NUMBER		(MAX_PROCESS-1)
#define MAX_TIMERS				8
#define MAX_SEMAPHORES			8
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
#define		LAST_IN_LIST		0x08
#define		INVALID				0x80

typedef struct
{
	uint8_t 	process;
	uint8_t		status;
	uint16_t	rxlen;
	uint32_t	flags;
	uint8_t		sentinel_start;
	uint8_t		sentinel_end;
	uint8_t		*rx_buf;
	uint16_t	rx_buf_index;
	uint16_t	rx_buf_max_len;
	uint16_t	timeout;
	uint16_t	timeout_reload_value;
}HWMngr_t;
/* flags */
#define	HWMAN_TIMEOUT_ENABLED	0x80000000


#define	HWMAN_STATUS_FREE		0x00
#define	HWMAN_STATUS_ALLOCATED	0x01
#define	HWMAN_SENTINEL_ENABLE	0x02
#define	HWMAN_SENTINEL_FOUND	0x04
#define	HWMAN_SINGLE_IRQ		0x80
#define	HWMAN_STD_IRQ			0x00

#define	HWMAN_QUEUE_LEN			16

#ifdef A_HAS_UARTS
#include "HwDevices/hw_uart.h"

#ifdef	A_HAS_UART1
extern	UART_HandleTypeDef huart1;
#endif
#ifdef	A_HAS_UART2
extern	UART_HandleTypeDef huart2;
#endif
#ifdef	A_HAS_UART3
extern	UART_HandleTypeDef huart3;
#endif
#ifdef	A_HAS_UART4
extern	UART_HandleTypeDef huart4;
#endif
#ifdef	A_HAS_UART5
extern	UART_HandleTypeDef huart5;
#endif
#ifdef	A_HAS_UART6
extern	UART_HandleTypeDef huart6;
#endif
#ifdef	A_HAS_UART7
extern	UART_HandleTypeDef huart6;
#endif
typedef struct _HW_Uart_t
{
	UART_HandleTypeDef 	*hwuart_handle;
	uint32_t			hwuart_index;
}HW_Uart_t;

#define	A_MAX_UART		7
#endif

#ifdef A_HAS_SPI_BUS
#include "HwDevices/hw_spi.h"

#ifdef	A_HAS_SPI1
extern	SPI_HandleTypeDef hspi1;
#endif
#ifdef	A_HAS_SPI2
extern	SPI_HandleTypeDef hspi2;
#endif
#ifdef	A_HAS_SPI3
extern	SPI_HandleTypeDef hspi3;
#endif
#ifdef	A_HAS_SPI4
extern	SPI_HandleTypeDef hspi4;
#endif

typedef struct _HW_Spi_t
{
	SPI_HandleTypeDef 	*hwspi_handle;
	uint32_t			hwspi_index;
	uint8_t				hwspi_flags;
}HW_Spi_t;

#define	A_SPI_CLEARFLAG		0x00
#define	A_SPI_DMA_RUNNING	0x00
#define	A_SPI_DMA_DONE		0x01
#define	A_SPI_USE_DMA		0x80
#define	A_MAX_SPI			4
#endif

#ifdef A_HAS_I2C_BUS
#include "HwDevices/hw_i2c.h"

#ifdef	A_HAS_I2C1
extern	I2C_HandleTypeDef 	hi2c1;
#endif
#ifdef	A_HAS_I2C2
extern	I2C_HandleTypeDef 	hi2c2;
#endif

typedef struct _HW_I2C_t
{
	I2C_HandleTypeDef 	*hwi2c_handle;
	uint32_t			hwi2c_index;
	uint8_t				hwi2c_flags;
}HW_I2C_t;
#define	A_MAX_I2C			2
#endif

#ifdef A_HAS_TIMERS
#include "HwDevices/hw_timers.h"

#ifdef	A_HAS_TIMER1
extern	TIM_HandleTypeDef htim1;
#endif
#ifdef	A_HAS_TIMER2
extern	TIM_HandleTypeDef htim2;
#endif
#ifdef	A_HAS_TIMER3
extern	TIM_HandleTypeDef htim3;
#endif
#ifdef	A_HAS_TIMER4
extern	TIM_HandleTypeDef htim4;
#endif
#ifdef	A_HAS_TIMER5
extern	TIM_HandleTypeDef htim5;
#endif
#ifdef	A_HAS_TIMER6
extern	TIM_HandleTypeDef htim6;
#endif
#ifdef	A_HAS_TIMER7
extern	TIM_HandleTypeDef htim7;
#endif
#ifdef	A_HAS_TIMER8
extern	TIM_HandleTypeDef htim8;
#endif
#ifdef	A_HAS_TIMER9
extern	TIM_HandleTypeDef htim9;
#endif
#ifdef	A_HAS_TIMER10
extern	TIM_HandleTypeDef htim10;
#endif
#ifdef	A_HAS_TIMER11
extern	TIM_HandleTypeDef htim11;
#endif
#ifdef	A_HAS_TIMER12
extern	TIM_HandleTypeDef htim12;
#endif
#ifdef	A_HAS_TIMER13
extern	TIM_HandleTypeDef htim13;
#endif
#ifdef	A_HAS_TIMER14
extern	TIM_HandleTypeDef htim14;
#endif
#ifdef	A_HAS_TIMER15
extern	TIM_HandleTypeDef htim15;
#endif
#ifdef	A_HAS_TIMER16
extern	TIM_HandleTypeDef htim16;
#endif
#ifdef	A_HAS_TIMER17
extern	TIM_HandleTypeDef htim17;
#endif

typedef struct _HW_Timers_t
{
	TIM_HandleTypeDef 	*hwtimer_handle;
	uint32_t			hwtimer_index;
	uint8_t				hwtimer_owner;
	uint8_t				hwtimer_flags;
}HW_Timers_t;
#define	A_MAX_TIMERS		17
#endif

typedef struct
{
	uint8_t 	process;
	uint8_t		status;
}IrqMngr_t;
/* flags */
#define	IRQMAN_TIMEOUT_ENABLED	0x80000000
#define	IRQMAN_FREE		0
#define	IRQMAN_ALLOCATED	1

typedef struct
{
	uint8_t		current_process;
	uint32_t	g_tick_count;
	uint32_t	g_os_started;
	uint8_t		*first_mem;
	uint32_t	num_buf_in_use;
	uint32_t	first_data_address;
	uint32_t	mempool_available_size;
	uint32_t	mempool_requested_size;
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
	uint8_t		qspi_status;
	uint8_t		qspi_id;
	uint32_t 	qspi_size;
	char		version[40];
}Asys_t;


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

extern	void A_PreOS_Init(void);
extern	void A_Processor_Quirks(void);
extern	void A_PreScheduler_Init(void);
extern	void A_IrqPriority_Init(void);

extern	void A_bzero(uint8_t *ptr,uint16_t count);
extern	void A_memcpy(uint8_t *dest,uint8_t *source,uint16_t size);
extern	void schedule(void);
extern	void A_mem_init(void);
extern	void defrag_mem(void);
extern	void reset_orphaned_chunks(uint8_t process);

extern	void check_semaphores(void);
extern	void MX_USB_DEVICE_Init(void);
extern	uint8_t MX_USB_Device_Init(void);
extern	void MX_LWIP_Init(A_IpAddr_t *A_IpAddr);

/* Audio */
extern	void IrqProcessSamples(void);
extern	void RunOscillator32(void);
/* Audio END */



extern	void supervisor(void);
extern	void supervisor_process1(void);
extern	void supervisor_process2(void);
extern	void supervisor_process3(void);
extern	void supervisor_process4(void);

extern	Asys_t		Asys;

#endif /* KERNEL_A_H_ */
