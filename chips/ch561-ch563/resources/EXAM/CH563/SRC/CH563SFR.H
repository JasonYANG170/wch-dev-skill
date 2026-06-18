/********************************** (C) COPYRIGHT *******************************
* File Name          : CH563SFR.H
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : SpecialFunctionRegister
*******************************************************************************/



/******************************************************************************/
#include "CH563BAS.H"
#ifndef __CH563SFR_H__
#define __CH563SFR_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/

// Address Space
//   CODE:   00000000H - 000FFFFFH, 1MB
//    SFR:   00400000H - 0040FFFFH, 64KB
//   DATA:   00808000H - 0081FFFFH, 96KB/64KB/32KB
//   XBUS:   00C00000H - 00CFFFFFH, 1MB
//  OTHER:   00E00000H - FFFFFFFFH, undefined
//
//    SFR:   00400000H - 0040FFFFH, 64KB
//      SYS:    +1000H - 1FFFH, include base config, interrupt, GPIO, etc...
//      USB:    +4000H - 5FFFH
//      ETH:    +6000H - 7FFFH
//      TMR0:   +8000H - 83FFH
//      TMR1:   +8400H - 87FFH
//      TMR2:   +8800H - 8BFFH
//      TMR3:   +8C00H - 8FFFH
//      ADC:    +A000H - AFFFH
//      SPI0:   +C000H - C7FFH
//      SPI1:   +C800H - CFFFH
//      UART0:  +D000H - D7FFH
//      UART1:  +D800H - DFFFH

// Register Bit Attribute / Bit Access Type
//   RF:    Read only for Fixed value
//   RO:    Read Only (internal change)
//   RZ:    Read only with auto clear Zero
//   WO:    Write Only (read zero or different)
//   WA:    Write only under safe Accessing mode (read zero or different)
//   WZ:    Write only with auto clear Zero
//   RW:    Read / Write
//   RWA:   Read / Write under safe Accessing mode
//   RW1:   Read / Write 1 to Clear

/* Register name rule:
   R32_* for 32 bits register (UINT32,ULONG)
   R16_* for 16 bits register (UINT16,USHORT)
   R8_*  for  8 bits register (UINT8,UCHAR)
   RB_*  for bit or bit mask of 8 bit register
   BA_*  for base address point
   Others for register address offset */

/* ********************************************************************************************************************* */

/* System: safe accessing register */
#define R32_SAFE_ACCESS     (*((PUINT32V)0x00400000)) // RW, safe accessing
#define R8_SAFE_ACCESS_SIG  (*((PUINT8V)0x00400000))  // WO, safe accessing sign register, must write 0x57 then 0xA8 to enter safe accessing mode
#define  RB_SAFE_ACC_MODE   0x03                      // RO, current safe accessing mode: 11=safe/unlocked (SAM), other=locked (00..01..10..11)
#define  RB_SAFE_ACC_TIMER  0x70                      // RO, safe accessing timer bit mask (16*clock number)
#define R8_SAFE_ACCESS_ID   (*((PUINT8V)0x00400002))  // RF, safe accessing ID register, always 0x01
#define R8_WDOG_CLEAR       (*((PUINT8V)0x00400003))  // WO, clear watch-dog

/* System: global configuration register */
#define R32_GLOBAL_CONFIG   (*((PUINT32V)0x00400004)) // RW, global configuration
#define R8_GLOB_MEM_CFG     (*((PUINT8V)0x00400004))  // RWA, global memory configuration, SAM and bit7:6 must write 1:0
#define  RB_GLOB_MEM_CFG    0x03                      // RWA, global memory config: 10=C96K/D32K, 11=C32K/D96K, 00/01=C64K/D64K
#define  RB_GLOB_BOOT_APP   0x10                      // RO, boot loader action status
#define  RB_GLOB_CFG_FLAG   0x80                      // RO, global config flag
#define R8_GLOB_LOCK_PORT   (*((PUINT8V)0x00400005))  // RWA, lock port configuration, SAM and bit7:6 must write 0:0
#define  RB_GLOB_LOCK_PA    0x01                      // RWA, lock GPIO PA
#define  RB_GLOB_LOCK_PB    0x02                      // RWA, lock GPIO PB
#define  RB_GLOB_LOCK_PD    0x08                      // RWA, lock GPIO PD
#define R8_GLOB_RST_CFG     (*((PUINT8V)0x00400006))  // RWA, global reset configuration, SAM and bit7:6 must write 0:1
#define  RB_GLOB_FORCE_RST  0x01                      // WA/WZ, force global reset, high action, auto clear
#define  RB_GLOB_WDOG_EN    0x02                      // RWA, watch-dog enable
#define  RB_GLOB_DEBUG_EN   0x20                      // RO, debug enable status
#define R8_GLOB_RESET_KEEP  (*((PUINT8V)0x00400007))  // RW, value keeper during global reset

/* System: PLL configuration register */
#define R32_PLL_CONFIG      (*((PUINT32V)0x00400008)) // RWA, PLL configuration, SAM
#define R8_PLL_EXT_CFG      (*((PUINT8V)0x00400008))  // RWA, PLL external config, SAM and bit7:6 must write 0:1
#define  RB_PLL_XT_FREQ     0x04                      // RWA, PLL external crystal frequency (USB clock): 1=30MHz, 0=12MHz
#define  RB_PLL_ETH_OSC     0x08                      // RWA, PLL ethernet clock source: 0=PLL, 1=external crystal
#define R8_PLL_OUT_DIV      (*((PUINT8V)0x00400009))  // RWA, PLL output clock divider, SAM and bit7:6 must write 1:0
#define  RB_PLL_ETH_DIV     0x0F                      // RWA, PLL ethernet clock divider
#define  RB_PLL_SYS_DIV     0x30                      // RWA, PLL system clock divider
#define R8_PLL_PRE_DIV      (*((PUINT8V)0x0040000A))  // RWA, PLL pre-divider, SAM, only low 3 bit
#define R8_PLL_LOOP_DIV     (*((PUINT8V)0x0040000B))  // RWA, PLL loop divider, SAM, only low 6 bit
// REF_FREQ = RB_PLL_XT_FREQ / R8_PLL_PRE_DIV, 5MHz ~ 30MHz
// VCO_FREQ = REF_FREQ * R8_PLL_LOOP_DIV, 100MHz ~ 300MHz
// ETH_FREQ = VCO_FREQ / RB_PLL_ETH_DIV, 25MHz
// SYS_FREQ = VCO_FREQ / RB_PLL_SYS_DIV, 20MHz ~ 125MHz
// default: VCO_FREQ = XT_30MHz / 3 * 25 = 250MHz, SYS_FREQ = VCO_FREQ / 4 = 62.5MHz
//          VCO_FREQ = XT_12MHz / 3 * 25 = 100MHz, SYS_FREQ = VCO_FREQ / 4 = 25MHz

/* System: sleep control register */
#define R32_SLEEP_CONTROL   (*((PUINT32V)0x0040000C)) // RWA, sleep control, SAM
#define R8_SLP_CLK_OFF0     (*((PUINT8V)0x0040000C))  // RWA, sleep clock off control byte 0, SAM
#define  RB_SLP_CLK_TMR0    0x01                      // RWA, sleep TMR0 clock
#define  RB_SLP_CLK_TMR1    0x02                      // RWA, sleep TMR1 clock
#define  RB_SLP_CLK_TMR2    0x04                      // RWA, sleep TMR2 clock
#define  RB_SLP_CLK_TMR3    0x08                      // RWA, sleep TMR3 clock
#define  RB_SLP_CLK_SPI0    0x10                      // RWA, sleep SPI0 clock
#define  RB_SLP_CLK_SPI1    0x20                      // RWA, sleep SPI1 clock
#define  RB_SLP_CLK_UART0   0x40                      // RWA, sleep UART0 clock
#define  RB_SLP_CLK_UART1   0x80                      // RWA, sleep UART1 clock
#define R8_SLP_CLK_OFF1     (*((PUINT8V)0x0040000D))  // RWA, sleep clock off control byte 1, SAM
#define  RB_SLP_CLK_ADC     0x10                      // RWA, sleep ADC clock
#define  RB_SLP_CLK_GPIO    0x20                      // RWA, sleep GPIO clock
#define  RB_SLP_CLK_USB     0x40                      // RWA, sleep USB clock
#define  RB_SLP_CLK_ETH     0x80                      // RWA, sleep ethernet clock
#define R8_SLP_WAKE_CTRL    (*((PUINT8V)0x0040000E))  // RWA, wake control, SAM
#define  RB_SLP_PA_WAKE     0x01                      // RWA, enable GPIO PA waking
#define  RB_SLP_PB_WAKE     0x02                      // RWA, enable GPIO PB waking
#define  RB_SLP_PD_WAKE     0x08                      // RWA, enable GPIO PD waking
#define  RB_SLP_USB_WAKE    0x10                      // RWA, enable USB waking
#define  RB_SLP_AP_WAK_USB  0x20                      // RWA, application force wake USB
#define  RB_SLP_WOL_WAKE    0x40                      // RWA, enable ethernet WOL waking
#define  RB_SLP_ETH_PWR_DN  0x80                      // RWA, enable power down for ethernet
#define R8_SLP_CTRL_PLL     (*((PUINT8V)0x0040000F))  // WA, PLL sleep control, SAM and write 0x6A to sleep CPU or write 0x95 to sleep PLL
#define R8_SLP_STATUS       (*((PUINT8V)0x0040000F))  // RO, sleep status
#define  RB_SLP_WOL_STATUS  0x01                      // RO, current ethernet WOL status
#define  RB_SLP_CPU_STATUS  0x40                      // RO, current CPU sleep status
#define  RB_SLP_PLL_STATUS  0x80                      // RO, current PLL sleep status

/* System: external bus configuration register */
#define R32_EXT_BUS_CFG     (*((PUINT32V)0x00400010)) // RW, external bus configuration
#define R8_XBUS_CONFIG      (*((PUINT8V)0x00400010))  // RW, external bus configuration
#define  RB_XBUS_ENABLE     0x01                      // RWA, external bus enable
#define  RB_XBUS_EN_32BIT   0x02                      // RW, enable 16bit or 32bit external bus
#define  RB_XBUS_ADDR_OE    0x0C                      // RWA, bus address output enable: 00=none,01=PA[5:0],10=PA[11:0],11=PA[19:0]
#define R8_XBUS_CYCLE       (*((PUINT8V)0x00400012))  // RW, external bus total cycle (clock number), only low 5 bit
#define R8_XBUS_SETUP_HOLD  (*((PUINT8V)0x00400013))  // RW, external bus setup and hold config
#define  RB_XBUS_HOLD       0x1F                      // RW, external bus hold time bit mask (clock number)
#define  RB_XBUS_SETUP      0x80                      // RW, external bus setup time: 0=1 clock, 1=2 clocks

