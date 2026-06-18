/*
 * File      : board.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-09-22     Bernard      add board.h to this bsp
 * 2017-10-20     ZYH          emmm...setup for HAL Libraries
 */

// <<< Use Configuration Wizard in Context Menu >>>
#ifndef __BOARD_H__
#define __BOARD_H__

#include "ch32m030.h"
#define CH32M030_PIN_NUMBERS   48
#define LQFP48                 0
#define QFN48                  1
#define QFN48_7                2
#define Pack                   LQFP48

//#define CH32M030_PIN_NUMBERS   28
//#define CH32M030_PIN_NUMBERS   32



/* board configuration */
#define SRAM_SIZE  12
#define SRAM_END (0x20000000 + SRAM_SIZE * 1024)

extern int _ebss;
#define HEAP_BEGIN  ((void *)&_ebss)
#define HEAP_END    (SRAM_END-_stack_size)


//extern volatile unsigned long  interrupter_sp_saver;
void rt_hw_board_init(void);

#endif /* __BOARD_H__ */
