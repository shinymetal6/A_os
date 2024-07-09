/*
******************************************************************************
**
**  File        : LinkerScript.ld
**
**  Author      : STM32CubeIDE
**
**  Abstract    : Linker script for STM32H7 series
**                2048Kbytes FLASH and 1056Kbytes RAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : STMicroelectronics STM32
**
**  Distribution: The file is distributed as is, without any warranty
**                of any kind.
**
*****************************************************************************
** @attention
**
** Copyright (c) 2023 STMicroelectronics.
** All rights reserved.
**
** This software is licensed under terms that can be found in the LICENSE file
** in the root directory of this software component.
** If no LICENSE file comes with this software, it is provided AS-IS.
**
****************************************************************************
*/

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = ORIGIN(RAM_D1) + LENGTH(RAM_D1);    /* end of RAM */
/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x200;      /* required amount of heap  */
_Min_Stack_Size = 0x400; /* required amount of stack */

/* Specify the memory areas */
MEMORY
{
  FLASH (rx)     : ORIGIN = 0x08000000, LENGTH = 2048K
  DTCMRAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 128K
  RAM_D1 (xrw)   : ORIGIN = 0x24000000, LENGTH = 512K
  RAM_D2 (xrw)   : ORIGIN = 0x30000000, LENGTH = 288K
  RAM_D3 (xrw)   : ORIGIN = 0x38000000, LENGTH = 64K
  ITCMRAM (xrw)  : ORIGIN = 0x00000000, LENGTH = 64K
}

/* Define output sections */
SECTIONS
{
	_osSysRam_start     = LOADADDR(.osSysRamStart);
	.osSysRamStart 		0x20000000  (NOLOAD): {. = ALIGN(4); KEEP(*(.osSysRamStart))}
	_osSysRam_end       = LOADADDR(.osSysRamEnd);
	.osSysRamEnd 		0x20010000  (NOLOAD): {. = ALIGN(4); KEEP(*(.osSysRamEnd))}

	_dtcm_user_data_start  = LOADADDR(.dtcm_user_data);
	.dtcm_user_data	 	0x20010000  (NOLOAD): {. = ALIGN(4); KEEP(*(.dtcm_user_data))}
	_dtcm_user_data_end  	= LOADADDR(.dtcm_user_data_end);
	.dtcm_user_data_end	0x2001C000  (NOLOAD): {. = ALIGN(4); KEEP(*(.dtcm_user_data_end))}
	
	_mempool_start      = LOADADDR(.osMemPoolStart);
	.osMemPoolStart		0x2001C000  (NOLOAD): {. = ALIGN(4); KEEP(*(.osMemPoolStart))}
    _mempool_end        = LOADADDR(.osMemPoolEnd);
	.osMemPoolEnd  		0x20020000  (NOLOAD): {. = ALIGN(4); KEEP(*(.osMemPoolEnd))}
		
	_dmaNoCache_start   = LOADADDR(.dmaNoCache);
	.dmaNoCache	        0x24070000  (NOLOAD): {. = ALIGN(4); KEEP(*(.dmaNoCache))}
	_dmaNoCache_end     = LOADADDR(.dmaNoCacheEnd);
	.dmaNoCacheEnd      0x24080000  (NOLOAD): {. = ALIGN(4); KEEP(*(.dmaNoCacheEnd))}
	
	_d2ram_start      	= LOADADDR(.d2ram);
	.d2ram		  		0x30000000  (NOLOAD): {. = ALIGN(4); KEEP(*(.d2ram))}
    _d2ram_end       	= LOADADDR(.d2ramEnd);
	.d2ramEnd  			0x30048000  (NOLOAD): {. = ALIGN(4); KEEP(*(.d2ramEnd))}
	
	_d3ram_start      	= LOADADDR(.d3ram);
	.d3ram		  		0x38000000  (NOLOAD): {. = ALIGN(4); KEEP(*(.d3ram))}
    _d3ram_end       	= LOADADDR(.d3ramEnd);
	.d3ramEnd  			0x38010000  (NOLOAD): {. = ALIGN(4); KEEP(*(.d3ramEnd))}
	
	_framebuffer_start 	= LOADADDR(.framebuffer);
	.framebuffer		0x38000000  (NOLOAD): {. = ALIGN(4); KEEP(*(.framebuffer))}
    _framebuffer_end    = LOADADDR(.framebufferEnd);
	.framebufferEnd  	0x38010000  (NOLOAD): {. = ALIGN(4); KEEP(*(.framebufferEnd))}
	
	
	_fdata_start = LOADADDR(.isr_vector);
	
  /* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
  } >FLASH
  
  .aos_nver :
  {
    . = ALIGN(4);
    KEEP(*(.aos_nver)) /* aos name and version */
    . = ALIGN(4);
  } >FLASH
  
  /* The program code and other data goes into FLASH */
  .text :
  {
    . = ALIGN(4);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;        /* define a global symbols at end of code */
  } >FLASH

  /* Constant data goes into FLASH */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
  } >FLASH

  .ARM.extab (READONLY)  : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH
  .ARM (READONLY) : {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >FLASH

  .preinit_array (READONLY)    :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH

  .init_array (READONLY) :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH

  .fini_array (READONLY) :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH

  /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data :
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */
    *(.RamFunc)        /* .RamFunc sections */
    *(.RamFunc*)       /* .RamFunc* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >RAM_D1 AT> FLASH

  _sid3_data = LOADADDR(.d3_data);
  .d3_data :
  {
    . = ALIGN(4);
    _sd3_data = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */
    *(.RamD3Func)        /* .RamFunc sections */
    *(.RamD3Func*)       /* .RamFunc* sections */
    . = ALIGN(4);
    _ed3_data = .;        /* define a global symbol at data end */
  } >RAM_D3 AT> FLASH
  
  _sid2_data = LOADADDR(.d2_data);
  .d2_data :
  {
    . = ALIGN(4);
    _sd2_data = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */
    *(.RamD2Func)        /* .RamFunc sections */
    *(.RamD2Func*)       /* .RamFunc* sections */
    . = ALIGN(4);
    _ed2_data = .;        /* define a global symbol at data end */
  } >RAM_D2 AT> FLASH

  _siitcm_data = LOADADDR(.itcm_data);
  .itcm_data :
  {
    . = ALIGN(4);
    _sitcm_data = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */
    *(.RamITCMFunc)        /* .RamFunc sections */
    *(.RamITCMFunc*)       /* .RamFunc* sections */
    . = ALIGN(4);
    _eitcm_data = .;        /* define a global symbol at data end */
  } >ITCMRAM AT> FLASH
  
    _fdata_end = LOADADDR(.flash_end);
  .flash_end :
  {
    . = ALIGN(4);
    _sflash_end_start = .;        /* create a global symbol at data start */
    . = ALIGN(4);
    _eflash_end_start = .;        /* define a global symbol at data end */
  } > FLASH
  
  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss section */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM_D1

  /* User_heap_stack section, used to check that there is enough RAM left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM_D1

  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}