/* System: parallel slave configuration register */
#define R32_PARA_SLV_CFG    (*((PUINT32V)0x00400014)) // RW, parallel slave configuration
#define R8_SLV_CONFIG       (*((PUINT8V)0x00400014))  // RWA, parallel slave configuration, SAM
#define  RB_SLV_ENABLE      0x01                      // RWA, parallel slave enable
#define  RB_SLV_IE_CMD      0x02                      // RWA, enable interrupt for slave writing command event
#define  RB_SLV_IE_WR       0x04                      // RWA, enable interrupt for slave writing event
#define  RB_SLV_IE_RD       0x08                      // RWA, enable interrupt for slave reading event
#define R8_SLV_DOUT         (*((PUINT8V)0x00400016))  // RW, parallel slave data to output
#define R8_SLV_STATUS       (*((PUINT8V)0x00400017))  // RW, parallel slave status to output, only low 7 bit

/* System: miscell control register */
#define R32_MISCELL_CTRL    (*((PUINT32V)0x0040001C)) // RW, miscell control
#define R8_MISC_CTRL_ETH    (*((PUINT8V)0x0040001C))  // RW, miscell control for ethernet
#define  RB_MISC_ETH_RX     0x01                      // RW, miscell control ethernet rx LED enable
#define  RB_MISC_ETH_TX     0x02                      // RW, miscell control ethernet tx LED enable
#define  RB_MISC_ETH_LED    0x04                      // RW, miscell control ethernet link LED enable
#define  RB_MISC_ETH_RST    0x80                      // RW, miscell control ethernet transceiver reset
#define R8_MISC_CTRL_USB    (*((PUINT8V)0x0040001D))  // RW, miscell control for USB
#define  RB_MISC_USB_VBUS   0x01                      // RW, miscell control USB OTG VBUS pin enable
#define  RB_MISC_USB_ID_EN  0x02                      // RW, miscell control USB OTG ID pin enable
#define  RB_MISC_USB_ID_ST  0x08                      // RW, miscell control USB OTG ID setting value
#define R8_MISC_CTRL_OTHER  (*((PUINT8V)0x0040001E))  // RW, miscell control for others
#define  RB_MISC_PECL_EN    0x01                      // RW, miscell control PECL input enable

/* Interrupt vector register */
#define R32_INT_VEC_CTRL    (*((PUINT32V)0x00400020)) // RWA, interrupt vector control, SAM
#define R8_INT_VEC_IRQ      (*((PUINT8V)0x00400020))  // RWA, IRQ normal interrupt vector control, SAM
#define  RB_IV_IRQ_TMR0     0x01                      // RWA, TMR0 IRQ vector enable
#define  RB_IV_IRQ_SPI0     0x02                      // RWA, SPI0 IRQ vector enable
#define  RB_IV_IRQ_PB       0x04                      // RWA, GPIO PB IRQ vector enable
#define R8_INT_VEC_FIQ      (*((PUINT8V)0x00400021))  // RWA, FIQ fast interrupt vector control, SAM
#define  RB_IV_FIQ_TMR0     0x01                      // RWA, TMR0 FIQ vector enable
#define  RB_IV_FIQ_SPI0     0x02                      // RWA, SPI0 FIQ vector enable
#define  RB_IV_FIQ_PB       0x04                      // RWA, GPIO PB FIQ vector enable
#define R32_INT_VEC_TMR0    (*((PUINT32V)0x00400024)) // RWA, interrupt vector for TMR0, SAM
#define R32_INT_VEC_SPI0    (*((PUINT32V)0x00400028)) // RWA, interrupt vector for SPI0, SAM
#define R32_INT_VEC_PB      (*((PUINT32V)0x0040002C)) // RWA, interrupt vector for GPIO PB, SAM

/* generate interrupt vector data */
#define MAKE_INT_VEC_IRQ( IrqHand )  (0xEA000000|((UINT32)IrqHand-0x20>>2))  // generate IRQ interrupt vector data
#define MAKE_INT_VEC_FIQ( FiqHand )  (0xEA000000|((UINT32)FiqHand-0x24>>2))  // generate FIQ interrupt vector data

/* Interrupt flag register */
#define R32_INT_FLAG        (*((PUINT32V)0x00400040)) // RW, interrupt flag
#define R8_INT_FLAG_0       (*((PUINT8V)0x00400040))  // RO, interrupt flag byte 0
#define  RB_IF_TMR0         0x01                      // RO, interrupt flag of TMR0
#define  RB_IF_TMR1         0x02                      // RO, interrupt flag of TMR1
#define  RB_IF_TMR2         0x04                      // RO, interrupt flag of TMR2
#define  RB_IF_TMR3         0x08                      // RO, interrupt flag of TMR3
#define  RB_IF_SPI0         0x10                      // RO, interrupt flag of SPI0
#define  RB_IF_SPI1         0x20                      // RO, interrupt flag of SPI1
#define  RB_IF_UART0        0x40                      // RO, interrupt flag of UART0
#define  RB_IF_UART1        0x80                      // RO, interrupt flag of UART1
#define R8_INT_FLAG_1       (*((PUINT8V)0x00400041))  // RO, interrupt flag byte 1
#define  RB_IF_PA           0x01                      // RO, interrupt flag of GPIO PA
#define  RB_IF_PB           0x02                      // RO, interrupt flag of GPIO PB
#define  RB_INT_WOL_STATUS  0x04                      // RO, current ethernet WOL status
#define  RB_IF_PD           0x08                      // RO, interrupt flag of GPIO PD
#define  RB_IF_ADC          0x10                      // RO, interrupt flag of ADC
#define  RB_IF_SLV          0x20                      // RO, interrupt flag of parallel slave
#define  RB_IF_USB          0x40                      // RO, interrupt flag of USB
#define  RB_IF_ETH          0x80                      // RO, interrupt flag of ethernet
#define R8_INT_FLAG_SLV     (*((PUINT8V)0x00400042))  // RW1, parallel slave interrupt flag
#define  RB_IF_SLV_CMD0     0x10                      // RO, parallel slave command raw flag
#define  RB_IF_SLV_CMD      0x20                      // RO, parallel slave command synchro flag
#define  RB_IF_SLV_WR       0x40                      // RW1, interrupt flag of parallel slave writing event
#define  RB_IF_SLV_RD       0x80                      // RW1, interrupt flag of parallel slave reading event
#define R8_INT_SLV_DIN      (*((PUINT8V)0x00400043))  // RO, parallel slave data input

/* IRQ interrupt enable register */
#define R32_INT_EN_IRQ      (*((PUINT32V)0x00400048)) // RW, IRQ interrupt enable
#define R8_INT_EN_IRQ_0     (*((PUINT8V)0x00400048))  // RW, IRQ interrupt enable byte 0
#define  RB_IE_IRQ_TMR0     0x01                      // RW, IRQ interrupt enable for TMR0
#define  RB_IE_IRQ_TMR1     0x02                      // RW, IRQ interrupt enable for TMR1
#define  RB_IE_IRQ_TMR2     0x04                      // RW, IRQ interrupt enable for TMR2
#define  RB_IE_IRQ_TMR3     0x08                      // RW, IRQ interrupt enable for TMR3
#define  RB_IE_IRQ_SPI0     0x10                      // RW, IRQ interrupt enable for SPI0
#define  RB_IE_IRQ_SPI1     0x20                      // RW, IRQ interrupt enable for SPI1
#define  RB_IE_IRQ_UART0    0x40                      // RW, IRQ interrupt enable for UART0
#define  RB_IE_IRQ_UART1    0x80                      // RW, IRQ interrupt enable for UART1
#define R8_INT_EN_IRQ_1     (*((PUINT8V)0x00400049))  // RW, IRQ interrupt enable byte 1
#define  RB_IE_IRQ_PA       0x01                      // RW, IRQ interrupt enable for GPIO PA
#define  RB_IE_IRQ_PB       0x02                      // RW, IRQ interrupt enable for GPIO PB
#define  RB_IE_IRQ_PD       0x08                      // RW, IRQ interrupt enable for GPIO PD
#define  RB_IE_IRQ_ADC      0x10                      // RW, IRQ interrupt enable for ADC
#define  RB_IE_IRQ_SLV      0x20                      // RW, IRQ interrupt enable for parallel slave
#define  RB_IE_IRQ_USB      0x40                      // RW, IRQ interrupt enable for USB
#define  RB_IE_IRQ_ETH      0x80                      // RW, IRQ interrupt enable for ethernet
#define R8_INT_EN_IRQ_GLOB  (*((PUINT8V)0x0040004B))  // RW, IRQ global interrupt enable
#define  RB_IE_IRQ_WOL      0x40                      // RW, IRQ interrupt enable for ethernet WOL, independent of global interrupt enable
#define  RB_IE_IRQ_GLOB     0x80                      // RW, IRQ global interrupt enable

/* FIQ interrupt enable register */
#define R32_INT_EN_FIQ      (*((PUINT32V)0x0040004C)) // RW, FIQ interrupt enable
#define R8_INT_EN_FIQ_0     (*((PUINT8V)0x0040004C))  // RW, FIQ interrupt enable byte 0
#define  RB_IE_FIQ_TMR0     0x01                      // RW, FIQ interrupt enable for TMR0
#define  RB_IE_FIQ_TMR1     0x02                      // RW, FIQ interrupt enable for TMR1
#define  RB_IE_FIQ_TMR2     0x04                      // RW, FIQ interrupt enable for TMR2
#define  RB_IE_FIQ_TMR3     0x08                      // RW, FIQ interrupt enable for TMR3
#define  RB_IE_FIQ_SPI0     0x10                      // RW, FIQ interrupt enable for SPI0
#define  RB_IE_FIQ_SPI1     0x20                      // RW, FIQ interrupt enable for SPI1
#define  RB_IE_FIQ_UART0    0x40                      // RW, FIQ interrupt enable for UART0
#define  RB_IE_FIQ_UART1    0x80                      // RW, FIQ interrupt enable for UART1
#define R8_INT_EN_FIQ_1     (*((PUINT8V)0x0040004D))  // RW, FIQ interrupt enable byte 1
#define  RB_IE_FIQ_PA       0x01                      // RW, FIQ interrupt enable for GPIO PA
#define  RB_IE_FIQ_PB       0x02                      // RW, FIQ interrupt enable for GPIO PB
#define  RB_IE_FIQ_PD       0x08                      // RW, FIQ interrupt enable for GPIO PD
#define  RB_IE_FIQ_ADC      0x10                      // RW, FIQ interrupt enable for ADC
#define  RB_IE_FIQ_SLV      0x20                      // RW, FIQ interrupt enable for parallel slave
#define  RB_IE_FIQ_USB      0x40                      // RW, FIQ interrupt enable for USB
#define  RB_IE_FIQ_ETH      0x80                      // RW, FIQ interrupt enable for ethernet
#define R8_INT_EN_FIQ_GLOB  (*((PUINT8V)0x0040004F))  // RW, FIQ global interrupt enable
#define  RB_IE_FIQ_GLOB     0x80                      // RW, FIQ global interrupt enable

