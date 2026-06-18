/********************************** (C) COPYRIGHT *******************************
* File Name          : startup_ch32x3x5.s
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/03/01
* Description        : CH32X3x5 vector table for eclipse toolchain.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

	.section	.init,"ax",@progbits
	.global	_start
	.align	1
_start:

    .option norvc;
        j   handle_reset
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
    .word   EXTI15_8_IRQHandler        /* EXTI Line [15:8] */
    .word   FLASH_IRQHandler           /* Flash */
    .word   RCC_IRQHandler             /* RCC */
    .word   EXTI7_0_IRQHandler         /* EXTI Line [7:0] */
    .word   SPI2_IRQHandler            /* SPI2 */
    .word   DMA1_Channel1_IRQHandler   /* DMA1 Channel 1 */
    .word   DMA1_Channel2_IRQHandler   /* DMA1 Channel 2 */
    .word   DMA1_Channel3_IRQHandler   /* DMA1 Channel 3 */
    .word   DMA1_Channel4_IRQHandler   /* DMA1 Channel 4 */
    .word   DMA1_Channel5_IRQHandler   /* DMA1 Channel 5 */
    .word   DMA1_Channel6_IRQHandler   /* DMA1 Channel 6 */
    .word   DMA1_Channel7_IRQHandler   /* DMA1 Channel 7 */
    .word   DMA1_Channel8_IRQHandler   /* DMA1 Channel 8 */
    .word   DMA1_Channel9_IRQHandler   /* DMA1 Channel 9 */
    .word   DMA1_Channel10_IRQHandler  /* DMA1 Channel 10 */
    .word   DMA1_Channel11_IRQHandler  /* DMA1 Channel 11 */
    .word   USART2_IRQHandler          /* USART2 */
    .word   I2C1_EV_IRQHandler         /* I2C1 Event */
    .word   I2C1_ER_IRQHandler         /* I2C1 Error */
    .word   USART1_IRQHandler          /* USART1 */
    .word   SPI1_IRQHandler            /* SPI1 */
    .word   TIM1_UP_IRQHandler         /* TIM1 Update */
    .word   TIM1_CC_IRQHandler         /* TIM1 Capture Compare */
    .word   TIM1_TRG_COM_IRQHandler    /* TIM1 Trigger and Commutation */
    .word   TIM1_BRK_IRQHandler        /* TIM1 Break */
    .word   TIM2_IRQHandler            /* TIM2 */
    .word   I2C2_EV_IRQHandler         /* I2C2 Event */
    .word   I2C2_ER_IRQHandler         /* I2C2 Error */
    .word   USBPD_IRQHandler           /* USBPD */
    .word   USBPDWakeUp_IRQHandler     /* USBPD Wakeup */
    .word   ARGB_IRQHandler            /* ARGB */
    .word   TIM3_IRQHandler            /* TIM3 */
    .word   TIM4_IRQHandler            /* TIM4 */
    .word   SPI3_IRQHandler            /* SPI3 */
    .word   USART3_IRQHandler          /* USART3 */
    .word   USART4_IRQHandler          /* USART4 */
    .word   USBSS_IRQHandler           /* USBSS */
    .word   USBSS_LINK_IRQHandler      /* USBSS LINK */
    .word   USBHS_IRQHandler           /* USBHS */
    .word   USBHSWakeup_IRQHandler     /* USBHS Wakeup */
    .word   USBSSWakeup_IRQHandler     /* USBSS Wakeup */
    .word   PVD_IRQHandler             /* PVD */
    .word   ADC1_IRQHandler            /* ADC1 */
    .word   ADC2_IRQHandler            /* ADC2 */
    .word   ADC3_IRQHandler            /* ADC3 */
    .word   RTCAlarm_IRQHandler        /* RTCAlarm */
    .word   RTC_IRQHandler             /* RTC */
    .word   USART_WKUP_IRQHandler      /* USART WKUP*/
    .word   ADC4_IRQHandler            /* ADC4 */

    .option rvc;
    .section    .text.vector_handler, "ax", @progbits
    .weak   NMI_Handler                /* NMI */
    .weak   HardFault_Handler          /* Hard Fault */
    .weak   Ecall_M_Mode_Handler       /* Ecall M Mode */
    .weak   Ecall_U_Mode_Handler       /* Ecall U Mode */
    .weak   Break_Point_Handler        /* Break Point */
    .weak   SysTick_Handler            /* SysTick */
    .weak   SW_Handler                 /* SW */
    .weak   WWDG_IRQHandler            /* Window Watchdog */
    .weak   EXTI15_8_IRQHandler        /* EXTI Line [15:8] */
    .weak   FLASH_IRQHandler           /* Flash */
    .weak   RCC_IRQHandler             /* RCC */
    .weak   EXTI7_0_IRQHandler         /* EXTI Line [7:0] */
    .weak   SPI2_IRQHandler            /* SPI2 */
    .weak   DMA1_Channel1_IRQHandler   /* DMA1 Channel 1 */
    .weak   DMA1_Channel2_IRQHandler   /* DMA1 Channel 2 */
    .weak   DMA1_Channel3_IRQHandler   /* DMA1 Channel 3 */
    .weak   DMA1_Channel4_IRQHandler   /* DMA1 Channel 4 */
    .weak   DMA1_Channel5_IRQHandler   /* DMA1 Channel 5 */
    .weak   DMA1_Channel6_IRQHandler   /* DMA1 Channel 6 */
    .weak   DMA1_Channel7_IRQHandler   /* DMA1 Channel 7 */
    .weak   DMA1_Channel8_IRQHandler   /* DMA1 Channel 8 */
    .weak   DMA1_Channel9_IRQHandler   /* DMA1 Channel 9 */
    .weak   DMA1_Channel10_IRQHandler  /* DMA1 Channel 10 */
    .weak   DMA1_Channel11_IRQHandler  /* DMA1 Channel 11 */
    .weak   USART2_IRQHandler          /* USART2 */
    .weak   I2C1_EV_IRQHandler         /* I2C1 Event */
    .weak   I2C1_ER_IRQHandler         /* I2C1 Error */
    .weak   USART1_IRQHandler          /* USART1 */
    .weak   SPI1_IRQHandler            /* SPI1 */
    .weak   TIM1_UP_IRQHandler         /* TIM1 Update */
    .weak   TIM1_CC_IRQHandler         /* TIM1 Capture Compare */
    .weak   TIM1_TRG_COM_IRQHandler    /* TIM1 Trigger and Commutation */
    .weak   TIM1_BRK_IRQHandler        /* TIM1 Break */
    .weak   TIM2_IRQHandler            /* TIM2 */
    .weak   I2C2_EV_IRQHandler         /* I2C2 Event */
    .weak   I2C2_ER_IRQHandler         /* I2C2 Error */
    .weak   USBPD_IRQHandler           /* USBPD */
    .weak   USBPDWakeUp_IRQHandler     /* USBPD Wakeup */
    .weak   ARGB_IRQHandler            /* ARGB */
    .weak   TIM3_IRQHandler            /* TIM3 */
    .weak   TIM4_IRQHandler            /* TIM4 */
    .weak   SPI3_IRQHandler            /* SPI3 */
    .weak   USART3_IRQHandler          /* USART3 */
    .weak   USART4_IRQHandler          /* USART4 */
    .weak   USBSS_IRQHandler           /* USBSS */
    .weak   USBSS_LINK_IRQHandler      /* USBSS LINK */
    .weak   USBHS_IRQHandler           /* USBHS */
    .weak   USBHSWakeup_IRQHandler     /* USBHS Wakeup */
    .weak   USBSSWakeup_IRQHandler     /* USBSS Wakeup */
    .weak   PVD_IRQHandler             /* PVD */
    .weak   ADC1_IRQHandler            /* ADC1 */
    .weak   ADC2_IRQHandler            /* ADC2 */
    .weak   ADC3_IRQHandler            /* ADC3 */
    .weak   RTCAlarm_IRQHandler        /* RTCAlarm */
    .weak   RTC_IRQHandler             /* RTC */
    .weak   USART_WKUP_IRQHandler      /* USART WKUP*/
    .weak   ADC4_IRQHandler            /* ADC4 */


