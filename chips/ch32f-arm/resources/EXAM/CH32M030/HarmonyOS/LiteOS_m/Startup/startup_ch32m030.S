;/********************************** (C) COPYRIGHT *******************************
;* File Name          : startup_ch32m030.s
;* Author             : WCH
;* Version            : V1.0.2
;* Date               : 2025/12/15
;* Description        : CH32M030 StartUp File.
;*********************************************************************************
;* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
;* Attention: This software (modified or not) and binary are used for 
;* microcontroller manufactured by Nanjing Qinheng Microelectronics.
;*******************************************************************************/

	.section	.init,"ax",@progbits
	.global	_start
	.align	2
_start:

    .option norvc;
    j       handle_reset
    .word   0
    .word   NMI_Handler                /* NMI */
    .word   HardFault_Handler          /* Hard Fault */
    .word   0
    .word   Ecall_M_Mode_Handler       /* Ecall M Mode */
    .word   0
    .word   0
    .word   Ecall_U_Mode_Handler       /* Ecall U Mode */
    .word   Break_Point_Handler        /* Break Point */
    .word   0
    .word   0
    .word   SysTick_Handler            /* SysTick */
    .word   0
    .word   SW_Handler                 /* SW */
    .word   0
    /* External Interrupts */
    .word   WWDG_IRQHandler            /* Window Watchdog */
    .word   OPA_IRQHandler             /* OPA */
    .word   FLASH_IRQHandler           /* Flash */
    .word   RCC_IRQHandler             /* RCC */
    .word   EXTI7_0_IRQHandler         /* EXTI Line 7..0 */
    .word   AWU_IRQHandler             /* Auto Wake up */
    .word   DMA1_Channel1_IRQHandler   /* DMA1 Channel 1 */
    .word   DMA1_Channel2_IRQHandler   /* DMA1 Channel 2 */
    .word   DMA1_Channel3_IRQHandler   /* DMA1 Channel 3 */
    .word   DMA1_Channel4_IRQHandler   /* DMA1 Channel 4 */
    .word   DMA1_Channel5_IRQHandler   /* DMA1 Channel 5 */
    .word   DMA1_Channel6_IRQHandler   /* DMA1 Channel 6 */
    .word   DMA1_Channel7_IRQHandler   /* DMA1 Channel 7 */
    .word   ADC_IRQHandler             /* ADC */
    .word   I2C1_EV_IRQHandler         /* I2C1 Event */
    .word   I2C1_ER_IRQHandler         /* I2C1 Error */
    .word   USART1_IRQHandler          /* USART1 */
    .word   EXTI15_8_IRQHandler        /* EXTI Line 15..8 */
    .word   TIM1_BRK_IRQHandler        /* TIM1 Break */
    .word   TIM1_UP_IRQHandler         /* TIM1 Update */
    .word   TIM1_TRG_COM_IRQHandler    /* TIM1 Trigger and Commutation */
    .word   TIM1_CC_IRQHandler         /* TIM1 Capture Compare */
    .word   TIM2_IRQHandler            /* TIM2 */
    .word   USBPD0_IRQHandler          /* USBPD0 */
    .word   USBPD0WakeUp_IRQHandler    /* USBPD0 Wake up */
    .word   USBPD1_IRQHandler          /* USBPD1 */
    .word   USBPD1WakeUp_IRQHandler    /* USBPD1 Wake up */
    .word   TIM3_IRQHandler            /* TIM3 */
    .word   SPI1_IRQHandler            /* SPI1 */
    .word   USBFS_IRQHandler           /* USBFS */
    .word   USBFSWakeUp_IRQHandler       /* USBFS Wakeup from suspend */

    .option rvc;
    .section    .text.vector_handler, "ax", @progbits
    .weak   NMI_Handler
    .weak   HardFault_Handler
    .weak   Ecall_M_Mode_Handler
    .weak   Ecall_U_Mode_Handler
    .weak   Break_Point_Handler
    .weak   SysTick_Handler
    .weak   SW_Handler
    .weak   WWDG_IRQHandler
    .weak   OPA_IRQHandler
    .weak   FLASH_IRQHandler
    .weak   RCC_IRQHandler
    .weak   EXTI7_0_IRQHandler
    .weak   AWU_IRQHandler
    .weak   DMA1_Channel1_IRQHandler
    .weak   DMA1_Channel2_IRQHandler
    .weak   DMA1_Channel3_IRQHandler
    .weak   DMA1_Channel4_IRQHandler
    .weak   DMA1_Channel5_IRQHandler
    .weak   DMA1_Channel6_IRQHandler
    .weak   DMA1_Channel7_IRQHandler
    .weak   ADC_IRQHandler
    .weak   I2C1_EV_IRQHandler
    .weak   I2C1_ER_IRQHandler
    .weak   USART1_IRQHandler
    .weak   EXTI15_8_IRQHandler
    .weak   TIM1_BRK_IRQHandler
    .weak   TIM1_UP_IRQHandler
    .weak   TIM1_TRG_COM_IRQHandler
    .weak   TIM1_CC_IRQHandler
    .weak   TIM2_IRQHandler
    .weak   USBPD0_IRQHandler
    .weak   USBPD0WakeUp_IRQHandler
    .weak   USBPD1_IRQHandler
    .weak   USBPD1WakeUp_IRQHandler
    .weak   TIM3_IRQHandler
    .weak   SPI1_IRQHandler
    .weak   USBFS_IRQHandler
    .weak   USBFSWakeUp_IRQHandler