/* Interrupt enable register address offset and bit define */
#define BA_INT_FLAG         ((PUINT8V)0x00400040)     // point interrupt flag base address
#define BA_IE_IRQ           ((PUINT8V)0x00400048)     // point IRQ interrupt enable base address
#define BA_IE_FIQ           ((PUINT8V)0x0040004C)     // point FIQ interrupt enable base address
#define IE_BYTE_0           0
#define  RB_IE_TMR0         0x01                      // RW, interrupt enable for TMR0
#define  RB_IE_TMR1         0x02                      // RW, interrupt enable for TMR1
#define  RB_IE_TMR2         0x04                      // RW, interrupt enable for TMR2
#define  RB_IE_TMR3         0x08                      // RW, interrupt enable for TMR3
#define  RB_IE_SPI0         0x10                      // RW, interrupt enable for SPI0
#define  RB_IE_SPI1         0x20                      // RW, interrupt enable for SPI1
#define  RB_IE_UART0        0x40                      // RW, interrupt enable for UART0
#define  RB_IE_UART1        0x80                      // RW, interrupt enable for UART1
#define IE_BYTE_1           1
#define  RB_IE_PA           0x01                      // RW, interrupt enable for GPIO PA
#define  RB_IE_PB           0x02                      // RW, interrupt enable for GPIO PB
#define  RB_IE_PD           0x08                      // RW, interrupt enable for GPIO PD
#define  RB_IE_ADC          0x10                      // RW, interrupt enable for ADC
#define  RB_IE_SLV          0x20                      // RW, interrupt enable for parallel slave
#define  RB_IE_USB          0x40                      // RW, interrupt enable for USB
#define  RB_IE_ETH          0x80                      // RW, interrupt enable for ethernet
#define IE_BYTE_GLOB        3
#define  RB_IE_WOL          0x40                      // RW, IRQ interrupt enable for ethernet WOL, independent of global interrupt enable
#define  RB_IE_GLOB         0x80                      // RW, global interrupt enable

/* GPIO PA interrupt register */
#define R32_INT_STATUS_PA   (*((PUINT32V)0x00400050)) // RW1, GPIO PA interrupt flag
#define R8_INT_STATUS_PA_1  (*((PUINT8V)0x00400051))  // RW1, GPIO PA interrupt flag byte 1
#define R8_INT_STATUS_PA_2  (*((PUINT8V)0x00400052))  // RW1, GPIO PA interrupt flag byte 2
#define R32_INT_ENABLE_PA   (*((PUINT32V)0x00400054)) // RW, GPIO PA interrupt enable
#define R8_INT_ENABLE_PA_1  (*((PUINT8V)0x00400055))  // RW, GPIO PA interrupt enable byte 1
#define R8_INT_ENABLE_PA_2  (*((PUINT8V)0x00400056))  // RW, GPIO PA interrupt enable byte 2
#define R32_INT_MODE_PA     (*((PUINT32V)0x00400058)) // RW, GPIO PA interrupt mode: 0=level action, 1=edge action
#define R8_INT_MODE_PA_1    (*((PUINT8V)0x00400059))  // RW, GPIO PA interrupt mode byte 1
#define R8_INT_MODE_PA_2    (*((PUINT8V)0x0040005A))  // RW, GPIO PA interrupt mode byte 2
#define R32_INT_POLAR_PA    (*((PUINT32V)0x0040005C)) // RW, GPIO PA interrupt polarity: 0=normal/low level/fall edge, 1=invert/high level/rise edge
#define R8_INT_POLAR_PA_1   (*((PUINT8V)0x0040005D))  // RW, GPIO PA interrupt polarity byte 1
#define R8_INT_POLAR_PA_2   (*((PUINT8V)0x0040005E))  // RW, GPIO PA interrupt polarity byte 2

/* GPIO PB interrupt register */
#define R32_INT_STATUS_PB   (*((PUINT32V)0x00400060)) // RW1, GPIO PB interrupt flag
#define R8_INT_STATUS_PB_0  (*((PUINT8V)0x00400060))  // RW1, GPIO PB interrupt flag byte 0
#define R8_INT_STATUS_PB_1  (*((PUINT8V)0x00400061))  // RW1, GPIO PB interrupt flag byte 1
#define R8_INT_STATUS_PB_2  (*((PUINT8V)0x00400062))  // RW1, GPIO PB interrupt flag byte 2
#define R32_INT_ENABLE_PB   (*((PUINT32V)0x00400064)) // RW, GPIO PB interrupt enable
#define R8_INT_ENABLE_PB_0  (*((PUINT8V)0x00400064))  // RW, GPIO PB interrupt enable byte 0
#define R8_INT_ENABLE_PB_1  (*((PUINT8V)0x00400065))  // RW, GPIO PB interrupt enable byte 1
#define R8_INT_ENABLE_PB_2  (*((PUINT8V)0x00400066))  // RW, GPIO PB interrupt enable byte 2
#define R32_INT_MODE_PB     (*((PUINT32V)0x00400068)) // RW, GPIO PB interrupt mode: 0=level action, 1=edge action
#define R8_INT_MODE_PB_0    (*((PUINT8V)0x00400068))  // RW, GPIO PB interrupt mode byte 0
#define R8_INT_MODE_PB_1    (*((PUINT8V)0x00400069))  // RW, GPIO PB interrupt mode byte 1
#define R8_INT_MODE_PB_2    (*((PUINT8V)0x0040006A))  // RW, GPIO PB interrupt mode byte 2
#define R32_INT_POLAR_PB    (*((PUINT32V)0x0040006C)) // RW, GPIO PB interrupt polarity: 0=normal/low level/fall edge, 1=invert/high level/rise edge
#define R8_INT_POLAR_PB_0   (*((PUINT8V)0x0040006C))  // RW, GPIO PB interrupt polarity byte 0
#define R8_INT_POLAR_PB_1   (*((PUINT8V)0x0040006D))  // RW, GPIO PB interrupt polarity byte 1
#define R8_INT_POLAR_PB_2   (*((PUINT8V)0x0040006E))  // RW, GPIO PB interrupt polarity byte 2

/* GPIO PD interrupt register */
#define R32_INT_STATUS_PD   (*((PUINT32V)0x00400070)) // RW1, GPIO PD interrupt flag
#define R8_INT_STATUS_PD_0  (*((PUINT8V)0x00400070))  // RW1, GPIO PD interrupt flag byte 0
#define R8_INT_STATUS_PD_3  (*((PUINT8V)0x00400073))  // RW1, GPIO PD interrupt flag byte 3
#define R32_INT_ENABLE_PD   (*((PUINT32V)0x00400074)) // RW, GPIO PD interrupt enable
#define R8_INT_ENABLE_PD_0  (*((PUINT8V)0x00400074))  // RW, GPIO PD interrupt enable byte 0
#define R8_INT_ENABLE_PD_3  (*((PUINT8V)0x00400077))  // RW, GPIO PD interrupt enable byte 3
#define R32_INT_MODE_PD     (*((PUINT32V)0x00400078)) // RW, GPIO PD interrupt mode: 0=level action, 1=edge action
#define R8_INT_MODE_PD_0    (*((PUINT8V)0x00400078))  // RW, GPIO PD interrupt mode byte 0
#define R8_INT_MODE_PD_3    (*((PUINT8V)0x0040007B))  // RW, GPIO PD interrupt mode byte 3
#define R32_INT_POLAR_PD    (*((PUINT32V)0x0040007C)) // RW, GPIO PD interrupt polarity: 0=normal/low level/fall edge, 1=invert/high level/rise edge
#define R8_INT_POLAR_PD_0   (*((PUINT8V)0x0040007C))  // RW, GPIO PD interrupt polarity byte 0
#define R8_INT_POLAR_PD_3   (*((PUINT8V)0x0040007F))  // RW, GPIO PD interrupt polarity byte 3

/* GPIO interrupt register address offset and bit define */
#define BA_INT_PA           ((PUINT8V)0x00400050)     // point GPIO PA interrupt base address
#define BA_INT_PB           ((PUINT8V)0x00400060)     // point GPIO PB interrupt base address
#define BA_INT_PD           ((PUINT8V)0x00400070)     // point GPIO PD interrupt base address
#define INT_GPIO_STATUS     0x00
#define INT_GPIO_STATUS_0   0x00
#define INT_GPIO_STATUS_1   0x01
#define INT_GPIO_STATUS_2   0x02
#define INT_GPIO_ENABLE     0x04
#define INT_GPIO_ENABLE_0   0x04
#define INT_GPIO_ENABLE_1   0x05
#define INT_GPIO_ENABLE_2   0x06
#define INT_GPIO_MODE       0x08
#define INT_GPIO_MODE_0     0x08
#define INT_GPIO_MODE_1     0x09
#define INT_GPIO_MODE_2     0x0A
#define INT_GPIO_POLAR      0x0C
#define INT_GPIO_POLAR_0    0x0C
#define INT_GPIO_POLAR_1    0x0D
#define INT_GPIO_POLAR_2    0x0E

