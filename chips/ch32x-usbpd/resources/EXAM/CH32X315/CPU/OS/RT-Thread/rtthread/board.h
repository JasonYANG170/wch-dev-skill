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

#include "ch32x3x5.h"

#define CH32X315MCU6_CHIP   1
#define CH32X315WCU6_CHIP   2
#define CH32X315CCU6_CHIP   3
#define CH32X305RCT6_CHIP   4


#define CH32X3xx_CHIP     CH32X315WCU6_CHIP
/* board configuration */
#define SRAM_SIZE  64
#define SRAM_END (0x20000000+ SRAM_SIZE * 1024)

extern int _ebss;
#define HEAP_BEGIN  ((void *)&_ebss)
#define HEAP_END    (SRAM_END-__stack_size)



void rt_hw_board_init(void);

#endif /* __BOARD_H__ */