NMI_Handler:
HardFault_Handler:
Ecall_M_Mode_Handler:
Ecall_U_Mode_Handler:
Break_Point_Handler:
SysTick_Handler:
SW_Handler:
WWDG_IRQHandler:
OPA_IRQHandler:
FLASH_IRQHandler:
RCC_IRQHandler:
EXTI7_0_IRQHandler:
AWU_IRQHandler:
DMA1_Channel1_IRQHandler:
DMA1_Channel2_IRQHandler:
DMA1_Channel3_IRQHandler:
DMA1_Channel4_IRQHandler:
DMA1_Channel5_IRQHandler:
DMA1_Channel6_IRQHandler:
DMA1_Channel7_IRQHandler:
ADC_IRQHandler:
I2C1_EV_IRQHandler:
I2C1_ER_IRQHandler:
USART1_IRQHandler:
EXTI15_8_IRQHandler:
TIM1_BRK_IRQHandler:
TIM1_UP_IRQHandler:
TIM1_TRG_COM_IRQHandler:
TIM1_CC_IRQHandler:
TIM2_IRQHandler:
USBPD0_IRQHandler:
USBPD0WakeUp_IRQHandler:
USBPD1_IRQHandler:
USBPD1WakeUp_IRQHandler:
TIM3_IRQHandler:
SPI1_IRQHandler:
USBFS_IRQHandler:
USBFSWakeUp_IRQHandler:
1:
	j 1b

	.section	.text.handle_reset,"ax",@progbits
	.weak	handle_reset
	.align	1
handle_reset:
.option push 
.option	norelax 
      csrw mepc, t0
	la gp, __global_pointer$
.option	pop 
1:
	la sp, _eusrstack 
2:
/* Load data section from flash to RAM */
	la a0, _data_lma
	la a1, _data_vma
	la a2, _edata
	bgeu a1, a2, 2f
1:
	lw t0, (a0)
	sw t0, (a1)
	addi a0, a0, 4
	addi a1, a1, 4
	bltu a1, a2, 1b
2:
/* Clear bss section */
	la a0, _sbss
	la a1, _ebss
	bgeu a0, a1, 2f
1:
	sw zero, (a0)
	addi a0, a0, 4
	bltu a0, a1, 1b
2:
/* Enabale MIE mapping and prefetch */
    li t0, 0x21
    csrw 0xbc0, t0
/* Enable interrupt nesting and hardware stack */
	li t0, 0xf
	csrw 0x804, t0
	li t0, 0x1
	csrw 0xbc1, t0
/* Enable global interrupt and configure privileged mode */
  	li t0, 0x1800
  	csrw mstatus, t0
/* Configure entry address mode */
	la t0, _start
  	ori t0, t0, 3
	csrw mtvec, t0

  	jal  SystemInit
	la t0, main
	csrw mepc, t0
	mret