/* GPIO PA register */
#define R32_PA_DIR          (*((PUINT32V)0x00400080)) // RW, GPIO PA I/O direction: 0=in, 1=out
#define R8_PA_DIR_0         (*((PUINT8V)0x00400080))  // RW, GPIO PA I/O direction byte 0
#define R8_PA_DIR_1         (*((PUINT8V)0x00400081))  // RW, GPIO PA I/O direction byte 1
#define R8_PA_DIR_2         (*((PUINT8V)0x00400082))  // RW, GPIO PA I/O direction byte 2
#define R32_PA_PIN          (*((PUINT32V)0x00400084)) // RO, GPIO PA input
#define R8_PA_PIN_0         (*((PUINT8V)0x00400084))  // RO, GPIO PA input byte 0
#define R8_PA_PIN_1         (*((PUINT8V)0x00400085))  // RO, GPIO PA input byte 1
#define R8_PA_PIN_2         (*((PUINT8V)0x00400086))  // RO, GPIO PA input byte 2
#define R32_PA_OUT          (*((PUINT32V)0x00400088)) // RW, GPIO PA output
#define R8_PA_OUT_0         (*((PUINT8V)0x00400088))  // RW, GPIO PA output byte 0
#define R8_PA_OUT_1         (*((PUINT8V)0x00400089))  // RW, GPIO PA output byte 1
#define R8_PA_OUT_2         (*((PUINT8V)0x0040008A))  // RW, GPIO PA output byte 2
#define R32_PA_CLR          (*((PUINT32V)0x0040008C)) // WZ, GPIO PA clear output: 0=keep, 1=clear
#define R8_PA_CLR_0         (*((PUINT8V)0x0040008C))  // WZ, GPIO PA clear output byte 0
#define R8_PA_CLR_1         (*((PUINT8V)0x0040008D))  // WZ, GPIO PA clear output byte 1
#define R8_PA_CLR_2         (*((PUINT8V)0x0040008E))  // WZ, GPIO PA clear output byte 2
#define R32_PA_PU           (*((PUINT32V)0x00400090)) // RW, GPIO PA pullup resistance enable
#define R8_PA_PU_0          (*((PUINT8V)0x00400090))  // RW, GPIO PA pullup resistance enable byte 0
#define R8_PA_PU_1          (*((PUINT8V)0x00400091))  // RW, GPIO PA pullup resistance enable byte 1
#define R8_PA_PU_2          (*((PUINT8V)0x00400092))  // RW, GPIO PA pullup resistance enable byte 2
#define R32_PA_PD           (*((PUINT32V)0x00400094)) // RW, GPIO PA output open-drain & input pulldown resistance enable
#define R8_PA_PD_0          (*((PUINT8V)0x00400094))  // RW, GPIO PA output open-drain & input pulldown resistance enable byte 0
#define R8_PA_PD_1          (*((PUINT8V)0x00400095))  // RW, GPIO PA output open-drain & input pulldown resistance enable byte 1
#define R8_PA_PD_2          (*((PUINT8V)0x00400096))  // RW, GPIO PA output open-drain & input pulldown resistance enable byte 2
#define R32_PA_DRV          (*((PUINT32V)0x00400098)) // RW, GPIO PA driving capability: 0=4mA, 1=16mA
#define R8_PA_DRV_1         (*((PUINT8V)0x00400099))  // RW, GPIO PA driving capability byte 1
#define R8_PA_DRV_2         (*((PUINT8V)0x0040009A))  // RW, GPIO PA driving capability byte 2
#define R32_PA_SMT          (*((PUINT32V)0x0040009C)) // RW, GPIO PA output slew rate & input schmitt trigger: 0=fast,normal, 1=slow,schmitt
#define R8_PA_SMT_1         (*((PUINT8V)0x0040009D))  // RW, GPIO PA output slew rate & input schmitt trigger byte 1
#define R8_PA_SMT_2         (*((PUINT8V)0x0040009E))  // RW, GPIO PA output slew rate & input schmitt trigger byte 2

/* GPIO PB register */
#define R32_PB_DIR          (*((PUINT32V)0x004000A0)) // RW, GPIO PB I/O direction: 0=in, 1=out
#define R8_PB_DIR_0         (*((PUINT8V)0x004000A0))  // RW, GPIO PB I/O direction byte 0
#define R8_PB_DIR_1         (*((PUINT8V)0x004000A1))  // RW, GPIO PB I/O direction byte 1
#define R8_PB_DIR_2         (*((PUINT8V)0x004000A2))  // RW, GPIO PB I/O direction byte 2
#define R32_PB_PIN          (*((PUINT32V)0x004000A4)) // RO, GPIO PB input
#define R8_PB_PIN_0         (*((PUINT8V)0x004000A4))  // RO, GPIO PB input byte 0
#define R8_PB_PIN_1         (*((PUINT8V)0x004000A5))  // RO, GPIO PB input byte 1
#define R8_PB_PIN_2         (*((PUINT8V)0x004000A6))  // RO, GPIO PB input byte 2
#define R32_PB_OUT          (*((PUINT32V)0x004000A8)) // RW, GPIO PB output
#define R8_PB_OUT_0         (*((PUINT8V)0x004000A8))  // RW, GPIO PB output byte 0
#define R8_PB_OUT_1         (*((PUINT8V)0x004000A9))  // RW, GPIO PB output byte 1
#define R8_PB_OUT_2         (*((PUINT8V)0x004000AA))  // RW, GPIO PB output byte 2
#define R32_PB_CLR          (*((PUINT32V)0x004000AC)) // WZ, GPIO PB clear output: 0=keep, 1=clear
#define R8_PB_CLR_0         (*((PUINT8V)0x004000AC))  // WZ, GPIO PB clear output byte 0
#define R8_PB_CLR_1         (*((PUINT8V)0x004000AD))  // WZ, GPIO PB clear output byte 1
#define R8_PB_CLR_2         (*((PUINT8V)0x004000AE))  // WZ, GPIO PB clear output byte 2
#define R32_PB_PU           (*((PUINT32V)0x004000B0)) // RW, GPIO PB pullup resistance enable
#define R8_PB_PU_0          (*((PUINT8V)0x004000B0))  // RW, GPIO PB pullup resistance enable byte 0
#define R8_PB_PU_1          (*((PUINT8V)0x004000B1))  // RW, GPIO PB pullup resistance enable byte 1
#define R8_PB_PU_2          (*((PUINT8V)0x004000B2))  // RW, GPIO PB pullup resistance enable byte 2
#define R32_PB_PD           (*((PUINT32V)0x004000B4)) // RW, GPIO PB output open-drain & input pulldown resistance enable
#define R8_PB_PD_0          (*((PUINT8V)0x004000B4))  // RW, GPIO PB output open-drain & input pulldown resistance enable byte 0
#define R8_PB_PD_1          (*((PUINT8V)0x004000B5))  // RW, GPIO PB output open-drain & input pulldown resistance enable byte 1
#define R8_PB_PD_2          (*((PUINT8V)0x004000B6))  // RW, GPIO PB output open-drain & input pulldown resistance enable byte 2
#define R32_PB_DRV          (*((PUINT32V)0x004000B8)) // RW, GPIO PB driving capability: 0=4mA, 1=16mA
#define R8_PB_DRV_1         (*((PUINT8V)0x004000B9))  // RW, GPIO PB driving capability byte 1
#define R32_PB_SMT          (*((PUINT32V)0x004000BC)) // RW, GPIO PB output slew rate & input schmitt trigger: 0=fast,normal, 1=slow,schmitt
#define R8_PB_SMT_0         (*((PUINT8V)0x004000BC))  // RW, GPIO PB output slew rate & input schmitt trigger byte 0
#define R8_PB_SMT_1         (*((PUINT8V)0x004000BD))  // RW, GPIO PB output slew rate & input schmitt trigger byte 1
#define R8_PB_SMT_2         (*((PUINT8V)0x004000BE))  // RW, GPIO PB output slew rate & input schmitt trigger byte 2

/* GPIO PD register */
#define R32_PD_DIR          (*((PUINT32V)0x004000C0)) // RW, GPIO PD I/O direction: 0=in, 1=out
#define R8_PD_DIR_0         (*((PUINT8V)0x004000C0))  // RW, GPIO PD I/O direction byte 0
#define R8_PD_DIR_1         (*((PUINT8V)0x004000C1))  // RW, GPIO PD I/O direction byte 1
#define R8_PD_DIR_2         (*((PUINT8V)0x004000C2))  // RW, GPIO PD I/O direction byte 2
#define R8_PD_DIR_3         (*((PUINT8V)0x004000C3))  // RW, GPIO PD I/O direction byte 3
#define R32_PD_PIN          (*((PUINT32V)0x004000C4)) // RO, GPIO PD input
#define R8_PD_PIN_0         (*((PUINT8V)0x004000C4))  // RO, GPIO PD input byte 0
#define R8_PD_PIN_1         (*((PUINT8V)0x004000C5))  // RO, GPIO PD input byte 1
#define R8_PD_PIN_2         (*((PUINT8V)0x004000C6))  // RO, GPIO PD input byte 2
#define R8_PD_PIN_3         (*((PUINT8V)0x004000C7))  // RO, GPIO PD input byte 3
#define R32_PD_OUT          (*((PUINT32V)0x004000C8)) // RW, GPIO PD output
#define R8_PD_OUT_0         (*((PUINT8V)0x004000C8))  // RW, GPIO PD output byte 0
#define R8_PD_OUT_1         (*((PUINT8V)0x004000C9))  // RW, GPIO PD output byte 1
#define R8_PD_OUT_2         (*((PUINT8V)0x004000CA))  // RW, GPIO PD output byte 2
#define R8_PD_OUT_3         (*((PUINT8V)0x004000CB))  // RW, GPIO PD output byte 3
#define R32_PD_PU           (*((PUINT32V)0x004000D0)) // RW, GPIO PD pullup resistance enable
#define R8_PD_PU_0          (*((PUINT8V)0x004000D0))  // RW, GPIO PD pullup resistance enable 0
#define R8_PD_PU_1          (*((PUINT8V)0x004000D1))  // RW, GPIO PD pullup resistance enable 1
#define R8_PD_PU_2          (*((PUINT8V)0x004000D2))  // RW, GPIO PD pullup resistance enable 2
#define R8_PD_PU_3          (*((PUINT8V)0x004000D3))  // RW, GPIO PD pullup resistance enable 3
#define R32_PD_PD           (*((PUINT32V)0x004000D4)) // RW, GPIO PD pulldown resistance enable
#define R8_PD_PD_0          (*((PUINT8V)0x004000D4))  // RW, GPIO PD pulldown resistance enable 0
#define R8_PD_PD_1          (*((PUINT8V)0x004000D5))  // RW, GPIO PD pulldown resistance enable 1
#define R8_PD_PD_2          (*((PUINT8V)0x004000D6))  // RW, GPIO PD pulldown resistance enable 2
#define R8_PD_PD_3          (*((PUINT8V)0x004000D7))  // RW, GPIO PD pulldown resistance enable 3
#define R32_PD_DRV          (*((PUINT32V)0x004000D8)) // RW, GPIO PD driving capability: 0=4mA, 1=16mA
#define R8_PD_DRV           (*((PUINT8V)0x004000D8))  // RW, GPIO PD driving capability
#define R32_PD_SMT          (*((PUINT32V)0x004000DC)) // RW, GPIO PD output slew rate & input schmitt trigger: 0=fast,normal, 1=slow,schmitt
#define R8_PD_SMT           (*((PUINT8V)0x004000DC))  // RW, GPIO PD output slew rate & input schmitt trigger