NMI_Handler:                
HardFault_Handler:          
Ecall_M_Mode_Handler:       
Ecall_U_Mode_Handler:       
Break_Point_Handler:        
SysTick_Handler:            
SW_Handler:                 
WWDG_IRQHandler:            
EXTI15_8_IRQHandler:        
FLASH_IRQHandler:           
RCC_IRQHandler:             
EXTI7_0_IRQHandler:         
SPI2_IRQHandler:            
DMA1_Channel1_IRQHandler:   
DMA1_Channel2_IRQHandler:   
DMA1_Channel3_IRQHandler:   
DMA1_Channel4_IRQHandler:   
DMA1_Channel5_IRQHandler:   
DMA1_Channel6_IRQHandler:   
DMA1_Channel7_IRQHandler:   
DMA1_Channel8_IRQHandler:   
DMA1_Channel9_IRQHandler:   
DMA1_Channel10_IRQHandler:  
DMA1_Channel11_IRQHandler:  
USART2_IRQHandler:          
I2C1_EV_IRQHandler:         
I2C1_ER_IRQHandler:         
USART1_IRQHandler:          
SPI1_IRQHandler:            
TIM1_UP_IRQHandler:         
TIM1_CC_IRQHandler:         
TIM1_TRG_COM_IRQHandler:    
TIM1_BRK_IRQHandler:        
TIM2_IRQHandler:            
I2C2_EV_IRQHandler:         
I2C2_ER_IRQHandler:         
USBPD_IRQHandler:           
USBPDWakeUp_IRQHandler:     
ARGB_IRQHandler:            
TIM3_IRQHandler:            
TIM4_IRQHandler:            
SPI3_IRQHandler:            
USART3_IRQHandler:          
USART4_IRQHandler:          
USBSS_IRQHandler:           
USBSS_LINK_IRQHandler:      
USBHS_IRQHandler:           
USBHSWakeup_IRQHandler:     
USBSSWakeup_IRQHandler:     
PVD_IRQHandler:             
ADC1_IRQHandler:            
ADC2_IRQHandler:            
ADC3_IRQHandler:            
RTCAlarm_IRQHandler:        
RTC_IRQHandler:             
USART_WKUP_IRQHandler:      
ADC4_IRQHandler:            
1:
	j 1b

	.section	.text.handle_reset,"ax",@progbits
	.weak	handle_reset
	.align	1
handle_reset:
.option push
.option	norelax
	la gp, __global_pointer$
.option	pop

	la sp, _eusrstack

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

/* Configure Prefetch */
    li t0, 0x123703E1
    csrw 0xBC0, t0
/* Configure nesting level */
	li t0, 0x01
	csrw 0xBC1, t0
/* Enable interrupt nesting and hardware stack */
    li t0, 0x1f
    csrw 0x804, t0
/* Enable floating point and global interrupt, configure privileged mode */
   	li t0, 0x7800
   	csrw mstatus, t0
/* Configure the interrupt vector table recognition mode and entry address mode */
 	la t0, _start
    ori t0, t0, 3           
	csrw mtvec, t0

    jal  SystemInit
	la t0, main
	csrw mepc, t0
	mret