/* GPIO register address offset and bit define */
#define BA_PA               ((PUINT8V)0x00400080)     // point GPIO PA base address
#define BA_PB               ((PUINT8V)0x004000A0)     // point GPIO PB base address
#define BA_PD               ((PUINT8V)0x004000C0)     // point GPIO PD base address
#define GPIO_DIR            0x00
#define GPIO_DIR_0          0x00
#define GPIO_DIR_1          0x01
#define GPIO_DIR_2          0x02
#define GPIO_DIR_3          0x03
#define GPIO_PIN            0x04
#define GPIO_PIN_0          0x04
#define GPIO_PIN_1          0x05
#define GPIO_PIN_2          0x06
#define GPIO_PIN_3          0x07
#define GPIO_OUT            0x08
#define GPIO_OUT_0          0x08
#define GPIO_OUT_1          0x09
#define GPIO_OUT_2          0x0A
#define GPIO_OUT_3          0x0B
#define GPIO_CLR            0x0C
#define GPIO_CLR_0          0x0C
#define GPIO_CLR_1          0x0D
#define GPIO_CLR_2          0x0E
#define GPIO_CLR_3          0x0F
#define GPIO_PU             0x10
#define GPIO_PU_0           0x10
#define GPIO_PU_1           0x11
#define GPIO_PU_2           0x12
#define GPIO_PU_3           0x13
#define GPIO_PD             0x14
#define GPIO_PD_0           0x14
#define GPIO_PD_1           0x15
#define GPIO_PD_2           0x16
#define GPIO_PD_3           0x17
#define GPIO_DRV            0x18
#define GPIO_DRV_0          0x18
#define GPIO_DRV_1          0x19
#define GPIO_DRV_2          0x1A
#define GPIO_SMT            0x1C
#define GPIO_SMT_0          0x1C
#define GPIO_SMT_1          0x1D
#define GPIO_SMT_2          0x1E
#define  RB_PD_15_8         0x01                      // RW, GPIO PD[15:8] pullup resistance enable together
#define  RB_PD_23_16        0x01                      // RW, GPIO PD[23:16] pullup resistance enable together
#define  RB_PD_31_24        0x01                      // RW, GPIO PD[31:24] pullup resistance enable together

/* GPIO alias name */
#define  TWP0               (1<<4)                    // PA4
#define  TWP1               (1<<5)                    // PA5
#define  TACK               (1<<6)                    // PA6
#define  TDO                (1<<7)                    // PA7
#define  TRST               (1<<9)                    // PA9
#define  TDI                (1<<13)                   // PA13
#define  TCK                (1<<14)                   // PA14
#define  TMS                (1<<15)                   // PA15
#define  UID                (1<<8)                    // PA8
#define  SLVI               (1<<9)                    // PA9
#define  SLVA               (1<<10)                   // PA10
#define  SLVCS              (1<<11)                   // PA11
#define  TNOW0              (1<<7)                    // PA7
#define  DTR0               (1<<7)                    // PA7
#define  RTS0               (1<<8)                    // PA8
#define  CTS0               (1<<12)                   // PA12
#define  DSR0               (1<<13)                   // PA13
#define  RI0                (1<<14)                   // PA14
#define  DCD0               (1<<15)                   // PA15
#define  CTS1               (1<<16)                   // PA16
#define  RTS1               (1<<17)                   // PA17
#define  TNOW1              (1<<17)                   // PA17
#define  ELED               (1<<13)                   // PA13
#define  ELINK              (1<<18)                   // PA18
#define  VBUS               (1<<19)                   // PA19
#define  PRD                (1<<20)                   // PA20
#define  PWR                (1<<21)                   // PA21
#define  CTS0X              (1<<0)                    // PB0
#define  DSR0X              (1<<1)                    // PB1
#define  RI0X               (1<<2)                    // PB2
#define  DCD0X              (1<<3)                    // PB3
#define  DTR0X              (1<<4)                    // PB4
#define  RTS0X              (1<<5)                    // PB5
#define  TRAN0              (1<<0)                    // PB0
#define  RECV0              (1<<1)                    // PB1
#define  TRAN1              (1<<2)                    // PB2
#define  RECV1              (1<<3)                    // PB3
#define  PWM0               (1<<0)                    // PB0
#define  CAT0               (1<<1)                    // PB1
/* note: PB1/CAT0 will input from PECL if RB_MISC_PECL_EN=1 and TMR0.RB_TMR_OUT_EN=1 */
#define  PWM1               (1<<2)                    // PB2
#define  CAT1               (1<<3)                    // PB3
#define  PWM2               (1<<4)                    // PB4
#define  CAT2               (1<<5)                    // PB5
#define  PWM3               (1<<6)                    // PB6
#define  CAT3               (1<<6)                    // PB6
#define  RXD0               (1<<8)                    // PB8
#define  TXD0               (1<<9)                    // PB9
#define  RXD1               (1<<10)                   // PB10
/* note: PB10/RXD1 will input from PECL if RB_MISC_PECL_EN=1 and TMR0.RB_TMR_OUT_EN=0 */
#define  RXTX1              (1<<10)                   // PB10
#define  TXD1               (1<<11)                   // PB11
#define  SCS                (1<<12)                   // PB12
#define  SCK0               (1<<13)                   // PB13
#define  MOSI               (1<<14)                   // PB14
#define  MISO               (1<<15)                   // PB15
#define  SDX0               (1<<15)                   // PB15
#define  ADCS               (1<<16)                   // PB16
#define  SCK1               (1<<17)                   // PB17
#define  SDO                (1<<18)                   // PB18
#define  SDI                (1<<19)                   // PB19
#define  SDX1               (1<<19)                   // PB19

/* ADC register */
#define R32_ADC_CONTROL     (*((PUINT32V)0x0040A000)) // RW, ADC control
#define R8_ADC_CTRL_MOD     (*((PUINT8V)0x0040A000))  // RW, ADC mode control
#define R8_ADC_CTRL_DMA     (*((PUINT8V)0x0040A001))  // RW, ADC DMA control and etc.
#define R8_ADC_INTER_EN     (*((PUINT8V)0x0040A002))  // RW, ADC interrupt enable
#define R8_ADC_CLOCK_DIV    (*((PUINT8V)0x0040A003))  // RW, ADC clock divisor
#define R32_ADC_STATUS      (*((PUINT32V)0x0040A004)) // RW, ADC status
#define R16_ADC_DATA        (*((PUINT16V)0x0040A004)) // RO, ADC result data
#define R8_ADC_INT_FLAG     (*((PUINT8V)0x0040A006))  // RW1, ADC interrupt flag
#define R8_ADC_FIFO_COUNT   (*((PUINT8V)0x0040A007))  // RO, ADC FIFO count status
#define R16_ADC_CMP_VALUE   (*((PUINT16V)0x0040A00C)) // RW, ADC comparison reference value
#define R16_ADC_FIFO        (*((PUINT16V)0x0040A010)) // RO, ADC FIFO register
#define R16_ADC_DMA_NOW     (*((PUINT16V)0x0040A014)) // RW, ADC DMA current address
#define R16_ADC_DMA_BEG     (*((PUINT16V)0x0040A018)) // RW, ADC DMA begin address
#define R16_ADC_DMA_END     (*((PUINT16V)0x0040A01C)) // RW, ADC DMA end address

/* ADC register address offset and bit define */
#define ADC_FIFO_SIZE       8                         // ADC FIFO size (depth)
#define BA_ADC              ((PUINT8V)0x0040A000)     // point ADC base address
#define ADC_CTRL_MOD        0
#define  RB_ADC_CYCLE_CLK   0x0F                      // RW, ADC cycle bit mask (clock number): 0=manual sample, other=set cycle for auto sample
#define  RB_ADC_CHAN_MOD    0x30                      // RW, ADC channel control mode: 00=0#, 01=1#, 10=2#, 11=auto flip 0#/1#
#define  RB_ADC_SAMPLE_WID  0x40                      // RW, ADC sample pulse width: 0=1 clock, 1=2 clock
#define  RB_ADC_POWER_ON    0x80                      // RW, ADC module enable
#define ADC_CTRL_DMA        1
#define  RB_ADC_DMA_ENABLE  0x01                      // RW, ADC DMA enable
#define  RB_ADC_DMA_BURST   0x02                      // RW, ADC DMA burst enable
#define  RB_ADC_DMA_LOOP    0x04                      // RW, ADC DMA address loop enable
#define  RB_ADC_CHAN_OE     0x40                      // WO, ADC channel control output enable
#define  RB_ADC_MAN_SAMPLE  0x80                      // RW, ADC manual sample control, high action
#define ADC_INTER_EN        2
#define  RB_ADC_IE_ADC_CMP  0x01                      // RW, enable interrupt for current ADC comparison action
#define  RB_ADC_IE_ADC_END  0x02                      // RW, enable interrupt for current ADC end
#define  RB_ADC_IE_FIFO_HF  0x04                      // RW, enable interrupt for ADC FIFO half
#define  RB_ADC_IE_DMA_END  0x08                      // RW, enable interrupt for ADC DMA completion
#define  RB_ADC_IE_FIFO_OV  0x10                      // RW, enable interrupt for ADC FIFO overflow
#define  RB_ADC_IE_DMA_ERR  0x20                      // RW, enable interrupt for ADC DMA respond error
#define  RB_ADC_CMP_MOD_EQ  0x40                      // RW, ADC equal comparison enable: 0=exclude equal, 1=include equal
#define  RB_ADC_CMP_MOD_GT  0x80                      // RW, ADC comparison mode: 0=less action, 1=great action
#define ADC_CLOCK_DIV       3
#define ADC_DATA            4
#define ADC_INT_FLAG        6
#define  RB_ADC_IF_ADC_CMP  0x01                      // RW1, interrupt flag for current ADC comparison action
#define  RB_ADC_IF_ADC_END  0x02                      // RW1, interrupt flag for current ADC end
#define  RB_ADC_IF_FIFO_HF  0x04                      // RW1, interrupt flag for ADC FIFO half
#define  RB_ADC_IF_DMA_END  0x08                      // RW1, interrupt flag for ADC DMA completion
#define  RB_ADC_IF_FIFO_OV  0x10                      // RW1, interrupt flag for ADC FIFO overflow
#define  RB_ADC_IF_DMA_ERR  0x20                      // RW1, interrupt flag for ADC DMA respond error
#define  RB_ADC_EOC_FLAG    0x40                      // RO, current ADC converter end indicator
#define  RB_ADC_CHAN_INDEX  0x80                      // RO, current ADC channel number for auto flip: 0=0#/2#, 1=1#
#define ADC_FIFO_COUNT      7
#define ADC_CMP_VALUE       0x0C
#define ADC_FIFO            0x10

/* UART0 register */
#define R8_UART0_RBR        (*((PUINT8V)0x0040D000))  // RO, UART0 receiver buffer, receiving byte
#define R8_UART0_THR        (*((PUINT8V)0x0040D000))  // WO, UART0 transmitter holding, transmittal byte
#define R8_UART0_IER        (*((PUINT8V)0x0040D001))  // RW, UART0 interrupt enable
#define R8_UART0_IIR        (*((PUINT8V)0x0040D002))  // RO, UART0 interrupt identification
#define R8_UART0_FCR        (*((PUINT8V)0x0040D002))  // WO, UART0 FIFO control
#define R8_UART0_LCR        (*((PUINT8V)0x0040D003))  // RW, UART0 line control
#define R8_UART0_MCR        (*((PUINT8V)0x0040D004))  // RW, UART0 modem control
#define R8_UART0_LSR        (*((PUINT8V)0x0040D005))  // RO, UART0 line status
#define R8_UART0_MSR        (*((PUINT8V)0x0040D006))  // RO, UART0 modem status
#define R8_UART0_DIV        (*((PUINT8V)0x0040D007))  // RW, UART0 pre-divisor latch byte, only low 7 bit, from 1 to 0/128
#define R8_UART0_DLL        (*((PUINT8V)0x0040D000))  // RW, UART0 divisor latch LSB byte
#define R8_UART0_DLM        (*((PUINT8V)0x0040D001))  // RW, UART0 divisor latch MSB byte

/* UART1 register */
#define R8_UART1_RBR        (*((PUINT8V)0x0040D800))  // RO, UART1 receiver buffer, receiving byte
#define R8_UART1_THR        (*((PUINT8V)0x0040D800))  // WO, UART1 transmitter holding, transmittal byte
#define R8_UART1_IER        (*((PUINT8V)0x0040D801))  // RW, UART1 interrupt enable
#define R8_UART1_IIR        (*((PUINT8V)0x0040D802))  // RO, UART1 interrupt identification
#define R8_UART1_FCR        (*((PUINT8V)0x0040D802))  // WO, UART1 FIFO control
#define R8_UART1_LCR        (*((PUINT8V)0x0040D803))  // RW, UART1 line control
#define R8_UART1_MCR        (*((PUINT8V)0x0040D804))  // RW, UART1 modem control
#define R8_UART1_LSR        (*((PUINT8V)0x0040D805))  // RO, UART1 line status
#define R8_UART1_MSR        (*((PUINT8V)0x0040D806))  // RO, UART1 modem status
#define R8_UART1_ADR        (*((PUINT8V)0x0040D807))  // RW, UART1 slave address: 0xFF=disable, other=enable
#define R8_UART1_DIV        (*((PUINT8V)0x0040D807))  // RW, UART1 pre-divisor latch byte, only low 7 bit, from 1 to 0/128
#define R8_UART1_DLL        (*((PUINT8V)0x0040D800))  // RW, UART1 divisor latch LSB byte
#define R8_UART1_DLM        (*((PUINT8V)0x0040D801))  // RW, UART1 divisor latch MSB byte

/* UART register address offset and bit define */
#define UART0_FIFO_SIZE     16                        // UART0 FIFO size (depth)
#define UART0_RECV_RDY_SZ   14                        // the max FIFO trigger level for UART0 receiver data available
#define UART1_FIFO_SIZE     32                        // UART1 FIFO size (depth)
#define UART1_RECV_RDY_SZ   28                        // the max FIFO trigger level for UART1 receiver data available
#define BA_UART0            ((PUINT8V)0x0040D000)     // point UART0 base address
#define BA_UART1            ((PUINT8V)0x0040D800)     // point UART1 base address
#define UART_RBR            0
#define UART_THR            0
#define UART_IER            1
#define  RB_IER_RECV_RDY    0x01                      // RW, UART interrupt enable for receiver data ready
#define  RB_IER_THR_EMPTY   0x02                      // RW, UART interrupt enable for THR empty
#define  RB_IER_LINE_STAT   0x04                      // RW, UART interrupt enable for receiver line status
#define  RB_IER_MODEM_CHG   0x08                      // RW, UART interrupt enable for modem status change
#define  RB_IER_MODEM_IO    0x10                      // RW, UART0 modem pin selection: 0=from/to GPIO PA, 1=from/to GPIO PB
#define  RB_IER_IRDA_MOD    0x10                      // RW, UART1 IrDA mode enable
#define  RB_IER_MOUT_EN     0x20                      // RW, UART modem output pin enable
#define  RB_IER_TXD_EN      0x40                      // RW, UART TXD pin enable
#define  RB_IER_RESET       0x80                      // WZ, UART software reset control, high action, auto clear
#define UART_IIR            2
#define  RB_IIR_NO_INT      0x01                      // RO, UART no interrupt flag: 0=interrupt action, 1=no interrupt
#define  RB_IIR_INT_MASK    0x0F                      // RO, UART interrupt flag bit mask
#define  RB_IIR_FIFO_ID     0xC0                      // RO, UART FIFO enabled flag
#define UART_FCR            2
#define  RB_FCR_FIFO_EN     0x01                      // WO, UART FIFO enable
#define  RB_FCR_RX_FIFO_CLR 0x02                      // WZ, clear UART receiver FIFO, high action, auto clear
#define  RB_FCR_TX_FIFO_CLR 0x04                      // WZ, clear UART transmitter FIFO, high action, auto clear
#define  RB_FCR_FIFO_TRIG   0xC0                      // WO, UART0/1 receiver FIFO trigger level: 00-1byte, 01-4/8bytes, 10-8/16bytes, 11-14/28bytes
#define UART_LCR            3
#define  RB_LCR_WORD_SZ     0x03                      // RW, UART word bit length: 00-5bit, 01-6bit, 10-7bit, 11-8bit
#define  RB_LCR_STOP_BIT    0x04                      // RW, UART stop bit length: 0-1bit, 1-2bit
#define  RB_LCR_PAR_EN      0x08                      // RW, UART parity enable
#define  RB_LCR_PAR_MOD     0x30                      // RW, UART pariry mode: 00-odd, 01-even, 10-mark, 11-space
#define  RB_LCR_BREAK_EN    0x40                      // RW, UART break control enable
#define  RB_LCR_DLAB        0x80                      // RW, UART divisor latch access bit
#define UART_MCR            4
#define  RB_MCR_DTR         0x01                      // RW, UART control DTR
#define  RB_MCR_RTS         0x02                      // RW, UART control RTS
#define  RB_MCR_OUT1        0x04                      // RW, UART control OUT1
#define  RB_MCR_OUT2        0x08                      // RW, UART control OUT2
#define  RB_MCR_LOOP        0x10                      // RW, UART0 enable local loop back
#define  RB_MCR_RXTX        0x10                      // RW, UART1 enable RXTX for half-duplex (TXD auto three-state output on RXD pin)
#define  RB_MCR_AU_FLOW_EN  0x20                      // RW, UART enable autoflow control
#define  RB_MCR_TNOW        0x40                      // RW, UART enable TNOW output on DTR or RTS pin
#define  RB_MCR_HALF        0x80                      // RW, UART enable half-duplex
#define UART_LSR            5
#define  RB_LSR_DATA_RDY    0x01                      // RO, UART receiver fifo data ready status
#define  RB_LSR_OVER_ERR    0x02                      // RZ, UART receiver overrun error
#define  RB_LSR_PAR_ERR     0x04                      // RZ, UART receiver parity error
#define  RB_LSR_FRAME_ERR   0x08                      // RZ, UART receiver frame error
#define  RB_LSR_BREAK_ERR   0x10                      // RZ, UART receiver break error
#define  RB_LSR_TX_FIFO_EMP 0x20                      // RO, UART transmitter fifo empty status
#define  RB_LSR_TX_ALL_EMP  0x40                      // RO, UART transmitter all empty status
#define  RB_LSR_ERR_RX_FIFO 0x80                      // RO, error in UART receiver fifo
#define UART_MSR            6
#define  RB_MSR_CTS_CHG     0x01                      // RZ, UART CTS changed status, high action
#define  RB_MSR_DSR_CHG     0x02                      // RZ, UART DSR changed status, high action
#define  RB_MSR_RI_CHG      0x04                      // RZ, UART RI changed status, high action
#define  RB_MSR_DCD_CHG     0x08                      // RZ, UART DCD changed status, high action
#define  RB_MSR_CTS         0x10                      // RO, UART CTS action status
#define  RB_MSR_DSR         0x20                      // RO, UART DSR action status
#define  RB_MSR_RI          0x40                      // RO, UART RI action status
#define  RB_MSR_DCD         0x80                      // RO, UART DCD action status
#define UART_ADR            7
#define UART_DIV            7
#define UART_DLL            0
#define UART_DLM            1

/* UART interrupt identification values for IIR bits 3:0 */
#define UART_II_SLV_ADDR    0x0E                      // RO, UART1 slave address match
#define UART_II_LINE_STAT   0x06                      // RO, UART interrupt by receiver line status
#define UART_II_RECV_RDY    0x04                      // RO, UART interrupt by receiver data available
#define UART_II_RECV_TOUT   0x0C                      // RO, UART interrupt by receiver fifo timeout
#define UART_II_THR_EMPTY   0x02                      // RO, UART interrupt by THR empty
#define UART_II_MODEM_CHG   0x00                      // RO, UART interrupt by modem status change
#define UART_II_NO_INTER    0x01                      // RO, no UART interrupt is pending

/* SPI0 register */
#define R32_SPI0_CONTROL    (*((PUINT32V)0x0040C000)) // RW, SPI0 control
#define R8_SPI0_CTRL_MOD    (*((PUINT8V)0x0040C000))  // RW, SPI0 mode control
#define R8_SPI0_CTRL_DMA    (*((PUINT8V)0x0040C001))  // RW, SPI0 DMA control
#define R8_SPI0_INTER_EN    (*((PUINT8V)0x0040C002))  // RW, SPI0 interrupt enable
#define R8_SPI0_CLOCK_DIV   (*((PUINT8V)0x0040C003))  // RW, SPI0 master clock divisor
#define R8_SPI0_SLAVE_PRE   (*((PUINT8V)0x0040C003))  // RW, SPI0 slave preset value
#define R32_SPI0_STATUS     (*((PUINT32V)0x0040C004)) // RW, SPI0 status
#define R8_SPI0_BUFFER      (*((PUINT8V)0x0040C004))  // RW, SPI0 data buffer
#define R8_SPI0_RUN_FLAG    (*((PUINT8V)0x0040C005))  // RO, SPI0 work flag
#define R8_SPI0_INT_FLAG    (*((PUINT8V)0x0040C006))  // RW1, SPI0 interrupt flag
#define R8_SPI0_FIFO_COUNT  (*((PUINT8V)0x0040C007))  // RO, SPI0 FIFO count status
#define R16_SPI0_TOTAL_CNT  (*((PUINT16V)0x0040C00C)) // RW, SPI0 total byte count, only low 12 bit
#define R32_SPI0_FIFO       (*((PUINT32V)0x0040C010)) // RW, SPI0 FIFO register
#define R8_SPI0_FIFO        (*((PUINT8V)0x0040C010))  // RO/WO, SPI0 FIFO register
#define R8_SPI0_FIFO_COUNT1 (*((PUINT8V)0x0040C013))  // RO, SPI0 FIFO count status
#define R16_SPI0_DMA_NOW    (*((PUINT16V)0x0040C014)) // RW, SPI0 DMA current address
#define R16_SPI0_DMA_BEG    (*((PUINT16V)0x0040C018)) // RW, SPI0 DMA begin address
#define R16_SPI0_DMA_END    (*((PUINT16V)0x0040C01C)) // RW, SPI0 DMA end address

/* SPI1 register */
#define R32_SPI1_CONTROL    (*((PUINT32V)0x0040C800)) // RW, SPI1 control
#define R8_SPI1_CTRL_MOD    (*((PUINT8V)0x0040C800))  // RW, SPI1 mode control
#define R8_SPI1_CTRL_DMA    (*((PUINT8V)0x0040C801))  // RW, SPI1 DMA control
#define R8_SPI1_INTER_EN    (*((PUINT8V)0x0040C802))  // RW, SPI1 interrupt enable
#define R8_SPI1_CLOCK_DIV   (*((PUINT8V)0x0040C803))  // RW, SPI1 master clock divisor
#define R32_SPI1_STATUS     (*((PUINT32V)0x0040C804)) // RW, SPI1 status
#define R8_SPI1_BUFFER      (*((PUINT8V)0x0040C804))  // RW, SPI1 data buffer
#define R8_SPI1_RUN_FLAG    (*((PUINT8V)0x0040C805))  // RO, SPI1 work flag
#define R8_SPI1_INT_FLAG    (*((PUINT8V)0x0040C806))  // RW1, SPI1 interrupt flag
#define R8_SPI1_FIFO_COUNT  (*((PUINT8V)0x0040C807))  // RO, SPI1 FIFO count status
#define R16_SPI1_TOTAL_CNT  (*((PUINT16V)0x0040C80C)) // RW, SPI1 total byte count, only low 12 bit
#define R32_SPI1_FIFO       (*((PUINT32V)0x0040C810)) // RW, SPI1 FIFO register
#define R8_SPI1_FIFO        (*((PUINT8V)0x0040C810))  // RO/WO, SPI1 FIFO register
#define R8_SPI1_FIFO_COUNT1 (*((PUINT8V)0x0040C813))  // RO, SPI1 FIFO count status

/* SPI register address offset and bit define */
#define SPI0_FIFO_SIZE      32                        // SPI0 FIFO size (depth)
#define SPI1_FIFO_SIZE      16                        // SPI1 FIFO size (depth)
#define BA_SPI0             ((PUINT8V)0x0040C000)     // point SPI0 base address
#define BA_SPI1             ((PUINT8V)0x0040C800)     // point SPI1 base address
#define SPI_CTRL_MOD        0
#define  RB_SPI_MODE_SLAVE  0x01                      // RW, SPI slave mode: 0=master/host, 1=slave/device
#define  RB_SPI_ALL_CLEAR   0x02                      // RW, force clear SPI FIFO and count
#define  RB_SPI_2WIRE_MOD   0x04                      // RW, SPI enable 2 wire mode: 0=3wire(SCK,MOSI,MISO), 1=2wire(SCK,MISO=SDX)
#define  RB_SPI_MST_SCK_MOD 0x08                      // RW, SPI master clock mode: 0=mode 0, 1=mode 3
#define  RB_SPI_SLV_CMD_MOD 0x08                      // RW, SPI slave command mode: 0=byte stream, 1=first byte command
#define  RB_SPI_FIFO_DIR    0x10                      // RW, SPI FIFO direction: 0=out(write @master mode), 1=in(read @master mode)
#define  RB_SPI_SCK_OE      0x20                      // RW, SPI SCK output enable
#define  RB_SPI_MOSI_OE     0x40                      // RW, SPI MOSI output enable
#define  RB_SPI1_SDO_OE     0x40                      // RW, SPI1 SDO output enable
#define  RB_SPI_MISO_OE     0x80                      // RW, SPI MISO output enable
#define  RB_SPI1_SDI_OE     0x80                      // RW, SPI1 SDI output enable, SPI1 enable 2 wire mode: 0=3wire(SCK1,SDO,SDI), 1=2wire(SCK1,SDX1)
#define SPI_CTRL_DMA        1
#define  RB_SPI_DMA_ENABLE  0x01                      // RW, SPI DMA enable
#define  RB_SPI_DMA_BURST   0x02                      // RW, SPI DMA burst enable
#define  RB_SPI_DMA_LOOP    0x04                      // RW, SPI DMA address loop enable
#define SPI_INTER_EN        2
#define  RB_SPI_IE_CNT_END  0x01                      // RW, enable interrupt for SPI total byte count end
#define  RB_SPI_IE_BYTE_END 0x02                      // RW, enable interrupt for SPI byte exchanged
#define  RB_SPI_IE_FIFO_HF  0x04                      // RW, enable interrupt for SPI FIFO half
#define  RB_SPI_IE_DMA_END  0x08                      // RW, enable interrupt for SPI DMA completion
#define  RB_SPI_IE_FIFO_OV  0x10                      // RW, enable interrupt for SPI FIFO overflow
#define  RB_SPI_IE_DMA_ERR  0x20                      // RW, enable interrupt for SPI DMA respond error
#define  RB_SPI_IE_FST_BYTE 0x80                      // RW, enable interrupt for SPI slave mode first byte received
#define SPI_CLOCK_DIV       3
#define SPI_SLAVE_PRESET    3
#define SPI_BUFFER          4
#define SPI_RUN_FLAG        5
#define  RB_SPI_SLV_CMD_ACT 0x10                      // RO, SPI slave command flag
#define  RB_SPI_FIFO_READY  0x20                      // RO, SPI FIFO ready status
#define  RB_SPI_SLV_CS_LOAD 0x40                      // RO, SPI slave chip-select loading status
#define  RB_SPI_SLV_SELECT  0x80                      // RO, SPI slave selection status
#define SPI_INT_FLAG        6
#define  RB_SPI_IF_CNT_END  0x01                      // RW1, interrupt flag for SPI total byte count end
#define  RB_SPI_IF_BYTE_END 0x02                      // RW1, interrupt flag for SPI byte exchanged
#define  RB_SPI_IF_FIFO_HF  0x04                      // RW1, interrupt flag for SPI FIFO half
#define  RB_SPI_IF_DMA_END  0x08                      // RW1, interrupt flag for SPI DMA completion
#define  RB_SPI_IF_FIFO_OV  0x10                      // RW1, interrupt flag for SPI FIFO overflow
#define  RB_SPI_IF_DMA_ERR  0x20                      // RW1, interrupt flag for SPI DMA respond error
#define  RB_SPI_FREE        0x40                      // RO, current SPI free status
#define  RB_SPI_IF_FST_BYTE 0x80                      // RW1, interrupt flag for SPI slave mode first byte received
#define SPI_FIFO_COUNT      7
#define SPI_TOTAL_CNT       0x0C
#define SPI_FIFO            0x10

/* Timer0 register */
#define R32_TMR0_CONTROL    (*((PUINT32V)0x00408000)) // RW, TMR0 control
#define R8_TMR0_CTRL_MOD    (*((PUINT8V)0x00408000))  // RW, TMR0 mode control
#define R8_TMR0_CTRL_DMA    (*((PUINT8V)0x00408001))  // RW, TMR0 DMA control
#define R8_TMR0_INTER_EN    (*((PUINT8V)0x00408002))  // RW, TMR0 interrupt enable
#define R8_TMR0_NRZI_CK_DIV (*((PUINT8V)0x00408003))  // RW, TMR0 NRZI clock divisor, only low 4 bit, from 0 to 15
#define R32_TMR0_STATUS     (*((PUINT32V)0x00408004)) // RW, TMR0 status
#define R8_TMR0_NRZI_STATUS (*((PUINT8V)0x00408004))  // RO, TMR0 NRZI status
#define R8_TMR0_INT_FLAG    (*((PUINT8V)0x00408006))  // RW1, TMR0 interrupt flag
#define R8_TMR0_FIFO_COUNT  (*((PUINT8V)0x00408007))  // RO, TMR0 FIFO count status
#define R32_TMR0_COUNT      (*((PUINT32V)0x00408008)) // RO, TMR0 current count
#define R16_TMR0_COUNT      (*((PUINT16V)0x00408008)) // RO, TMR0 current count
#define R8_TMR0_COUNT       (*((PUINT8V)0x00408008))  // RO, TMR0 current count
#define R32_TMR0_CNT_END    (*((PUINT32V)0x0040800C)) // RW, TMR0 end count value, only low 28 bit
#define R32_TMR0_FIFO       (*((PUINT32V)0x00408010)) // RO/WO, TMR0 FIFO register, only low 28 bit
#define R16_TMR0_FIFO       (*((PUINT16V)0x00408010)) // RO/WO, TMR0 FIFO register
#define R8_TMR0_FIFO        (*((PUINT8V)0x00408010))  // RO/WO, TMR0 FIFO register
#define R16_TMR0_DMA_NOW    (*((PUINT16V)0x00408014)) // RW, TMR0 DMA current address
#define R16_TMR0_DMA_BEG    (*((PUINT16V)0x00408018)) // RW, TMR0 DMA begin address
#define R16_TMR0_DMA_END    (*((PUINT16V)0x0040801C)) // RW, TMR0 DMA end address

/* Timer1 register */
#define R32_TMR1_CONTROL    (*((PUINT32V)0x00408400)) // RW, TMR1 control
#define R8_TMR1_CTRL_MOD    (*((PUINT8V)0x00408400))  // RW, TMR1 mode control
#define R8_TMR1_CTRL_DMA    (*((PUINT8V)0x00408401))  // RW, TMR1 DMA control
#define R8_TMR1_INTER_EN    (*((PUINT8V)0x00408402))  // RW, TMR1 interrupt enable
#define R8_TMR1_NRZI_CK_DIV (*((PUINT8V)0x00408403))  // RW, TMR1 NRZI clock divisor, only low 4 bit, from 0 to 15
#define R32_TMR1_STATUS     (*((PUINT32V)0x00408404)) // RW, TMR1 status
#define R8_TMR1_NRZI_STATUS (*((PUINT8V)0x00408404))  // RO, TMR1 NRZI status
#define R8_TMR1_INT_FLAG    (*((PUINT8V)0x00408406))  // RW1, TMR1 interrupt flag
#define R8_TMR1_FIFO_COUNT  (*((PUINT8V)0x00408407))  // RO, TMR1 FIFO count status
#define R32_TMR1_COUNT      (*((PUINT32V)0x00408408)) // RO, TMR1 current count
#define R16_TMR1_COUNT      (*((PUINT16V)0x00408408)) // RO, TMR1 current count
#define R8_TMR1_COUNT       (*((PUINT8V)0x00408408))  // RO, TMR1 current count
#define R32_TMR1_CNT_END    (*((PUINT32V)0x0040840C)) // RW, TMR1 end count value, only low 28 bit
#define R32_TMR1_FIFO       (*((PUINT32V)0x00408410)) // RO/WO, TMR1 FIFO register, only low 28 bit
#define R16_TMR1_FIFO       (*((PUINT16V)0x00408410)) // RO/WO, TMR1 FIFO register
#define R8_TMR1_FIFO        (*((PUINT8V)0x00408410))  // RO/WO, TMR1 FIFO register
#define R16_TMR1_DMA_NOW    (*((PUINT16V)0x00408414)) // RW, TMR1 DMA current address
#define R16_TMR1_DMA_BEG    (*((PUINT16V)0x00408418)) // RW, TMR1 DMA begin address
#define R16_TMR1_DMA_END    (*((PUINT16V)0x0040841C)) // RW, TMR1 DMA end address

/* Timer2 register */
#define R32_TMR2_CONTROL    (*((PUINT32V)0x00408800)) // RW, TMR2 control
#define R8_TMR2_CTRL_MOD    (*((PUINT8V)0x00408800))  // RW, TMR2 mode control
#define R8_TMR2_CTRL_DMA    (*((PUINT8V)0x00408801))  // RW, TMR2 DMA control
#define R8_TMR2_INTER_EN    (*((PUINT8V)0x00408802))  // RW, TMR2 interrupt enable
#define R32_TMR2_STATUS     (*((PUINT32V)0x00408804)) // RW, TMR2 status
#define R8_TMR2_INT_FLAG    (*((PUINT8V)0x00408806))  // RW1, TMR2 interrupt flag
#define R8_TMR2_FIFO_COUNT  (*((PUINT8V)0x00408807))  // RO, TMR2 FIFO count status
#define R32_TMR2_COUNT      (*((PUINT32V)0x00408808)) // RO, TMR2 current count
#define R16_TMR2_COUNT      (*((PUINT16V)0x00408808)) // RO, TMR2 current count
#define R8_TMR2_COUNT       (*((PUINT8V)0x00408808))  // RO, TMR2 current count
#define R32_TMR2_CNT_END    (*((PUINT32V)0x0040880C)) // RW, TMR2 end count value, only low 28 bit
#define R32_TMR2_FIFO       (*((PUINT32V)0x00408810)) // RO/WO, TMR2 FIFO register, only low 28 bit
#define R16_TMR2_FIFO       (*((PUINT16V)0x00408810)) // RO/WO, TMR2 FIFO register
#define R8_TMR2_FIFO        (*((PUINT8V)0x00408810))  // RO/WO, TMR2 FIFO register
#define R16_TMR2_DMA_NOW    (*((PUINT16V)0x00408814)) // RW, TMR2 DMA current address
#define R16_TMR2_DMA_BEG    (*((PUINT16V)0x00408818)) // RW, TMR2 DMA begin address
#define R16_TMR2_DMA_END    (*((PUINT16V)0x0040881C)) // RW, TMR2 DMA end address

/* Timer3 register */
#define R32_TMR3_CONTROL    (*((PUINT32V)0x00408C00)) // RW, TMR3 control
#define R8_TMR3_CTRL_MOD    (*((PUINT8V)0x00408C00))  // RW, TMR3 mode control
#define R8_TMR3_INTER_EN    (*((PUINT8V)0x00408C02))  // RW, TMR3 interrupt enable
#define R32_TMR3_STATUS     (*((PUINT32V)0x00408C04)) // RW, TMR3 status
#define R8_TMR3_INT_FLAG    (*((PUINT8V)0x00408C06))  // RW1, TMR3 interrupt flag
#define R8_TMR3_FIFO_COUNT  (*((PUINT8V)0x00408C07))  // RO, TMR3 FIFO count status
#define R32_TMR3_COUNT      (*((PUINT32V)0x00408C08)) // RO, TMR3 current count
#define R16_TMR3_COUNT      (*((PUINT16V)0x00408C08)) // RO, TMR3 current count
#define R8_TMR3_COUNT       (*((PUINT8V)0x00408C08))  // RO, TMR3 current count
#define R32_TMR3_CNT_END    (*((PUINT32V)0x00408C0C)) // RW, TMR3 end count value, only low 28 bit
#define R32_TMR3_FIFO       (*((PUINT32V)0x00408C10)) // RO/WO, TMR3 FIFO register, only low 28 bit
#define R16_TMR3_FIFO       (*((PUINT16V)0x00408C10)) // RO/WO, TMR3 FIFO register
#define R8_TMR3_FIFO        (*((PUINT8V)0x00408C10))  // RO/WO, TMR3 FIFO register

/* Timer register address offset and bit define */
#define TMR_FIFO_SIZE       8                         // timer FIFO size (depth)
#define BA_TMR0             ((PUINT8V)0x00408000)     // point TMR0 base address
#define BA_TMR1             ((PUINT8V)0x00408400)     // point TMR1 base address
#define BA_TMR2             ((PUINT8V)0x00408800)     // point TMR2 base address
#define BA_TMR3             ((PUINT8V)0x00408C00)     // point TMR3 base address
#define TMR_CTRL_MOD        0
#define  RB_TMR_MODE_IN     0x01                      // RW, timer in mode: 0=timer/PWM/count/NRZI encode, 1=catcher/NRZI decode
#define  RB_TMR_ALL_CLEAR   0x02                      // RW, force clear timer FIFO and count
#define  RB_TMR_COUNT_EN    0x04                      // RW, timer count enable
#define  RB_TMR_OUT_EN      0x08                      // RW, timer output enable
#define  RB_TMR_OUT_POLAR   0x10                      // RW, timer PWM/NRZI encode output polarity: 0=high action, 1=low action
#define  RB_TMR_CAT_WIDTH   0x10                      // RW, timer catcher input pulse min width selection: 0=16*clock, 1=8*clock
#define  RB_TMR_MODE_NRZI   0x20                      // RW, TMR0/TMR1 NRZI mode: 0=timer/PWM/catcher, 1=NRZI encode/decode
#define  RB_TMR3_MODE_COUNT 0x20                      // RW, TMR3 count mode: 0=timer/PWM/catcher/NRZI, 1=count
#define  RB_TMR_PWM_REPEAT  0xC0                      // RW, timer PWM repeat mode: 00=1, 01=4, 10=8, 11-16
#define  RB_TMR_CATCH_EDGE  0xC0                      // RW, timer catcher edge mode: 00=disable, 01=edge change, 10=fall to fall, 11-rise to rise
#define  RB_TMR_NRZI_BALAN  0xC0                      // RW, timer NRZI balance mode: 00/11=no balance, 01=32bit, 10=48bit
#define TMR_CTRL_DMA        1
#define  RB_TMR_DMA_ENABLE  0x01                      // RW, timer DMA enable
#define  RB_TMR_DMA_BURST   0x02                      // RW, timer DMA burst enable
#define  RB_TMR_DMA_LOOP    0x04                      // RW, timer DMA address loop enable
#define TMR_INTER_EN        2
#define  RB_TMR_IE_CYC_END  0x01                      // RW, enable interrupt for timer catcher count timeout or PWM cycle end
#define  RB_TMR_IE_DATA_ACT 0x02                      // RW, enable interrupt for timer catcher input action or PWM trigger or NRZI recv packet end/tran packet end
#define  RB_TMR_IE_FIFO_HF  0x04                      // RW, enable interrupt for timer FIFO half
#define  RB_TMR_IE_DMA_END  0x08                      // RW, enable interrupt for timer DMA completion
#define  RB_TMR_IE_FIFO_OV  0x10                      // RW, enable interrupt for timer FIFO overflow
#define  RB_TMR_IE_DMA_ERR  0x20                      // RW, enable interrupt for timer DMA respond error
#define  RB_TMR_IE_NRZI_AUX 0x40                      // RW, enable interrupt for timer NRZI aux notice: NRZI recv error/tran start new data block
#define  RB_TMR3_FORCE_EN   0x80                      // RW, TMR3 force together timer0/1/2 count enable, independent of RB_TMR_COUNT_EN
#define TMR_NRZI_CK_DIV     3
#define TMR_NRZI_STATUS     4
#define  RB_TMR_RECV_FREE   0x01                      // RO, timer NRZI receiver free status, 0->1 then RB_TMR_IF_DATA_ACT for recv
#define  RB_TMR_RECV_ERR    0x02                      // RO, timer NRZI receiving error status, 0->1 then RB_TMR_IF_NRZI_AUX for recv
#define  RB_TMR_TRAN_END    0x10                      // RO, timer NRZI transmittal end status, 0->1 then RB_TMR_IF_DATA_ACT for tran
#define  RB_TMR_TRAN_DOE    0x20                      // RO, timer NRZI transmitter encode output enable status, 0->1 then RB_TMR_IF_NRZI_AUX for tran
#define TMR_INT_FLAG        6
#define  RB_TMR_IF_CYC_END  0x01                      // RW1, interrupt flag for timer catcher count timeout or PWM cycle end
#define  RB_TMR_IF_DATA_ACT 0x02                      // RW1, interrupt flag for timer catcher input action or PWM trigger or NRZI recv packet end/tran packet end
#define  RB_TMR_IF_FIFO_HF  0x04                      // RW1, interrupt flag for timer FIFO half
#define  RB_TMR_IF_DMA_END  0x08                      // RW1, interrupt flag for timer DMA completion
#define  RB_TMR_IF_FIFO_OV  0x10                      // RW1, interrupt flag for timer FIFO overflow
#define  RB_TMR_IF_DMA_ERR  0x20                      // RW1, interrupt flag for timer DMA respond error
#define  RB_TMR_IF_NRZI_AUX 0x40                      // RW1, interrupt flag for timer NRZI aux notice: NRZI recv error/tran start new data block
#define TMR_FIFO_COUNT      7
#define TMR_COUNT           0x08
#define TMR_CNT_END         0x0C
#define TMR_FIFO            0x10
#define TMR_DMA_NOW         0x14
#define TMR_DMA_BEG         0x18
#define TMR_DMA_END         0x1C

/* Address space define */
#define BA_CODE             ((PUINT32)0x00000000)     // point code base address
#define SZ_CODE             0x00100000                // code size
#define BA_SFR              ((PUINT32)0x00400000)     // point SFR base address
#define SZ_SFR              0x00010000                // SFR size
#ifdef	MEM_DATA
#if		MEM_DATA == 32
#define BA_RAM              ((PUINT32)0x00818000)     // point RAM base address
#define SZ_RAM              0x00008000                // RAM size
#endif
#if		MEM_DATA == 64
#define BA_RAM              ((PUINT32)0x00810000)     // point RAM base address
#define SZ_RAM              0x00010000                // RAM size
#endif
#if		MEM_DATA == 96
#define BA_RAM              ((PUINT32)0x00808000)     // point RAM base address
#define SZ_RAM              0x00018000                // RAM size
#endif
#endif
#ifdef	SZ_RAM
#else
#define BA_RAM              ((PUINT32)( ( R8_GLOB_MEM_CFG & RB_GLOB_MEM_CFG ) == 0x03 ? 0x00808000 : ( ( R8_GLOB_MEM_CFG & RB_GLOB_MEM_CFG ) == 0x02 ? 0x00818000 : 0x00810000 ) ))  // point RAM base address
#define SZ_RAM              ( ( R8_GLOB_MEM_CFG & RB_GLOB_MEM_CFG ) == 0x03 ? 0x00018000 : ( ( R8_GLOB_MEM_CFG & RB_GLOB_MEM_CFG ) == 0x02 ? 0x00008000 : 0x00010000 ) )             // RAM size
#endif
#define BA_XBUS             ((PUINT32)0x00C00000)     // point XBUS base address
#define SZ_XBUS             0x00100000                // XBUS size

#ifdef __cplusplus
}
#endif

#endif
