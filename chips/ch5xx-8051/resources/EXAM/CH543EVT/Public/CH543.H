/********************************** (C) COPYRIGHT *******************************
* File Name          : CH543.H
* Author             : WCH
* Version            : V1.0
* Date               : 
* Description        : Header file for CH543 microcontrollers.   Web:  http://wch.cn			 
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/

// 3 blocks: __BASE_TYPE__, __CH543_H__, __USB_DEF__

#ifndef __BASE_TYPE__
#define __BASE_TYPE__

#ifdef __cplusplus
extern "C" {
#endif

/*----- constant and type define -----------------------------------------*/

#ifndef TRUE
#define TRUE    1
#define FALSE   0
#endif
#ifndef NULL
#define NULL    0
#endif

#ifndef BOOL
typedef bit                             BOOL;
#endif
#ifndef UINT8
typedef unsigned char                   UINT8;
#endif
#ifndef UINT16
typedef unsigned short                  UINT16;
#endif
#ifndef UINT32
typedef unsigned long                   UINT32;
#endif
#ifndef UINT8D
typedef unsigned char  data             UINT8D;
#endif
#ifndef UINT16D
typedef unsigned short data             UINT16D;
#endif
#ifndef UINT32D
typedef unsigned long  data             UINT32D;
#endif
#ifndef UINT8I
typedef unsigned char  idata            UINT8I;
#endif
#ifndef UINT16I
typedef unsigned short idata            UINT16I;
#endif
#ifndef UINT32I
typedef unsigned long  idata            UINT32I;
#endif
#ifndef UINT8X
typedef unsigned char  xdata            UINT8X;
#endif
#ifndef UINT16X
typedef unsigned short xdata            UINT16X;
#endif
#ifndef UINT32X
typedef unsigned long  xdata            UINT32X;
#endif
#ifndef UINT8V
typedef unsigned char volatile          UINT8V;
#endif
#ifndef UINT8DV
typedef unsigned char volatile data     UINT8DV;
#endif
#ifndef UINT8XV
typedef unsigned char volatile xdata    UINT8XV;
#endif
#ifndef UINT8PV
typedef unsigned char volatile pdata    UINT8PV;
#endif
#ifndef UINT8C
typedef const unsigned char code        UINT8C;
#endif
#ifndef PUINT8
typedef unsigned char                 *PUINT8;
#endif
#ifndef PUINT16
typedef unsigned short                *PUINT16;
#endif
#ifndef PUINT32
typedef unsigned long                 *PUINT32;
#endif
#ifndef PUINT8I
typedef unsigned char  idata          *PUINT8I;
#endif
#ifndef PUINT16I
typedef unsigned short idata          *PUINT16I;
#endif
#ifndef PUINT32I
typedef unsigned long  idata          *PUINT32I;
#endif
#ifndef PUINT8X
typedef unsigned char  xdata          *PUINT8X;
#endif
#ifndef PUINT16X
typedef unsigned short xdata          *PUINT16X;
#endif
#ifndef PUINT32X
typedef unsigned long  xdata          *PUINT32X;
#endif
#ifndef PUINT8V
typedef unsigned char volatile        *PUINT8V;
#endif
#ifndef PUINT8DV
typedef unsigned char volatile data   *PUINT8DV;
#endif
#ifndef PUINT8XV
typedef unsigned char volatile xdata  *PUINT8XV;
#endif
#ifndef PUINT8PV
typedef unsigned char volatile pdata  *PUINT8PV;
#endif
#ifndef PUINT8C
typedef const unsigned char code      *PUINT8C;
#endif

#ifndef STRUCT_OFFSET
#define STRUCT_OFFSET( s, m )       ( (UINT8)( & (((s) *)0) -> (m) ) )  /* get the offset address for a member of a structure */
#endif

#ifdef __cplusplus
}
#endif

#endif  // __BASE_TYPE__

#ifndef __CH543_H__
#define __CH543_H__

#ifdef __cplusplus
extern "C" {
#endif

/*----- SFR --------------------------------------------------------------*/
/*  sbit are bit addressable, others are byte addressable */

/*  System Registers  */
sfr PSW             = 0xD0;         // program status word
 sbit CY            = PSW^7;        // carry flag
 sbit AC            = PSW^6;        // auxiliary carry flag
 sbit F0            = PSW^5;        // bit addressable general purpose flag 0
 sbit RS1           = PSW^4;        // register R0-R7 bank selection high bit
 sbit RS0           = PSW^3;        // register R0-R7 bank selection low bit
#define MASK_PSW_RS       0x18      // bit mask of register R0-R7 bank selection
// RS1 & RS0: register R0-R7 bank selection
//    00 - bank 0, R0-R7 @ address 0x00-0x07
//    01 - bank 1, R0-R7 @ address 0x08-0x0F
//    10 - bank 2, R0-R7 @ address 0x10-0x17
//    11 - bank 3, R0-R7 @ address 0x18-0x1F
 sbit OV            = PSW^2;        // overflow flag
 sbit F1            = PSW^1;        // bit addressable general purpose flag 1
 sbit P             = PSW^0;        // ReadOnly: parity flag
sfr ACC             = 0xE0;         // accumulator
sfr B               = 0xF0;         // general purpose register B
sfr SP              = 0x81;         // stack pointer
//sfr16 DPTR          = 0x82;         // DPTR pointer, little-endian
sfr DPL             = 0x82;         // data pointer low
sfr DPH             = 0x83;         // data pointer high
sfr SAFE_MOD        = 0xA1;         // WriteOnly: writing safe mode
//sfr CHIP_ID         = 0xA1;         // ReadOnly: reading chip ID
#define CHIP_ID           SAFE_MOD
sfr GLOBAL_CFG      = 0xB1;         // global config, Write@SafeMode
#define bBOOT_LOAD        0x20      // ReadOnly: boot loader status for discriminating BootLoader or Application: set 1 by power on reset, clear 0 by software reset
#define bSW_RESET         0x10      // software reset bit, auto clear by hardware
#define bCODE_WE          0x08      // enable flash-ROM (include code & Data-Flash) being program or erasing: 0=writing protect, 1=enable program and erase
#define bDATA_WE          0x04      // enable Data-Flash (flash-ROM data area) being program or erasing: 0=writing protect, 1=enable program and erase
#define bWDOG_EN          0x01      // enable watch-dog reset if watch-dog timer overflow: 0=as timer only, 1=enable reset if timer overflow

/* Clock and Sleep and Power Registers */
sfr PCON            = 0x87;         // power control and reset flag
#define SMOD              0x80      // baud rate selection for UART0 mode 1/2/3: 0=slow(Fsys/128 @mode2, TF1/32 @mode1/3, no effect for TF2),
                                    //   1=fast(Fsys/32 @mode2, TF1/16 @mode1/3, no effect for TF2)
#define bRST_FLAG1        0x20      // ReadOnly: recent reset flag high bit
#define bRST_FLAG0        0x10      // ReadOnly: recent reset flag low bit
#define MASK_RST_FLAG     0x30      // ReadOnly: bit mask of recent reset flag
#define RST_FLAG_SW       0x00
#define RST_FLAG_POR      0x10
#define RST_FLAG_WDOG     0x20
#define RST_FLAG_PIN      0x30
// bPC_RST_FLAG1 & bPC_RST_FLAG0: recent reset flag
//    00 - software reset, by bSW_RESET=1 @(bBOOT_LOAD=0 or bWDOG_EN=1), or USB PD reset if enabled
//    01 - power on reset
//    10 - watch-dog timer overflow reset
//    11 - external input manual reset by RST pin
#define GF1               0x08      // general purpose flag bit 1
#define GF0               0x04      // general purpose flag bit 0
#define PD                0x02      // power-down enable bit, auto clear by wake-up hardware
sfr POWER_CFG       = 0xBA;         // power config, Write@SafeMode
#define bPWR_SLP_MODE1    0x80      // power sleep mode high bit
#define bPWR_SLP_MODE0    0x40      // power sleep mode low bit
#define MASK_SLP_MODE     0xC0      // power sleep mode
// bPWR_SLP_MODE1 & bPWR_SLP_MODE0: power sleep mode
//    00: shutdown, deep sleep and slower waking, BGR/LDO/ROM/OSC all off
//    01: sleep and slow waking, BGR/Vcore on (LDO3V3 on/off), ROM/OSC off
//    10: halt and fast waking, BGR/LDO/ROM on, OSC off
//    11: idle and immediate waking, BGR/LDO/ROM/OSC on
#define bLDO_3V3_SEL      0x20      // VDD/V33 LDO voltage selection: 0=3.3V, 1=4.7V
#define bLDO_3V3_OFF      0x10      // disable VDD/V33 LDO: 0=enable LDO for I/O & USB, 1=disable LDO, short VDD/V33 and VDD12 power
#define bLDO_CORE_VOL     0x08      // core voltage mode: 0=normal, 1=raised for performance
#define MASK_ULLDO_VOL    0x07      // bit mask of ULLDO voltage selection
sfr CLOCK_CFG       = 0xB9;         // system clock config, Write@SafeMode
#define bUSB_CLK_EN       0x40      // USB clock enable: 0=disable for low power, 1=enable
#define bWDOG_IF_TO       0x20      // ReadOnly: watch-dog timer overflow interrupt flag, cleared by reload watch-dog count or auto cleared when MCU enter interrupt routine
#define bUART_2X          0x10      // UART clock double frequency mode: 0=Fsys, 1=2*Fsys for 2X baud rate
#define MASK_SYS_CK_SEL   0x07      // bit mask of system clock Fsys selection
/*
   Fosc = 48MHz if MASK_SYS_CK_SEL != 000, <200KHz if MASK_SYS_CK_SEL == 000
   Fusb4x = Fosc
              MASK_SYS_CK_SEL[2] [1] [0]
   Fsys = Fosc/1   =  48MHz:  1   1   1  @bLDO_CORE_VOL=1
   Fsys = Fosc/2   =  24MHz:  1   1   0  slow wakeup
   Fsys = Fosc/2   =  24MHz:  1   0   1  fast wakeup
   Fsys = Fosc/3   =  16MHz:  1   0   0
   Fsys = Fosc/4   =  12MHz:  0   1   1
   Fsys = Fosc/16  =   3MHz:  0   1   0
   Fsys = Fosc/128 = 375KHz:  0   0   1
   Fsys = Fosc/128 < 100KHz:  0   0   0  low frequency mode
*/
sfr WAKE_CTRL       = 0xA9;         // wake-up control, Write@SafeMode
#define bWAK_BY_USB       0x80      // enable wake-up by USB event
#define bWAK_P1_7_LO      0x40      // enable wake-up by pin P1.7 low level
#define bWAK_P1_5_LO      0x20      // enable wake-up by pin P1.5 low level
#define bWAK_P1_4_LO      0x10      // enable wake-up by pin P1.4 low level
#define bWAK_P1_3_LO      0x08      // enable wake-up by pin P1.3 low level
#define bWAK_P3_3_LO      0x04      // enable wake-up by pin P3.3 low level
#define bWAK_INT0E        0x02      // enable wake-up by pin INT0 edge
#define bWAK_RXD0_LO      0x01      // enable wake-up by RXD low level
sfr WDOG_COUNT      = 0xFF;         // watch-dog count, count by clock frequency Fsys/131072

/*  Interrupt Registers  */
sfr IE              = 0xA8;         // interrupt enable
 sbit EA            = IE^7;         // enable global interrupts: 0=disable, 1=enable if E_DIS=0
 sbit E_DIS         = IE^6;         // disable global interrupts, intend to inhibit interrupt during some flash-ROM operation: 0=enable if EA=1, 1=disable
 sbit ET2           = IE^5;         // enable timer2 interrupt
 sbit ES            = IE^4;         // enable UART0 interrupt
 sbit ET1           = IE^3;         // enable timer1 interrupt
 sbit EX1           = IE^2;         // enable external interrupt INT1
 sbit ET0           = IE^1;         // enable timer0 interrupt
 sbit EX0           = IE^0;         // enable external interrupt INT0
sfr IP              = 0xB8;         // interrupt priority and current priority
 sbit PH_FLAG       = IP^7;         // ReadOnly: high level priority action flag
 sbit PL_FLAG       = IP^6;         // ReadOnly: low level priority action flag
// PH_FLAG & PL_FLAG: current interrupt priority
//    00 - no interrupt now
//    01 - low level priority interrupt action now
//    10 - high level priority interrupt action now
//    11 - unknown error
 sbit PT2           = IP^5;         // timer2 interrupt priority level
 sbit PS            = IP^4;         // UART0 interrupt priority level
 sbit PT1           = IP^3;         // timer1 interrupt priority level
 sbit PX1           = IP^2;         // external interrupt INT1 priority level
 sbit PT0           = IP^1;         // timer0 interrupt priority level
 sbit PX0           = IP^0;         // external interrupt INT0 priority level
sfr IE_EX           = 0xE8;         // extend interrupt enable
 sbit IE_WDOG       = IE_EX^7;      // enable watch-dog timer interrupt
 sbit IE_GPIO       = IE_EX^6;      // enable GPIO input interrupt
 sbit IE_PWMX       = IE_EX^5;      // enable PWMX/CMP interrupt
 sbit IE_CMP        = IE_EX^5;      // enable PWMX/CMP interrupt
 sbit IE_USBPD      = IE_EX^4;      // enable USBPD or P1.0/P1.1 interrupt
 sbit IE_ADC        = IE_EX^3;      // enable ADC/TKEY interrupt
 sbit IE_USB        = IE_EX^2;      // enable USB or P3.6/P3.7 interrupt
 sbit IE_I2C        = IE_EX^1;      // enable I2C interrupt
 sbit IE_SPI0       = IE_EX^0;      // enable SPI0 interrupt
sfr IP_EX           = 0xE9;         // extend interrupt priority
#define bIP_LEVEL         0x80      // ReadOnly: current interrupt nested level: 0=no interrupt or two levels, 1=one level
#define bIP_GPIO          0x40      // GPIO input interrupt priority level
#define bIP_PWMX          0x20      // PWMX/CMP interrupt priority level
#define bIP_CMP           0x20      // PWMX/CMP interrupt priority level
#define bIP_USBPD         0x10      // USBPD or P1.0/P1.1 interrupt priority level
#define bIP_ADC           0x08      // ADC/TKEY interrupt priority level
#define bIP_USB           0x04      // USB or P3.6/P3.7 interrupt priority level
#define bIP_I2C           0x02      // I2C interrupt priority level
#define bIP_SPI0          0x01      // SPI0 interrupt priority level
sfr GPIO_IE         = 0xB2;         // GPIO interrupt enable, Write@SafeMode
#define bIE_IO_EDGE       0x80      // enable GPIO edge interrupt: 0=low/high level, 1=falling/rising edge
#define bIE_P1_7_LO       0x40      // enable interrupt by pin P1.7 low level / falling edge
#define bIE_P1_5_LO       0x20      // enable interrupt by pin P1.5 low level / falling edge
#define bIE_P1_4_LO       0x10      // enable interrupt by pin P1.4 low level / falling edge
#define bIE_P1_3_LO       0x08      // enable interrupt by pin P1.3 low level / falling edge
#define bIE_RST_HI        0x04      // enable interrupt by pin RST high level / rising edge
#define bIE_P3_1_LO       0x02      // enable interrupt by pin P3.1 low level / falling edge
#define bIE_RXD0_LO       0x01      // enable interrupt by RXD low level / falling edge

/*  FlashROM and Data-Flash Registers  */
#define ROM_PAGE_SIZE     0x40      // FlashROM page size ( number of bytes )
sfr16 ROM_ADDR      = 0x84;         // address for flash-ROM, little-endian
sfr ROM_ADDR_L      = 0x84;         // address low byte for flash-ROM
sfr ROM_ADDR_H      = 0x85;         // address high byte for flash-ROM
//sfr16 ROM_DATA_LO   = 0x84;         // ReadOnly: low word data (16 bits) for flash-ROM reading, little-endian
#define ROM_DATA_LO       ROM_ADDR
//sfr ROM_DATA_LL     = 0x84;         // ReadOnly: data low byte of low word for flash-ROM reading
#define ROM_DATA_LL       ROM_ADDR_L
//sfr ROM_DATA_LH     = 0x85;         // ReadOnly: data high byte of low word for flash-ROM reading
#define ROM_DATA_LH       ROM_ADDR_H
sfr16 ROM_DATA_HI   = 0x8E;         // ReadOnly: high word data (16 bits) for flash-ROM reading, little-endian
sfr ROM_DATA_HL     = 0x8E;         // ReadOnly: data low byte of high word for flash-ROM reading
sfr ROM_DATA_HH     = 0x8F;         // ReadOnly: data high byte of high word for flash-ROM reading
//sfr ROM_DAT_BUF     = 0x8E;         // data byte buffer for flash-ROM program and erasing
#define ROM_DAT_BUF       ROM_DATA_HL
//sfr ROM_BUF_MOD     = 0x8F;         // data buffer mode and end address for flash-ROM program and erasing
#define ROM_BUF_MOD       ROM_DATA_HH
#define bROM_BUF_BYTE     0x80      // flash-ROM data buffer mode: 0=data block (1~64bytes) to program from xRAM pointed by DPTR, 1=only one byte for program or erasing from SFR ROM_DAT_BUF
#define MASK_ROM_ADDR     0x3F      // bit mask for end address for flash-ROM block program if bROM_BUF_BYTE=0
sfr ROM_CTRL        = 0x86;         // WriteOnly: flash-ROM control
#define ROM_CMD_PROG      0x9A      // WriteOnly: flash-ROM or Data-Flash byte/page program operation command, for changing some ROM bit of a byte from 0 to 1
#define ROM_CMD_ERASE     0xA6      // WriteOnly: flash-ROM or Data-Flash page erase operation command, for changing all ROM bit of 64-Bytes from 1 to 0
#define ROM_CMD_RD_OTP    0x8D      // WriteOnly: OTP area dword read operation command
#define ROM_CMD_PG_OTP    0x99      // WriteOnly: OTP area byte/page program operation command
//sfr ROM_STATUS      = 0x86;         // ReadOnly: flash-ROM status
#define ROM_STATUS        ROM_CTRL
#define bROM_ADDR_OK      0x40      // ReadOnly: flash-ROM operation address valid flag, can be reviewed before or after operation: 0=invalid parameter, 1=address valid
#define bROM_CMD_ERR      0x02      // ReadOnly: flash-ROM operation command error flag: 0=command accepted, 1=unknown command or operation time out

/*  Port Registers  */
sfr P1              = 0x90;         // port 1 input & output
 sbit P1_7          = P1^7;
 sbit P1_6          = P1^6;
 sbit P1_5          = P1^5;
 sbit P1_4          = P1^4;
 sbit P1_3          = P1^3;
 sbit P1_2          = P1^2;
 sbit P1_1          = P1^1;
 sbit P1_0          = P1^0;
 sbit SCK           = P1^7;         // serial clock for SPI0
 sbit SDA           = P1^7;         // serial data for I2CS
 sbit TXD_          = P1^7;         // alternate pin for TXD if MASK_UART_I2C=01
 sbit AIN7          = P1^7;         // AIN7 for ADC
 sbit MISO          = P1^6;         // master serial data input or slave serial data output for SPI0
 sbit SCL           = P1^6;         // serial clock for I2CS
 sbit RXD_          = P1^6;         // alternate pin for RXD if MASK_UART_I2C=01
 sbit PWM2          = P1^6;         // PWM output for PWM2
 sbit AIN6          = P1^6;         // AIN6 for ADC
 sbit MOSI          = P1^5;         // master serial data output or slave serial data input for SPI0
 sbit PWM0          = P1^5;         // PWM output for PWM0
 sbit AIN5          = P1^5;         // AIN5 for ADC
 sbit T2_           = P1^4;         // alternate pin for T2
 sbit SCS           = P1^4;         // slave chip-selection input for SPI0
 sbit PWM3          = P1^4;         // PWM output for PWM3
 sbit AIN4          = P1^4;         // AIN4 for ADC
 sbit FB            = P1^3;         // current sink
 sbit CM            = P1^3;         // comparator/buffer input-
 sbit AIN11         = P1^3;         // AIN11 for ADC
 sbit INT0_         = P1^2;         // alternate pin for INT0
 sbit CO            = P1^2;         // comparator/buffer output
 sbit AIN10         = P1^2;         // AIN10 for ADC
 sbit T2EX          = P1^1;         // external trigger input for timer2 reload & capture
 sbit CAP2          = P1^1;         // capture2 input for timer2
 sbit CC2           = P1^1;         // CC2 for USB type-C / USB PD
 sbit AIN1          = P1^1;         // AIN1 for ADC
 sbit T2            = P1^0;         // external count input
 sbit CAP1          = P1^0;         // capture1 input for timer2
 sbit CC1           = P1^0;         // CC1 for USB type-C / USB PD
 sbit AIN0          = P1^0;         // AIN0 for ADC
sfr P1_MOD_OC       = 0x92;         // port 1 output mode: 0=push-pull, 1=open-drain
sfr P1_DIR_PU       = 0x93;         // port 1 direction for push-pull or pullup enable for open-drain
// Pn_MOD_OC & Pn_DIR_PU: pin input & output configuration for Pn (n=1/3)
//   0 0:  float input only, without pullup resistance
//   0 1:  push-pull output, strong driving high level and low level
//   1 0:  open-drain output and input without pullup resistance
//   1 1:  quasi-bidirectional (standard 8051 mode), open-drain output and input with pullup resistance, just driving high level strongly for 2 clocks if turning output level from low to high
#define bSCK              0x80      // serial clock for SPI0
#define bSDA              0x80      // serial data for I2CS
#define bTXD_             0x80      // alternate pin for TXD if MASK_UART_I2C=01
#define bAIN7             0x80      // AIN7 for ADC
#define bMISO             0x40      // master serial data input or slave serial data output for SPI0
#define bSCL              0x40      // serial clock for I2CS
#define bRXD_             0x40      // alternate pin for RXD if MASK_UART_I2C=01
#define bPWM2             0x40      // PWM output for PWM2
#define bAIN6             0x40      // AIN6 for ADC
#define bMOSI             0x20      // master serial data output or slave serial data input for SPI0
#define bPWM0             0x20      // PWM output for PWM0
#define bAIN5             0x20      // AIN5 for ADC
#define bT2_              0x10      // alternate pin for T2
#define bSCS              0x10      // slave chip-selection input for SPI0
#define bPWM3             0x10      // PWM output for PWM3
#define bAIN4             0x10      // AIN4 for ADC
#define bFB               0x08      // current sink
#define bCM               0x08      // comparator/buffer input-
#define bAIN11            0x08      // AIN11 for ADC
#define bINT0_            0x04      // alternate pin for INT0
#define bCO               0x04      // comparator/buffer output
#define bAIN10            0x04      // AIN10 for ADC
#define bT2EX             0x02      // external trigger input for timer2 reload & capture
#define bCAP2             bT2EX     // capture2 input for timer2
#define bCC2              0x02      // CC2 for USB type-C / USB PD
#define bAIN1             0x02      // AIN1 for ADC
#define bT2               0x01      // external count input or clock output for timer2
#define bCAP1             bT2       // capture1 input for timer2
#define bCC1              0x01      // CC1 for USB type-C / USB PD
#define bAIN0             0x01      // AIN0 for ADC
sfr P3              = 0xB0;         // port 3 input & output
 sbit P3_7          = P3^7;
 sbit P3_6          = P3^6;
 sbit P3_5          = P3^5;
 sbit P3_4          = P3^4;
 sbit P3_3          = P3^3;
 sbit P3_2          = P3^2;
 sbit P3_1          = P3^1;
 sbit P3_0          = P3^0;
 sbit UDM           = P3^7;         // ReadOnly: pin UDM input
 sbit DM            = P3^7;         // ReadOnly: pin UDM input
 sbit TXD10_        = P3^7;         // alternate pin for TXD if MASK_UART_I2C=10
 sbit RXD11_        = P3^7;         // alternate pin for RXD if MASK_UART_I2C=11
 sbit AIN9          = P3^7;         // AIN9 for ADC
 sbit UDP           = P3^6;         // ReadOnly: pin UDP input
 sbit DP            = P3^6;         // ReadOnly: pin UDP input
 sbit TXD11_        = P3^6;         // alternate pin for TXD if MASK_UART_I2C=11
 sbit RXD10_        = P3^6;         // alternate pin for RXD if MASK_UART_I2C=10
 sbit AIN8          = P3^6;         // AIN8 for ADC
 sbit T1            = P3^5;         // external count input for timer1
 sbit HVIO          = P3^5;         // HVIO pin for high voltage input / output
 sbit T0            = P3^4;         // external count input for timer0
 sbit PWM1          = P3^4;         // PWM output for PWM1
 sbit INT1          = P3^3;         // external interrupt 1 input
 sbit PWM2_         = P3^3;         // alternate pin for PWM2
 sbit INT0          = P3^2;         // external interrupt 0 input
 sbit AIN2          = P3^2;         // AIN2 for ADC
 sbit TXD           = P3^1;         // TXD output for UART0
 sbit PWM1_         = P3^1;         // alternate pin for PWM1
 sbit SDA_          = P3^1;         // alternate pin for SDA
 sbit RXD           = P3^0;         // RXD input for UART0
 sbit PWM0_         = P3^0;         // alternate pin for PWM0
 sbit SCL_          = P3^0;         // alternate pin for SCL
sfr P3_MOD_OC       = 0x96;         // port 3 output mode: 0=push-pull, 1=open-drain
sfr P3_DIR_PU       = 0x97;         // port 3 direction for push-pull or pullup enable for open-drain
#define bUDM              0x80      // ReadOnly: pin UDM input
#define bDM               0x80      // ReadOnly: pin UDM input
#define bAIN9             0x80      // AIN9 for ADC
#define bUDP              0x40      // ReadOnly: pin UDP input
#define bDP               0x40      // ReadOnly: pin UDP input
#define bAIN8             0x40      // AIN8 for ADC
#define bT1               0x20      // external count input for timer1
#define bHVIO             0x20      // HVIO pin for high voltage input / output
#define bT0               0x10      // external count input for timer0
#define bPWM1             0x10      // PWM output for PWM1
#define bINT1             0x08      // external interrupt 1 input
#define bPWM2_            0x08      // alternate pin for PWM2
#define bINT0             0x04      // external interrupt 0 input
#define bAIN2             0x04      // AIN2 for ADC
#define bTXD              0x02      // TXD output for UART0
#define bPWM1_            0x02      // alternate pin for PWM1
#define bSDA_             0x02      // alternate pin for SDA
#define bRXD              0x01      // RXD input for UART0
#define bPWM0_            0x01      // alternate pin for PWM0
#define bSCL_             0x01      // alternate pin for SCL
sfr P1_ANA_PIN      = 0x94;         // port 1 analog pin mode: 0=digital or analog pin, 1=analog pin only, digital pin disabled for low power
#define bP1_7_ANA         0x80      // P1.7/AIN7 pure analog input mode
#define bP1_6_ANA         0x40      // P1.6/AIN6 pure analog input mode
#define bP1_5_ANA         0x20      // P1.5/AIN5 pure analog input mode
#define bP1_4_ANA         0x10      // P1.4/AIN4 pure analog input mode
#define bP1_3_ANA         0x08      // P1.3/AIN11 pure analog input mode
#define bP1_2_ANA         0x04      // P1.2/AIN10 pure analog input mode
#define bP1_1_ANA         0x02      // P1.1/AIN1 pure analog input mode
#define bP1_0_ANA         0x01      // P1.0/AIN0 pure analog input mode
sfr P3_ANA_PIN      = 0x95;         // port 3 analog pin mode: 0=digital or analog pin, 1=analog pin only, digital pin disabled for low power
#define bP3_7_ANA         0x80      // P3.7/AIN9 pure analog input mode
#define bP3_6_ANA         0x40      // P3.6/AIN8 pure analog input mode
#define bP3_2_ANA         0x04      // P3.2/AIN2 pure analog input mode
#define bUSB_OUT_VOL1     0x02      // UDP/UDM output voltage selection high bit
#define bUSB_OUT_VOL0     0x01      // UDP/UDM output voltage selection low bit
#define MASK_USB_OUT_VOL  0x03      // bit mask of UDP/UDM output voltage selection
// bUSB_OUT_VOL1 & bUSB_OUT_VOL0: UDP/UDM output voltage selection
//   00: disable output
//   01: output 0.6V on UDM if bUDP_PDE=1 or 0.6V on UDP if bUDM_PDE=1, or short UDP and UDM if both bUDP_PDE=0 and bUDM_PDE=0
//   10: output 1.2V on both UDP/UDM
//   11: output 2.7V on both UDP/UDM

sfr PIN_RST         = 0xB3;         // RST pin config
#define bIO_INT_ACT       0x80      // ReadOnly: GPIO interrupt request action status
#define bRST              0x40      // ReadOnly: pin RST input
#define bCP               0x40      // comparator/buffer input+
#define bAIN3             0x40      // AIN3 for ADC
#define bRST_ANA          0x08      // RST/AIN3 pure analog input mode
#define bRST_VOL_SEL      0x04      // RST analog voltage scale selection: 0=100% without pulldown, 1=25% with 40K pulldown
#define bRST_PD1          0x02      // RST pulldown current selection high bit
#define bRST_PD0          0x01      // RST pulldown current selection low bit
#define MASK_RST_PD       0x03      // bit mask of RST pulldown current selection
// bRST_PD1 & bRST_PD0: RST pulldown current selection
//   00: disable pulldown
//   01: sink ~14.4mA@5V@VDD=4V7(350R), ~8mA@5V@VDD=3V3
//   10: sink ~2.3mA@5V@VDD=4V7(2K2), ~1.4mA@5V@VDD=3V3
//   11: sink 5uA
sfr PIN_MISC        = 0x91;         // miscellaneous pin control
#define bP1_7_WPU         0x80      // P1.7 weak pullup enable: 0=disable, 1=enable
#define bVHV_SINK         0x40      // VDD12/VHV sink current enable: 0=disable, 1=enable sink 9.2mA@5V@VDD=4V7, 6.6mA@5V@VDD=3V3
#define bP3_5_PDE         0x20      // P3.5 pulldown resistance/current enable: 0=disable, 1=enable
#define bINT0_POLAR       0x10      // INT0 polarity selection: 0=low action, 1=high action
#define bWAK_BY_USBPD     0x08      // enable wake-up by USB PD event

sfr PIN_FUNC        = 0xAA;         // pin function selection
#define bPWM1_PIN_X       0x80      // PWM1 alternate pin enable: 0=PWM1 on P3.4, 1=PWM1 on P3.1
#define bPWM0_PIN_X       0x40      // PWM0 alternate pin enable: 0=PWM0 on P1.5, 1=PWM0 on P3.0
#define bPWM2_PIN_X       0x20      // PWM2 alternate pin enable: 0=PWM2 on P1.6, 1=PWM2 on P3.3
#define bUART0_PIN_X      0x10      // UART0 alternate pin mode
#define bI2C_PIN_X        0x08      // I2C alternate pin mode
#define MASK_UART_I2C     0x18      // bit mask of UART0/I2C alternate pin
// bUART0_PIN_X & bI2C_PIN_X: UART0/I2C alternate pin
//    00: RXD/TXD on P3.0/P3.1, SCL/SDA on P1.6/P1.7
//    01: RXD/TXD on P1.6/P1.7, SCL/SDA on P3.0/P3.1
//    10: RXD/TXD on P3.6/P3.7, SCL/SDA on P1.6/P1.7
//    11: RXD/TXD on P3.7/P3.6, SCL/SDA on P1.6/P1.7
#define bINT0_PIN_X       0x04      // INT0 alternate pin enable: 0=INT0 on P3.2, 1=INT0 on P1.2
#define bT2EX_PIN_X       0x02      // T2EX/CAP2 alternate pin enable: 0=T2EX/CAP2 on P1.1, 1=T2EX/CAP2 on RST
#define bT2_PIN_X         0x01      // T2/CAP1 alternate pin enable: 0=T2/CAP1 on P1.0, 1=T2/CAP1 on P1.4
sfr XBUS_AUX        = 0xA2;         // xBUS auxiliary setting
#define bUART0_TX         0x80      // ReadOnly: indicate UART0 transmittal status
#define bUART0_RX         0x40      // ReadOnly: indicate UART0 receiving status
#define bSAFE_MOD_ACT     0x20      // ReadOnly: safe mode action status
#define GF2               0x08      // general purpose flag bit 2
#define bDPTR_AUTO_INC    0x04      // enable DPTR auto increase if finished MOVX_@DPTR instruction
#define DPS               0x01      // dual DPTR selection: 0=DPTR0 selected, 1=DPTR1 selected

/*  Timer0/1 Registers  */
sfr TCON            = 0x88;         // timer 0/1 control and external interrupt control
 sbit TF1           = TCON^7;       // timer1 overflow & interrupt flag, auto cleared when MCU enter interrupt routine
 sbit TR1           = TCON^6;       // timer1 run enable
 sbit TF0           = TCON^5;       // timer0 overflow & interrupt flag, auto cleared when MCU enter interrupt routine
 sbit TR0           = TCON^4;       // timer0 run enable
 sbit IE1           = TCON^3;       // INT1 interrupt flag, auto cleared when MCU enter interrupt routine
 sbit IT1           = TCON^2;       // INT1 interrupt type: 0=low level action, 1=falling edge action
 sbit IE0           = TCON^1;       // INT0 interrupt flag, auto cleared when MCU enter interrupt routine
 sbit IT0           = TCON^0;       // INT0 interrupt type: 0=low level action, 1=falling edge action
sfr TMOD            = 0x89;         // timer 0/1 mode
#define bT1_GATE          0x80      // gate control of timer1: 0=timer1 run enable while TR1=1, 1=timer1 run enable while P3.3 (INT1) pin is high and TR1=1
#define bT1_CT            0x40      // counter or timer mode selection for timer1: 0=timer, use internal clock, 1=counter, use P3.5 (T1) pin falling edge as clock
#define bT1_M1            0x20      // timer1 mode high bit
#define bT1_M0            0x10      // timer1 mode low bit
#define MASK_T1_MOD       0x30      // bit mask of timer1 mode
// bT1_M1 & bT1_M0: timer1 mode
//   00: mode 0, 13-bit timer or counter by cascaded TH1 and lower 5 bits of TL1, the upper 3 bits of TL1 are ignored
//   01: mode 1, 16-bit timer or counter by cascaded TH1 and TL1
//   10: mode 2, TL1 operates as 8-bit timer or counter, and TH1 provide initial value for TL1 auto-reload
//   11: mode 3, stop timer1
#define bT0_GATE          0x08      // gate control of timer0: 0=timer0 run enable while TR0=1, 1=timer0 run enable while INT0 pin is high and TR0=1
#define bT0_CT            0x04      // counter or timer mode selection for timer0: 0=timer, use internal clock, 1=counter, use P3.4 (T0) pin falling edge as clock
#define bT0_M1            0x02      // timer0 mode high bit
#define bT0_M0            0x01      // timer0 mode low bit
#define MASK_T0_MOD       0x03      // bit mask of timer0 mode
// bT0_M1 & bT0_M0: timer0 mode
//   00: mode 0, 13-bit timer or counter by cascaded TH0 and lower 5 bits of TL0, the upper 3 bits of TL0 are ignored
//   01: mode 1, 16-bit timer or counter by cascaded TH0 and TL0
//   10: mode 2, TL0 operates as 8-bit timer or counter, and TH0 provide initial value for TL0 auto-reload
//   11: mode 3, TL0 is 8-bit timer or counter controlled by standard timer0 bits, TH0 is 8-bit timer using TF1 and controlled by TR1, timer1 run enable if it is not mode 3
sfr TL0             = 0x8A;         // low byte of timer 0 count
sfr TL1             = 0x8B;         // low byte of timer 1 count
sfr TH0             = 0x8C;         // high byte of timer 0 count
sfr TH1             = 0x8D;         // high byte of timer 1 count

/*  UART0 Registers  */
sfr SCON            = 0x98;         // UART0 control (serial port control)
 sbit SM0           = SCON^7;       // UART0 mode bit0, selection data bit: 0=8 bits data, 1=9 bits data
 sbit SM1           = SCON^6;       // UART0 mode bit1, selection baud rate: 0=fixed, 1=variable
// SM0 & SM1: UART0 mode
//    00 - mode 0, shift Register, baud rate fixed at: Fsys/12
//    01 - mode 1, 8-bit UART,     baud rate = variable by timer1 or timer2 overflow rate
//    10 - mode 2, 9-bit UART,     baud rate fixed at: Fsys/128@SMOD=0, Fsys/32@SMOD=1
//    11 - mode 3, 9-bit UART,     baud rate = variable by timer1 or timer2 overflow rate
 sbit SM2           = SCON^5;       // enable multi-device communication in mode 2/3
#define MASK_UART0_MOD    0xE0      // bit mask of UART0 mode
 sbit REN           = SCON^4;       // enable UART0 receiving
 sbit TB8           = SCON^3;       // the 9th transmitted data bit in mode 2/3
 sbit RB8           = SCON^2;       // 9th data bit received in mode 2/3, or stop bit received for mode 1
 sbit TI            = SCON^1;       // transmit interrupt flag, set by hardware after completion of a serial transmittal, need software clear
 sbit RI            = SCON^0;       // receive interrupt flag, set by hardware after completion of a serial receiving, need software clear
sfr SBUF            = 0x99;         // UART0 data buffer: reading for receiving, writing for transmittal

/*  Timer2/Capture Registers  */
sfr T2CON           = 0xC8;         // timer 2 control
 sbit TF2           = T2CON^7;      // timer2 overflow & interrupt flag, need software clear, the flag will not be set when either RCLK=1 or TCLK=1
 sbit CAP1F         = T2CON^7;      // timer2 capture 1 interrupt flag, set by T2 edge trigger if bT2_CAP1_EN=1, need software clear
 sbit EXF2          = T2CON^6;      // timer2 external flag, set by T2EX edge trigger if EXEN2=1, need software clear
 sbit RCLK          = T2CON^5;      // selection UART0 receiving clock: 0=timer1 overflow pulse, 1=timer2 overflow pulse
 sbit TCLK          = T2CON^4;      // selection UART0 transmittal clock: 0=timer1 overflow pulse, 1=timer2 overflow pulse
 sbit EXEN2         = T2CON^3;      // enable T2EX trigger function: 0=ignore T2EX, 1=trigger reload or capture by T2EX edge
 sbit TR2           = T2CON^2;      // timer2 run enable
 sbit C_T2          = T2CON^1;      // timer2 clock source selection: 0=timer base internal clock, 1=external edge counter base T2 falling edge
 sbit CP_RL2        = T2CON^0;      // timer2 function selection (force 0 if RCLK=1 or TCLK=1): 0=timer and auto reload if count overflow or T2EX edge, 1=capture by T2EX edge
sfr T2MOD           = 0xC9;         // timer 2 mode and timer 0/1/2 clock mode
#define bTMR_CLK          0x80      // fastest internal clock mode for timer 0/1/2 under faster clock mode: 0=use divided clock, 1=use original Fsys as clock without dividing
#define bT2_CLK           0x40      // timer2 internal clock frequency selection: 0=standard clock, Fsys/12 for timer mode, Fsys/4 for UART0 clock mode,
                                    //   1=faster clock, Fsys/4 @bTMR_CLK=0 or Fsys @bTMR_CLK=1 for timer mode, Fsys/2 @bTMR_CLK=0 or Fsys @bTMR_CLK=1 for UART0 clock mode
#define bT1_CLK           0x20      // timer1 internal clock frequency selection: 0=standard clock, Fsys/12, 1=faster clock, Fsys/4 if bTMR_CLK=0 or Fsys if bTMR_CLK=1
#define bT0_CLK           0x10      // timer0 internal clock frequency selection: 0=standard clock, Fsys/12, 1=faster clock, Fsys/4 if bTMR_CLK=0 or Fsys if bTMR_CLK=1
#define bT2_CAP_M1        0x08      // timer2 capture mode high bit
#define bT2_CAP_M0        0x04      // timer2 capture mode low bit
#define MASK_CAP_MODE     0x0C      // bit mask of timer2 capture mode
// bT2_CAP_M1 & bT2_CAP_M0: timer2 capture point selection
//   x0: from falling edge to falling edge
//   01: from any edge to any edge (level changing)
//   11: from rising edge to rising edge
#define T2OE              0x02      // enable timer2 generated clock output: 0=disable output, 1=enable clock output at T2 pin, frequency = TF2/2
#define bT2_CAP1_EN       0x01      // enable T2 trigger function for capture 1 of timer2 if RCLK=0 & TCLK=0 & CP_RL2=1 & C_T2=0 & T2OE=0
sfr16 RCAP2         = 0xCA;         // reload & capture value, little-endian
sfr RCAP2L          = 0xCA;         // low byte of reload & capture value
sfr RCAP2H          = 0xCB;         // high byte of reload & capture value
sfr16 T2COUNT       = 0xCC;         // counter, little-endian
sfr TL2             = 0xCC;         // low byte of timer 2 count
sfr TH2             = 0xCD;         // high byte of timer 2 count
sfr16 T2CAP1        = 0xCE;         // ReadOnly: capture 1 value for timer2
sfr T2CAP1L         = 0xCE;         // ReadOnly: capture 1 value low byte for timer2
sfr T2CAP1H         = 0xCF;         // ReadOnly: capture 1 value high byte for timer2

/*  PWMX Registers  */
sfr PWM_DATA2       = 0x9A;         // PWM data for PWM2, low nibble is high 4 bits of PWM0 data for 12 bits mode
sfr PWM_DATA1       = 0x9B;         // PWM data for PWM1, low byte of PWM1 data for 12 bits mode
sfr PWM_DATA0       = 0x9C;         // PWM data for PWM0, low byte of PWM0 data for 12 bits mode
sfr PWM_CTRL        = 0x9D;         // PWM control
#define bPWM_IE_END       0x80      // interrupt enable for cycle end
#define bPWM1_POLAR       0x40      // PWM1 output polarity: 0=default low and high action, 1=default high and low action
#define bPWM0_POLAR       0x20      // PWM0 output polarity: 0=default low and high action, 1=default high and low action
#define bPWM_IF_END       0x10      // interrupt flag for cycle end, write 1 to clear or load new data into PWM_DATA0 to clear
#define bPWM1_OUT_EN      0x08      // PWM1 output enable
#define bPWM0_OUT_EN      0x04      // PWM0 output enable
#define bPWM_CLR_ALL      0x02      // force clear FIFO and count of PWMX
sfr PWM_CK_SE       = 0x9E;         // PWM clock divisor setting
sfr PWM_CTRL2       = 0x9F;         // PWM extend control
#define bPWM_MOD_12BIT    0x80      // PWM data 12 bits width mode: 0=8 bits data, 1=12 bits data
#define bPWM_STAG_STAT    0x40      // ReadOnly: PWM stagger status: 0=inhibit PWM1/PWM3 if stagger enable, 1=inhibit PWM0/PWM2 if stagger enable
#define bPWM2_3_STAG_EN   0x20      // PWM2/PWM3 stagger output mode enable
#define bPWM0_1_STAG_EN   0x10      // PWM0/PWM1 stagger output mode enable
#define bPWM1_3_DELAY     0x08      // PWM1/PWM3 delay output mode: 0=normal output, 1=delay 1 clock output
#define bPWM3_OUT_EN      0x02      // PWM3 output enable
#define bPWM2_OUT_EN      0x01      // PWM2 output enable
sfr PWM_DATA3       = 0xA3;         // PWM data for PWM3, low nibble is high 4 bits of PWM1 data for 12 bits mode
sfr16 PWM_CYCLE     = 0xB6;         // PWM cycle data
sfr PWM_CYCLE_L     = 0xB6;         // PWM cycle data low byte
sfr PWM_CYCLE_H     = 0xB7;         // PWM cycle data high 4 bits

/*  SPI0/Master0/Slave Registers  */
sfr SPI0_STAT       = 0xF8;         // SPI 0 status
 sbit S0_FST_ACT    = SPI0_STAT^7;  // ReadOnly: indicate first byte received status for SPI0
 sbit S0_IF_OV      = SPI0_STAT^6;  // interrupt flag for slave mode FIFO overflow, direct bit address clear or write 1 to clear
 sbit S0_IF_FIRST   = SPI0_STAT^5;  // interrupt flag for first byte received, direct bit address clear or write 1 to clear
 sbit S0_IF_BYTE    = SPI0_STAT^4;  // interrupt flag for a byte data exchanged, direct bit address clear or write 1 to clear or accessing FIFO to clear if bS0_AUTO_IF=1
 sbit S0_FREE       = SPI0_STAT^3;  // ReadOnly: SPI0 free status
 sbit S0_T_FIFO     = SPI0_STAT^2;  // ReadOnly: tx FIFO count for SPI0
 sbit S0_R_FIFO     = SPI0_STAT^0;  // ReadOnly: rx FIFO count for SPI0
sfr SPI0_DATA       = 0xF9;         // FIFO data port: reading for receiving, writing for transmittal
sfr SPI0_CTRL       = 0xFA;         // SPI 0 control
#define bS0_MISO_OE       0x80      // SPI0 MISO output enable
#define bS0_MOSI_OE       0x40      // SPI0 MOSI output enable
#define bS0_SCK_OE        0x20      // SPI0 SCK output enable
#define bS0_DATA_DIR      0x10      // SPI0 data direction: 0=out(master_write), 1=in(master_read)
#define bS0_MST_CLK       0x08      // SPI0 master clock mode: 0=mode 0 with default low, 1=mode 3 with default high
#define bS0_2_WIRE        0x04      // enable SPI0 two wire mode: 0=3 wire (SCK+MOSI+MISO), 1=2 wire (SCK+MISO)
#define bS0_CLR_ALL       0x02      // force clear FIFO and interrupt flag and count of SPI0
#define bS0_AUTO_IF       0x01      // enable FIFO accessing to auto clear S0_IF_BYTE interrupt flag
sfr SPI0_CK_SE      = 0xFB;         // clock divisor setting
//sfr SPI0_S_PRE      = 0xFB;         // preset value for SPI slave
#define SPI0_S_PRE        SPI0_CK_SE
sfr SPI0_SETUP      = 0xFC;         // SPI 0 setup
#define bS0_MODE_SLV      0x80      // SPI0 slave mode: 0=master, 1=slave
#define bS0_IE_FIFO_OV    0x40      // enable interrupt for slave mode FIFO overflow
#define bS0_IE_FIRST      0x20      // enable interrupt for first byte received for SPI0 slave mode
#define bS0_IE_BYTE       0x10      // enable interrupt for a byte received
#define bS0_BIT_ORDER     0x08      // SPI0 bit data order: 0=MSB first, 1=LSB first
#define bS0_SLV_SELT      0x02      // ReadOnly: SPI0 slave mode chip selected status: 0=unselected, 1=selected
#define bS0_SLV_PRELOAD   0x01      // ReadOnly: SPI0 slave mode data pre-loading status just after chip-selection

/*  I2C Slave Registers  */
sfr I2CS_STAT       = 0xA0;         // I2C slave status
 sbit I2CS_IF_STASTO = I2CS_STAT^7;  // START/STOP condition received interrupt flag for I2C slave, direct bit address clear or write 1 to clear
 sbit I2CS_IF_BYTE  = I2CS_STAT^6;  // byte transferred interrupt flag for I2C slave, direct bit address clear or write 1 to clear
 sbit I2CS_IF_ADDR  = I2CS_STAT^5;  // data address received interrupt flag for I2C slave, direct bit address clear or write 1 to clear
 sbit I2CS_IF_DEV_A = I2CS_STAT^4;  // device address received interrupt flag for I2C slave, direct bit address clear or write 1 to clear
#define bI2CS_IF_STASTO   0x80      // START/STOP condition received interrupt flag for I2C slave, write 1 to clear
#define bI2CS_IF_BYTE     0x40      // byte transferred interrupt flag for I2C slave, write 1 to clear
#define bI2CS_IF_ADDR     0x20      // data address received interrupt flag for I2C slave, write 1 to clear
#define bI2CS_IF_DEV_A    0x10      // device address received interrupt flag for I2C slave, write 1 to clear
#define MASK_I2CS_STAT    0x0F      // ReadOnly: bit mask of I2C slave status machine
// MASK_I2CS_STAT: I2C slave status machine
//   0000: free or receiving device address
//   0001: acknowledging device address received
//   0010: receiving data address
//   0011: acknowledging data address received
//   0100: receiving data byte
//   0101: acknowledging data byte received
//   0110: transmitting data byte
//   0111: checking acknowledge
//   1100: STOP condition, used to judge START/STOP if bI2CS_IF_STASTO=1
//   xxxx: error or unknown
sfr I2CS_CTRL       = 0xA4;         // I2CS control
#define bI2CS_IE_READ     0x80      // byte reading interrupt enable for I2C slave
#define bI2CS_IE_WRITE    0x40      // byte writing interrupt enable for I2C slave
#define bI2CS_IE_ADDR     0x20      // data address received interrupt enable for I2C slave
#define bI2CS_IE_DEV_A    0x10      // device address received interrupt enable for I2C slave, enable general address
#define bI2CS_IE_STASTO   0x08      // START/STOP condition received interrupt enable for I2C slave
#define bI2CS_SDA_IN      0x04      // ReadOnly: current SDA status after synchronization
#define bI2CS_EN          0x01      // I2C slave enable
sfr I2CS_DEV_A      = 0xA5;         // I2CS device address
#define MASK_I2CS_DEV_A   0xFE      // I2C slave device address: 00=general address, other=device address to match
#define bI2CS_DEV_A3H     0x0E      // ReadOnly: I2C requested device address for 4 bits device address mode
#define bI2CS_DA_4BIT     0x01      // I2C slave device address mode: 0=7 bits address, 1=4 bits address (ignore low 3 bits)
sfr I2CS_ADDR       = 0xA6;         // ReadOnly: I2CS requested data address
sfr I2CS_DATA       = 0xA7;         // I2CS data buffer

/*  ADC and touch-key Registers  */
sfr ADC_CTRL        = 0xF3;         // ADC/touch-key control and status
#define bTKEY_ACT         0x80      // ReadOnly: indicate touch-key running status (charge then ADC)
#define bADC_IF           0x20      // interrupt flag for ADC finished, write 1 to clear or write ADC_CHAN to clear or write TKEY_CTRL to clear
#define bADC_START        0x10      // set 1 to start ADC, auto cleared when ADC finished
#define bADC_EN           0x08      // control ADC power: 0=shut down ADC, 1=enable power for ADC
#define bADC_CLK1         0x02      // ADC clock frequency selection high bit
#define bADC_CLK0         0x01      // ADC clock frequency selection low bit
#define MASK_ADC_CLK      0x03      // bit mask of ADC clock frequency selection
// bADC_CLK1 & bADC_CLK0: ADC clock frequency selection
//   00: slowest clock 750KHz, 1024 Fosc cycles for each ADC
//   01: slower clock 1.5MHz, 512 Fosc cycles for each ADC
//   10: faster clock 3MHz, 256 Fosc cycles for each ADC
//   11: fastest clock 6MHz, 128 Fosc cycles for each ADC
sfr16 ADC_DAT       = 0xF4;         // ReadOnly: ADC data
sfr ADC_DAT_L       = 0xF4;         // ReadOnly: ADC data low byte
sfr ADC_DAT_H       = 0xF5;         // ReadOnly: ADC data high byte
sfr ADC_CHAN        = 0xF6;         // analog signal channel seletion
// ADC_CHAN[3:0]: ADC signal input channel selection if bADC_EN=1
//   0000: connect AIN0(P1.0)
//   0001: connect AIN1(P1.1)
//   0010: connect AIN2(P3.2)
//   0011: connect AIN3(RST)
//   0100: connect AIN4(P1.4)
//   0101: connect AIN5(P1.5)
//   0110: connect AIN6(P1.6)
//   0111: connect AIN7(P1.7)
//   1000: connect AIN8(P3.6)
//   1001: connect AIN9(P3.7)
//   1010: connect AIN10(P1.2)
//   1011: connect AIN11(P1.3)
//   1100: connect AIN12(VDD12*25%)
//   1101: connect AIN13(VREF)
sfr TKEY_CTRL       = 0xF2;         // WriteOnly: touch-key charging pulse width control (only low 7 bits valid), auto cleared

/*  Analog comparator/buffer and ISINK Registers  */
sfr ANA_CTRL        = 0xF1;         // comparator/buffer control and status
#define bCMP_CHAN_X       0x80      // comparator output outside enable: 0=disable, 1=output to P1.2
#define bCMP_OUT_ADC      0x40      // ADC sample input selection: 0=from ADC_CHAN selection, 1=from comparator/buffer output
#define bCMP_CHAN_N       0x20      // comparator input- channel selection: 0=connect comparator output for buffer, 1=from P1.3
#define bCMP_CHAN_P       0x10      // comparator input+ channel selection: 0=from ADC_CHAN selection, 1=from RST pin
#define bCMP_EN           0x08      // control comparator power: 0=shut down comparator, 1=enable power for comparator
#define bCMP_IE           0x04      // interrupt enable for comparator result changed
#define bCMP_IF           0x02      // interrupt flag for comparator result changed, write 1 to clear or auto cleared when MCU enter interrupt routine
sfr ISINK_DATA      = 0xAE;         // ISINK data high byte (bit8~bit1), Write@SafeMode
sfr ISINK_LSB       = 0xAF;         // WriteOnly: preset data bit0 for ISINK, ReadOnly: current ISINK data bit0

/*  USB/Host/Device Registers  */
sfr USB_PULL        = 0xB4;         // USB UDP/UDM pull resistance & current control
#define bUDM_PUE          0x80      // UDM 1K5 pullup resistance enable: 0=disable, 1=enable pullup
#define bUDM_PDE          0x40      // UDM 16K pulldown resistance enable: 0=disable, 1=enable pulldown
#define bUDM_PCS1         0x20      // UDM pull current selection high bit
#define bUDM_PCS0         0x10      // UDM pull current selection low bit
#define MASK_UDM_PCS      0x30      // bit mask of UDM pull current selection
#define bUDP_PUE          0x08      // UDP 1K5 pullup resistance enable: 0=disable, 1=enable pullup, insert diode for 5V if MASK_UDP_PCS=10
#define bUDP_PDE          0x04      // UDP 16K pulldown resistance enable: 0=disable, 1=enable pulldown
#define bUDP_PCS1         0x02      // UDP pull current selection high bit
#define bUDP_PCS0         0x01      // UDP pull current selection low bit
#define MASK_UDP_PCS      0x03      // bit mask of UDP pull current selection
// bUDP_PCS1/bUDM_PCS1 & bUDP_PCS0/bUDM_PCS0: UDP/UDM pull current selection
//   00: disable pull current
//   01: pulldown 2uA
//   10: pullup 10uA
//   11: pulldown 100uA
sfr UDEV_CTRL       = 0xD1;         // USB device physical port control
#define bUD_LOW_SPEED     0x04      // enable USB physical port low speed: 0=full speed, 1=low speed or swap DP/DM for full speed
#define bUD_GP_BIT        0x02      // general purpose bit
#define bUD_PORT_EN       0x01      // enable USB physical port I/O: 0=disable, 1=enable
//sfr UHOST_CTRL      = 0xD1;         // USB host physical port control
#define UHOST_CTRL        UDEV_CTRL
#define bUH_LOW_SPEED     0x04      // enable USB port low speed: 0=full speed, 1=low speed
#define bUH_BUS_RESET     0x02      // control USB bus reset: 0=normal, 1=force bus reset
#define bUH_PORT_EN       0x01      // enable USB port: 0=disable, 1=enable port, automatic disabled if USB device detached
sfr UEP1_CTRL       = 0xD2;         // endpoint 1 control
#define bUEP_R_TOG        0x80      // expected data toggle flag of USB endpoint X receiving (OUT): 0=DATA0, 1=DATA1
#define bUEP_T_TOG        0x40      // prepared data toggle flag of USB endpoint X transmittal (IN): 0=DATA0, 1=DATA1
#define bUEP_AUTO_TOG     0x10      // enable automatic toggle after successful transfer completion on endpoint 1/2/3: 0=manual toggle, 1=automatic toggle
#define bUEP_R_RES1       0x08      // handshake response type high bit for USB endpoint X receiving (OUT)
#define bUEP_R_RES0       0x04      // handshake response type low bit for USB endpoint X receiving (OUT)
#define MASK_UEP_R_RES    0x0C      // bit mask of handshake response type for USB endpoint X receiving (OUT)
#define UEP_R_RES_ACK     0x00
#define UEP_R_RES_TOUT    0x04
#define UEP_R_RES_NAK     0x08
#define UEP_R_RES_STALL   0x0C
// bUEP_R_RES1 & bUEP_R_RES0: handshake response type for USB endpoint X receiving (OUT)
//   00: ACK (ready)
//   01: no response, time out to host, for non-zero endpoint isochronous transactions
//   10: NAK (busy)
//   11: STALL (error)
#define bUEP_T_RES1       0x02      // handshake response type high bit for USB endpoint X transmittal (IN)
#define bUEP_T_RES0       0x01      // handshake response type low bit for USB endpoint X transmittal (IN)
#define MASK_UEP_T_RES    0x03      // bit mask of handshake response type for USB endpoint X transmittal (IN)
#define UEP_T_RES_ACK     0x00
#define UEP_T_RES_TOUT    0x01
#define UEP_T_RES_NAK     0x02
#define UEP_T_RES_STALL   0x03
// bUEP_T_RES1 & bUEP_T_RES0: handshake response type for USB endpoint X transmittal (IN)
//   00: DATA0 or DATA1 then expecting ACK (ready)
//   01: DATA0 or DATA1 then expecting no response, time out from host, for non-zero endpoint isochronous transactions
//   10: NAK (busy)
//   11: STALL (error)
sfr UEP1_T_LEN      = 0xD3;         // endpoint 1 transmittal length
sfr UEP2_CTRL       = 0xD4;         // endpoint 2 control
sfr UEP2_T_LEN      = 0xD5;         // endpoint 2 transmittal length
sfr UEP3_CTRL       = 0xD6;         // endpoint 3 control
sfr UEP3_T_LEN      = 0xD7;         // endpoint 3 transmittal length
sfr USB_INT_FG      = 0xD8;         // USB interrupt flag
 sbit U_IS_NAK      = USB_INT_FG^7; // ReadOnly: indicate current USB transfer is NAK received
 sbit U_TOG_OK      = USB_INT_FG^6; // ReadOnly: indicate current USB transfer toggle is OK
 sbit U_SETUP_ACT   = USB_INT_FG^5; // ReadOnly: indicate SETUP token & 8 bytes setup request received for USB device mode
 sbit UIF_FIFO_OV   = USB_INT_FG^4; // FIFO overflow interrupt flag for USB, direct bit address clear or write 1 to clear
 sbit UIF_HST_SOF   = USB_INT_FG^3; // host SOF timer interrupt flag for USB host, direct bit address clear or write 1 to clear
 sbit UIF_SUSPEND   = USB_INT_FG^2; // USB suspend or resume event interrupt flag, direct bit address clear or write 1 to clear
 sbit UIF_TRANSFER  = USB_INT_FG^1; // USB transfer completion interrupt flag, direct bit address clear or write 1 to clear
 sbit UIF_DETECT    = USB_INT_FG^0; // device detected event interrupt flag for USB host mode, direct bit address clear or write 1 to clear
 sbit UIF_BUS_RST   = USB_INT_FG^0; // bus reset event interrupt flag for USB device mode, direct bit address clear or write 1 to clear
sfr USB_INT_ST      = 0xD9;         // ReadOnly: USB interrupt status
#define bUIS_SETUP_ACT    0x80      // ReadOnly: indicate SETUP token & 8 bytes setup request received for USB device mode
#define bUIS_TOG_OK       0x40      // ReadOnly: indicate current USB transfer toggle is OK, keep last status during SETUP token
#define bUIS_TOKEN1       0x20      // ReadOnly: current token PID code high bit received for USB device mode, clear UIF_TRANSFER to set free
#define bUIS_TOKEN0       0x10      // ReadOnly: current token PID code low bit received for USB device mode, clear UIF_TRANSFER to set free
#define MASK_UIS_TOKEN    0x30      // ReadOnly: bit mask of current token PID code received for USB device mode
#define UIS_TOKEN_OUT     0x00
#define UIS_TOKEN_SOF     0x10
#define UIS_TOKEN_IN      0x20
#define UIS_TOKEN_FREE    0x30
// bUIS_TOKEN1 & bUIS_TOKEN0: current token PID code received for USB device mode, keep last status during SETUP token, clear UIF_TRANSFER ( UIF_TRANSFER from 1 to 0 ) to set free
//   00: OUT token PID received
//   01: SOF token PID received
//   10: IN token PID received
//   11: free
#define MASK_UIS_ENDP     0x0F      // ReadOnly: bit mask of current transfer endpoint number for USB device mode
#define MASK_UIS_H_RES    0x0F      // ReadOnly: bit mask of current transfer handshake response for USB host mode: 0000=no response, time out from device, others=handshake response PID received
sfr USB_MIS_ST      = 0xDA;         // ReadOnly: USB miscellaneous status
#define bUMS_SOF_PRES     0x80      // ReadOnly: indicate host SOF timer presage status
#define bUMS_SOF_ACT      0x40      // ReadOnly: indicate host SOF timer action status for USB host
#define bUMS_SIE_FREE     0x20      // ReadOnly: indicate USB SIE free status
#define bUMS_R_FIFO_RDY   0x10      // ReadOnly: indicate USB receiving FIFO ready status (not empty)
#define bUMS_BUS_RESET    0x08      // ReadOnly: indicate USB bus reset status
#define bUMS_SUSPEND      0x04      // ReadOnly: indicate USB suspend status
#define bUMS_DM_LEVEL     0x02      // ReadOnly: indicate UDM level saved at device attached to USB host
#define bUMS_DEV_ATTACH   0x01      // ReadOnly: indicate device attached status on USB host
sfr USB_RX_LEN      = 0xDB;         // ReadOnly: USB receiving length
sfr UEP0_CTRL       = 0xDC;         // endpoint 0 control
sfr UEP0_T_LEN      = 0xDD;         // endpoint 0 transmittal length
sfr USB_INT_EN      = 0xE1;         // USB interrupt enable
#define bUIE_DEV_SOF      0x80      // enable interrupt for SOF received for USB device mode
#define bUIE_DEV_NAK      0x40      // enable interrupt for NAK responded for USB device mode
#define bUIE_FIFO_OV      0x10      // enable interrupt for FIFO overflow
#define bUIE_HST_SOF      0x08      // enable interrupt for host SOF timer action for USB host mode
#define bUIE_SUSPEND      0x04      // enable interrupt for USB suspend or resume event
#define bUIE_TRANSFER     0x02      // enable interrupt for USB transfer completion
#define bUIE_DETECT       0x01      // enable interrupt for USB device detected event for USB host mode
#define bUIE_BUS_RST      0x01      // enable interrupt for USB bus reset event for USB device mode
sfr USB_CTRL        = 0xE2;         // USB base control
#define bUC_HOST_MODE     0x80      // enable USB host mode: 0=device mode, 1=host mode
#define bUC_LOW_SPEED     0x40      // enable USB low speed: 0=full speed, 1=low speed
#define bUC_DEV_SWAP      0x20      // enable swap DP/DM for full speed USB device if bUD_LOW_SPEED=1
#define bUC_DEV_EN        0x10      // USB device enable
#define bUC_SYS_CTRL1     0x20      // USB system control high bit
#define bUC_SYS_CTRL0     0x10      // USB system control low bit
#define MASK_UC_SYS_CTRL  0x30      // bit mask of USB system control
// bUC_HOST_MODE & bUC_SYS_CTRL1 & bUC_SYS_CTRL0: USB system control
//   0 00: disable USB device, enable GPIO
//   0 01: enable USB device, need bUDP_PUE=1@FS or bUDM_PUE=1@LS to enable pullup resistance
//   0 1x: enable full speed USB device and swap DP/DM if bUD_LOW_SPEED=1, need bUDM_PUE=1 to enable pullup resistance
//   1 00: enable USB host and normal status
//   1 01: enable USB host and force UDP/UDM output SE0 state
//   1 10: enable USB host and force UDP/UDM output J state
//   1 11: enable USB host and force UDP/UDM output resume or K state
#define bUC_INT_BUSY      0x08      // enable automatic responding busy for device mode or automatic pause for host mode during interrupt flag UIF_TRANSFER valid
#define bUC_RESET_SIE     0x04      // force reset USB SIE, need software clear
#define bUC_CLR_ALL       0x02      // force clear FIFO and interrupt flag and count of USB
sfr USB_DEV_AD      = 0xE3;         // USB device address, lower 7 bits for USB device address
#define bUDA_GP_BIT       0x80      // general purpose bit
#define MASK_USB_ADDR     0x7F      // bit mask for USB device address
//sfr16 UEP2_DMA      = 0xE4;         // endpoint 2 buffer start address, little-endian
sfr UEP2_DMA_L      = 0xE4;         // endpoint 2 buffer start address low byte
//sfr UEP2_DMA_H      = 0xE5;         // endpoint 2 buffer start address high byte
//sfr16 UEP3_DMA      = 0xE6;         // endpoint 3 buffer start address, little-endian
sfr UEP3_DMA_L      = 0xE6;         // endpoint 3 buffer start address low byte
//sfr UEP3_DMA_H      = 0xE7;         // endpoint 3 buffer start address high byte
sfr UEP4_1_MOD      = 0xEA;         // endpoint 4/1 mode
#define bUEP1_RX_EN       0x80      // enable USB endpoint 1 receiving (OUT)
#define bUEP1_TX_EN       0x40      // enable USB endpoint 1 transmittal (IN)
// bUEPn_RX_EN & bUEPn_TX_EN: USB endpoint 1/2/3 buffer mode, buffer start address is UEPn_DMA
//   0 0:  disable endpoint and disable buffer
//   1 0:  64 bytes buffer for receiving (OUT endpoint)
//   0 1:  64 bytes buffer for transmittal (IN endpoint)
//   1 1:  64 bytes buffer for receiving (OUT endpoint) + 64 bytes buffer for transmittal (IN endpoint), total=128bytes
sfr UEP2_3_MOD      = 0xEB;         // endpoint 2/3 mode
#define bUEP3_RX_EN       0x80      // enable USB endpoint 3 receiving (OUT)
#define bUEP3_TX_EN       0x40      // enable USB endpoint 3 transmittal (IN)
#define bUEP2_RX_EN       0x08      // enable USB endpoint 2 receiving (OUT)
#define bUEP2_TX_EN       0x04      // enable USB endpoint 2 transmittal (IN)
//sfr16 UEP0_DMA      = 0xEC;         // endpoint 0 buffer start address, little-endian, single 64 bytes buffer for endpoint 0 receiving & transmittal (OUT & IN endpoint)
sfr UEP0_DMA_L      = 0xEC;         // endpoint 0 buffer start address low byte
//sfr UEP0_DMA_H      = 0xED;         // endpoint 0 buffer start address high byte
//sfr16 UEP1_DMA      = 0xEE;         // endpoint 1 buffer start address, little-endian
sfr UEP1_DMA_L      = 0xEE;         // endpoint 1 buffer start address low byte
//sfr UEP1_DMA_H      = 0xEF;         // endpoint 1 buffer start address high byte
//sfr UH_SETUP        = 0xD2;         // host aux setup
#define UH_SETUP          UEP1_CTRL
#define bUH_PRE_PID_EN    0x80      // USB host PRE PID enable for low speed device via hub
#define bUH_SOF_EN        0x40      // USB host automatic SOF enable
//sfr UH_RX_CTRL      = 0xD4;         // host receiver endpoint control
#define UH_RX_CTRL        UEP2_CTRL
#define bUH_R_TOG         0x80      // expected data toggle flag of host receiving (IN): 0=DATA0, 1=DATA1
#define bUH_R_AUTO_TOG    0x10      // enable automatic toggle after successful transfer completion: 0=manual toggle, 1=automatic toggle
#define bUH_R_RES         0x04      // prepared handshake response type for host receiving (IN): 0=ACK (ready), 1=no response, time out to device, for isochronous transactions
//sfr UH_EP_PID       = 0xD5;         // host endpoint and token PID, lower 4 bits for endpoint number, upper 4 bits for token PID
#define UH_EP_PID         UEP2_T_LEN
#define MASK_UH_TOKEN     0xF0      // bit mask of token PID for USB host transfer
#define MASK_UH_ENDP      0x0F      // bit mask of endpoint number for USB host transfer
//sfr UH_TX_CTRL      = 0xD6;         // host transmittal endpoint control
#define UH_TX_CTRL        UEP3_CTRL
#define bUH_T_TOG         0x40      // prepared data toggle flag of host transmittal (SETUP/OUT): 0=DATA0, 1=DATA1
#define bUH_T_AUTO_TOG    0x10      // enable automatic toggle after successful transfer completion: 0=manual toggle, 1=automatic toggle
#define bUH_T_RES         0x01      // expected handshake response type for host transmittal (SETUP/OUT): 0=ACK (ready), 1=no response, time out from device, for isochronous transactions
//sfr UH_TX_LEN       = 0xD7;         // host transmittal endpoint transmittal length
#define UH_TX_LEN         UEP3_T_LEN
//sfr UH_EP_MOD       = 0xEB;         // host endpoint mode
#define UH_EP_MOD         UEP2_3_MOD
#define bUH_EP_TX_EN      0x40      // enable USB host OUT endpoint transmittal, buffer start address is UH_TX_DMA
#define bUH_EP_RX_EN      0x08      // enable USB host IN endpoint receiving, buffer start address is UH_RX_DMA
//sfr16 UH_RX_DMA     = 0xE4;         // host rx endpoint buffer start address, little-endian
//#define UH_RX_DMA         UEP2_DMA
//sfr UH_RX_DMA_L     = 0xE4;         // host rx endpoint buffer start address low byte
#define UH_RX_DMA_L       UEP2_DMA_L
//sfr UH_RX_DMA_H     = 0xE5;         // host rx endpoint buffer start address high byte
//#define UH_RX_DMA_H       UEP2_DMA_H
//sfr16 UH_TX_DMA     = 0xE6;         // host tx endpoint buffer start address, little-endian
//#define UH_TX_DMA         UEP3_DMA
//sfr UH_TX_DMA_L     = 0xE6;         // host tx endpoint buffer start address low byte
#define UH_TX_DMA_L       UEP3_DMA_L
//sfr UH_TX_DMA_H     = 0xE7;         // host tx endpoint buffer start address high byte
//#define UH_TX_DMA_H       UEP3_DMA_H

/*  USB PD / USB Type C Registers  */
sfr CC1_CTRL        = 0xC6;         // USB type C CC1 control
#define bCC_CE            0x80      // CC comparator enable, PD I/O enable if selected
#define bCC_LVO           0x40      // CC low voltage output enable: 0=normal VDD output, 1=1.12V low voltage output
#define bCC_CVS1          0x20      // CC comparator reference voltage selection high bit
#define bCC_CVS0          0x10      // CC comparator reference voltage selection low bit
#define MASK_CC_CVS       0x30      // bit mask for CC comparator reference voltage selection
// bCC_CVS1 & bCC_CVS0: USB CC comparator reference voltage selection
//   00=0.55V, 01=0.22V, 10=0.66V, 11=1.23V
#define bCC_PU1           0x08      // CC pullup current control high bit
#define bCC_PU0           0x04      // CC pullup current control low bit
#define MASK_CC_PU        0x0C      // bit mask for CC pullup current control
// bCC_PU1 & bCC_PU0: USB CC pullup current control
//   00: disable pullup current
//   01: pullup 330uA
//   10: pullup 180uA
//   11: pullup 80uA
#define bCC_PD            0x02      // CC 5K1 pulldown resistance enable: 0=disable, 1=enable pulldown
#define bCC_CMPO          0x01      // ReadOnly: CC comparator result: 0=CC lower than reference voltage, 1=CC higher than reference voltage
sfr CC2_CTRL        = 0xC7;         // USB type C CC2 control
sfr UPD_TIMER       = 0xBB;         // USB PD BMC timer
#define UPD_TMR_TX_48M    (80-1)    // timer value for USB PD BMC transmittal @Fsys=48MHz
#define UPD_TMR_RX_48M    (120-1)   // timer value for USB PD BMC receiving @Fsys=48MHz
#define UPD_TMR_TX_24M    (40-1)    // timer value for USB PD BMC transmittal @Fsys=24MHz
#define UPD_TMR_RX_24M    (60-1)    // timer value for USB PD BMC receiving @Fsys=24MHz
#define UPD_TMR_TX_12M    (20-1)    // timer value for USB PD BMC transmittal @Fsys=12MHz
#define UPD_TMR_RX_12M    (30-1)    // timer value for USB PD BMC receiving @Fsys=12MHz
sfr UPD_COUNT       = 0xBC;         // ReadOnly: USB PD data byte count
sfr UPD_T_LEN       = 0xBD;         // USB PD transmittal length
sfr UPD_T_SOP       = 0xBE;         // USB PD transmittal SOP config
#define bUPD_KC4_SYNC2    0x00      // select SYNC2 for 4th K-code of transmittal SOP
#define bUPD_KC4_SYNC3    0x40      // select SYNC3 for 4th K-code of transmittal SOP
#define bUPD_KC4_RST2     0x80      // select RST2 for 4th K-code of transmittal SOP
#define bUPD_KC3_SYNC1    0x00      // select SYNC1 for 3rd K-code of transmittal SOP
#define bUPD_KC3_SYNC3    0x10      // select SYNC3 for 3rd K-code of transmittal SOP
#define bUPD_KC3_RST1     0x20      // select RST1 for 3rd K-code of transmittal SOP
#define bUPD_KC2_SYNC1    0x00      // select SYNC1 for 2nd K-code of transmittal SOP
#define bUPD_KC2_SYNC3    0x04      // select SYNC3 for 2nd K-code of transmittal SOP
#define bUPD_KC2_RST1     0x08      // select RST1 for 2nd K-code of transmittal SOP
#define bUPD_KC1_SYNC1    0x00      // select SYNC1 for 1st K-code of transmittal SOP
#define bUPD_KC1_RST1     0x01      // select RST1 for 1st K-code of transmittal SOP
#define UPD_SOP0          ( bUPD_KC1_SYNC1 | bUPD_KC2_SYNC1 | bUPD_KC3_SYNC1 | bUPD_KC4_SYNC2 )  // Start of Packet Sequence
#define UPD_SOP1          ( bUPD_KC1_SYNC1 | bUPD_KC2_SYNC1 | bUPD_KC3_SYNC3 | bUPD_KC4_SYNC3 )  // Start of Packet Sequence Prime
#define UPD_SOP2          ( bUPD_KC1_SYNC1 | bUPD_KC2_SYNC3 | bUPD_KC3_SYNC1 | bUPD_KC4_SYNC3 )  // Start of Packet Sequence Double Prime
#define UPD_HARD_RESET    ( bUPD_KC1_RST1 | bUPD_KC2_RST1 | bUPD_KC3_RST1 | bUPD_KC4_RST2 )      // Hard Reset
#define UPD_CABLE_RESET   ( bUPD_KC1_RST1 | bUPD_KC2_SYNC1 | bUPD_KC3_RST1 | bUPD_KC4_SYNC3 )    // Cable Reset
sfr UPD_CRC32       = 0xBF;         // ReadOnly: USB PD CRC32 byte data review window, selected by MASK_PD_CRC_SEL
sfr UPD_INT_FG      = 0xC0;         // USB PD interrupt flag
 sbit PIF_TX_END    = UPD_INT_FG^7; // tx block end interrupt flag, direct bit address clear or write 1 to clear
 sbit PIF_RX_RST    = UPD_INT_FG^6; // rx reset interrupt flag, direct bit address clear or write 1 to clear
 sbit PIF_RX_ACT    = UPD_INT_FG^5; // rx block end interrupt flag, direct bit address clear or write 1 to clear
 sbit PIF_RX_BYTE   = UPD_INT_FG^4; // rx a byte or SOP interrupt flag, direct bit address clear or write 1 to clear
 sbit PIF_RX_BIT    = UPD_INT_FG^3; // rx a bit (if bPIE_RX_BYTE=0) or a symbol (5 bits) (if bPIE_RX_BYTE=1) interrupt flag, direct bit address clear or write 1 to clear
 sbit PD_START      = UPD_INT_FG^2; // set 1 to start PD tx (if bPD_TX_EN=1) or enable PD rx (if bPD_TX_EN=0), auto cleared when tx block end
 sbit PD_STAT1      = UPD_INT_FG^1; // ReadOnly: indicate current PD status high bit
 sbit PD_STAT0      = UPD_INT_FG^0; // ReadOnly: indicate current PD status low bit
#define MASK_PD_STAT      0x03      // ReadOnly: bit mask for current PD status
#define PD_RX_SOP0        0x01      // SOP0 received for rx
#define PD_RX_SOP1_HRST   0x02      // SOP1 or Hard Reset received for rx
#define PD_RX_SOP2_CRST   0x03      // SOP2 or Cable Reset received for rx
// PD_STAT1 & PD_STAT0: indicate current PD status
//   00: free for rx, CRC32[7:0] now for tx
//   01: SOP0 received for rx, CRC32[15:8] now for tx
//   10: SOP1 or Hard Reset received for rx, CRC32[23:16] now for tx
//   11: SOP2 or Cable Reset received for rx, CRC32[31:24] now for tx
sfr UPD_INT_EN      = 0xC1;         // USB PD interrupt enable
#define bPIE_TX_END       0x80      // enable interrupt for tx block end
#define bPIE_RX_RST       0x40      // enable interrupt for rx reset
#define bPIE_RX_ACT       0x20      // enable interrupt for rx block end
#define bPIE_RX_BYTE      0x10      // enable interrupt for rx a byte or SOP
#define bPIE_RX_BIT       0x08      // enable interrupt for rx a bit or a symbol
#define bPIE_CC_IO        0x04      // enable interrupt for CC1 or CC2 comparator result changing if bCC_CE=1, or P1.0/P1.1 level changing
#define bPD_CRC32_SEL1    0x02      // ReadOnly: PD CRC32 byte data window review selection high bit
#define bPD_CRC32_SEL0    0x01      // ReadOnly: PD CRC32 byte data window review selection low bit
#define MASK_PD_CRC_SEL   0x03      // ReadOnly: bit mask for PD CRC32 byte data window review selection, auto increase after read UPD_CRC32, auto zero if bPD_CLR_ALL=1 or read/write UPD_INT_EN
// bPD_CRC32_SEL1 & bPD_CRC32_SEL0: PD CRC32 byte data window review selection
//   00=bit[7:0], 01=bit[15:8], 10=bit[23:16], 11=bit[31:24]
sfr UPD_CTRL        = 0xC2;         // USB PD control
#define bPD_NIBBLE_HI     0x80      // ReadOnly: indicate current nibble status of a PD byte: 0=low 4 bits, 1=high 4 bits
#define bBMC_TX_BIT_B     0x40      // ReadOnly: indicate current is tx BMC bit back part
#define bPD_WAKE_POLAR    0x20      // PD I/O wakeup polarity selection: 0=low action, 1=high action
#define bPD_RST_EN        0x10      // PD reset signaling action: 0=generate interrupt, 1=reset system, Hard Reset always reset, Cable Reset reset if bPIE_RX_RST=1
#define bPD_DMA_EN        0x08      // PD DMA enable: 0=disable DMA, 1=enable auto DMA
#define bPD_CC_SEL        0x04      // PD I/O pin selection: 0=use CC1, 1=use CC2
#define bPD_CLR_ALL       0x02      // force clear FIFO and interrupt flag and count of PD, set 0 to enable PD I/O
#define bPD_TX_EN         0x01      // PD tx enable: 0=disable tx, enable rx if bCC_CE=1 & selected by bPD_CC_SEL, 1=enable tx & selected by bPD_CC_SEL
sfr UPD_DATA        = 0xC3;         // USB PD data buffer
//sfr16 UPD_DMA       = 0xC4;         // USB PD buffer start address, little-endian
sfr UPD_DMA_L       = 0xC4;         // USB PD buffer start address low byte
//sfr UPD_DMA_H       = 0xC5;         // USB PD buffer start address high byte

/*----- XDATA: xRAM ------------------------------------------*/

#define XDATA_RAM_SIZE    0x0100    // size of expanded xRAM, xdata SRAM embedded chip

/*----- Reference Information --------------------------------------------*/
#define ID_CH543          0x43      // chip ID
#define ID_CH542          0x42      // chip ID
#define ID_CH541          0x41      // chip ID
#define ID_CH540          0x40      // chip ID

/* Interrupt routine address and interrupt number */
#define INT_ADDR_INT0     0x0003    // interrupt vector address for INT0
#define INT_ADDR_TMR0     0x000B    // interrupt vector address for timer0
#define INT_ADDR_INT1     0x0013    // interrupt vector address for INT1
#define INT_ADDR_TMR1     0x001B    // interrupt vector address for timer1
#define INT_ADDR_UART0    0x0023    // interrupt vector address for UART0
#define INT_ADDR_TMR2     0x002B    // interrupt vector address for timer2
#define INT_ADDR_SPI0     0x0033    // interrupt vector address for SPI0
#define INT_ADDR_I2C      0x003B    // interrupt vector address for I2C
#define INT_ADDR_USB      0x0043    // interrupt vector address for USB or P3.6/P3.7
#define INT_ADDR_ADC      0x004B    // interrupt vector address for ADC/TKEY
#define INT_ADDR_USBPD    0x0053    // interrupt vector address for USBPD or P1.0/P1.1
#define INT_ADDR_PWMX     0x005B    // interrupt vector address for PWMX/CMP
#define INT_ADDR_CMP      0x005B    // interrupt vector address for PWMX/CMP
#define INT_ADDR_GPIO     0x0063    // interrupt vector address for GPIO
#define INT_ADDR_WDOG     0x006B    // interrupt vector address for watch-dog timer
#define INT_NO_INT0       0         // interrupt number for INT0
#define INT_NO_TMR0       1         // interrupt number for timer0
#define INT_NO_INT1       2         // interrupt number for INT1
#define INT_NO_TMR1       3         // interrupt number for timer1
#define INT_NO_UART0      4         // interrupt number for UART0
#define INT_NO_TMR2       5         // interrupt number for timer2
#define INT_NO_SPI0       6         // interrupt number for SPI0
#define INT_NO_I2C        7         // interrupt number for I2C
#define INT_NO_USB        8         // interrupt number for USB or P3.6/P3.7
#define INT_NO_ADC        9         // interrupt number for ADC/TKEY
#define INT_NO_USBPD      10        // interrupt number for USBPD or P1.0/P1.1
#define INT_NO_PWMX       11        // interrupt number for PWMX/CMP
#define INT_NO_CMP        11        // interrupt number for PWMX/CMP
#define INT_NO_GPIO       12        // interrupt number for GPIO
#define INT_NO_WDOG       13        // interrupt number for watch-dog timer

/* Special Program Space */
#define DATA_FLASH_ADDR   0x3800    // start address of Data-Flash
#define BOOT_LOAD_ADDR    0x3900    // start address of boot loader program
#define ROM_CFG_ADDR      0x3FFE    // chip configuration information address
#define ROM_CHIP_ID_LO    0x10      // chip ID number low dword
#define ROM_CHIP_ID_HI    0x14      // chip ID number high dword

/*
New Instruction:   MOVX @DPTR1,A
Instruction Code:  0xA5
Instruction Cycle: 1
Instruction Operation:
   step-1. write ACC @DPTR1 into xdata SRAM embedded chip
   step-2. increase DPTR1
ASM example:
       INC  XBUS_AUX
       MOV  DPTR,#TARGET_ADDR ;DPTR1
       DEC  XBUS_AUX
       MOV  DPTR,#SOURCE_ADDR ;DPTR0
       MOV  R7,#xxH
 LOOP: MOVX A,@DPTR ;DPTR0
       INC  DPTR    ;DPTR0, if need
       DB   0A5H    ;MOVX @DPTR1,A & INC DPTR1
       DJNZ R7,LOOP
*/

#ifdef __cplusplus
}
#endif

#endif  // __CH543_H__

#ifndef __USB_DEF__
#define __USB_DEF__

#ifdef __cplusplus
extern "C" {
#endif

/*----- USB constant and structure define --------------------------------*/

/* USB PID */
#ifndef USB_PID_SETUP
#define USB_PID_NULL            0x00    /* reserved PID */
#define USB_PID_SOF             0x05
#define USB_PID_SETUP           0x0D
#define USB_PID_IN              0x09
#define USB_PID_OUT             0x01
#define USB_PID_ACK             0x02
#define USB_PID_NAK             0x0A
#define USB_PID_STALL           0x0E
#define USB_PID_DATA0           0x03
#define USB_PID_DATA1           0x0B
#define USB_PID_PRE             0x0C
#endif

/* USB standard device request code */
#ifndef USB_GET_DESCRIPTOR
#define USB_GET_STATUS          0x00
#define USB_CLEAR_FEATURE       0x01
#define USB_SET_FEATURE         0x03
#define USB_SET_ADDRESS         0x05
#define USB_GET_DESCRIPTOR      0x06
#define USB_SET_DESCRIPTOR      0x07
#define USB_GET_CONFIGURATION   0x08
#define USB_SET_CONFIGURATION   0x09
#define USB_GET_INTERFACE       0x0A
#define USB_SET_INTERFACE       0x0B
#define USB_SYNCH_FRAME         0x0C
#endif

/* USB hub class request code */
#ifndef HUB_GET_DESCRIPTOR
#define HUB_GET_STATUS          0x00
#define HUB_CLEAR_FEATURE       0x01
#define HUB_GET_STATE           0x02
#define HUB_SET_FEATURE         0x03
#define HUB_GET_DESCRIPTOR      0x06
#define HUB_SET_DESCRIPTOR      0x07
#endif

/* USB HID class request code */
#ifndef HID_GET_REPORT
#define HID_GET_REPORT          0x01
#define HID_GET_IDLE            0x02
#define HID_GET_PROTOCOL        0x03
#define HID_SET_REPORT          0x09
#define HID_SET_IDLE            0x0A
#define HID_SET_PROTOCOL        0x0B
#endif

/* Bit define for USB request type */
#ifndef USB_REQ_TYP_MASK
#define USB_REQ_TYP_IN          0x80            /* control IN, device to host */
#define USB_REQ_TYP_OUT         0x00            /* control OUT, host to device */
#define USB_REQ_TYP_READ        0x80            /* control read, device to host */
#define USB_REQ_TYP_WRITE       0x00            /* control write, host to device */
#define USB_REQ_TYP_MASK        0x60            /* bit mask of request type */
#define USB_REQ_TYP_STANDARD    0x00
#define USB_REQ_TYP_CLASS       0x20
#define USB_REQ_TYP_VENDOR      0x40
#define USB_REQ_TYP_RESERVED    0x60
#define USB_REQ_RECIP_MASK      0x1F            /* bit mask of request recipient */
#define USB_REQ_RECIP_DEVICE    0x00
#define USB_REQ_RECIP_INTERF    0x01
#define USB_REQ_RECIP_ENDP      0x02
#define USB_REQ_RECIP_OTHER     0x03
#endif

/* USB request type for hub class request */
#ifndef HUB_GET_HUB_DESCRIPTOR
#define HUB_CLEAR_HUB_FEATURE   0x20
#define HUB_CLEAR_PORT_FEATURE  0x23
#define HUB_GET_BUS_STATE       0xA3
#define HUB_GET_HUB_DESCRIPTOR  0xA0
#define HUB_GET_HUB_STATUS      0xA0
#define HUB_GET_PORT_STATUS     0xA3
#define HUB_SET_HUB_DESCRIPTOR  0x20
#define HUB_SET_HUB_FEATURE     0x20
#define HUB_SET_PORT_FEATURE    0x23
#endif

/* Hub class feature selectors */
#ifndef HUB_PORT_RESET
#define HUB_C_HUB_LOCAL_POWER   0
#define HUB_C_HUB_OVER_CURRENT  1
#define HUB_PORT_CONNECTION     0
#define HUB_PORT_ENABLE         1
#define HUB_PORT_SUSPEND        2
#define HUB_PORT_OVER_CURRENT   3
#define HUB_PORT_RESET          4
#define HUB_PORT_POWER          8
#define HUB_PORT_LOW_SPEED      9
#define HUB_C_PORT_CONNECTION   16
#define HUB_C_PORT_ENABLE       17
#define HUB_C_PORT_SUSPEND      18
#define HUB_C_PORT_OVER_CURRENT 19
#define HUB_C_PORT_RESET        20
#endif

/* USB descriptor type */
#ifndef USB_DESCR_TYP_DEVICE
#define USB_DESCR_TYP_DEVICE    0x01
#define USB_DESCR_TYP_CONFIG    0x02
#define USB_DESCR_TYP_STRING    0x03
#define USB_DESCR_TYP_INTERF    0x04
#define USB_DESCR_TYP_ENDP      0x05
#define USB_DESCR_TYP_QUALIF    0x06
#define USB_DESCR_TYP_SPEED     0x07
#define USB_DESCR_TYP_OTG       0x09
#define USB_DESCR_TYP_HID       0x21
#define USB_DESCR_TYP_REPORT    0x22
#define USB_DESCR_TYP_PHYSIC    0x23
#define USB_DESCR_TYP_CS_INTF   0x24
#define USB_DESCR_TYP_CS_ENDP   0x25
#define USB_DESCR_TYP_HUB       0x29
#endif

/* USB device class */
#ifndef USB_DEV_CLASS_HUB
#define USB_DEV_CLASS_RESERVED  0x00
#define USB_DEV_CLASS_AUDIO     0x01
#define USB_DEV_CLASS_COMMUNIC  0x02
#define USB_DEV_CLASS_HID       0x03
#define USB_DEV_CLASS_MONITOR   0x04
#define USB_DEV_CLASS_PHYSIC_IF 0x05
#define USB_DEV_CLASS_POWER     0x06
#define USB_DEV_CLASS_PRINTER   0x07
#define USB_DEV_CLASS_STORAGE   0x08
#define USB_DEV_CLASS_HUB       0x09
#define USB_DEV_CLASS_VEN_SPEC  0xFF
#endif

/* USB endpoint type and attributes */
#ifndef USB_ENDP_TYPE_MASK
#define USB_ENDP_DIR_MASK       0x80
#define USB_ENDP_ADDR_MASK      0x0F
#define USB_ENDP_TYPE_MASK      0x03
#define USB_ENDP_TYPE_CTRL      0x00
#define USB_ENDP_TYPE_ISOCH     0x01
#define USB_ENDP_TYPE_BULK      0x02
#define USB_ENDP_TYPE_INTER     0x03
#endif

#ifndef USB_DEVICE_ADDR
#define USB_DEVICE_ADDR         0x02    /* default USB device address */
#endif
#ifndef DEFAULT_ENDP0_SIZE
#define DEFAULT_ENDP0_SIZE      8       /* default maximum packet size for endpoint 0 */
#endif
#ifndef MAX_PACKET_SIZE
#define MAX_PACKET_SIZE         64      /* maximum packet size */
#endif
#ifndef USB_BO_CBW_SIZE
#define USB_BO_CBW_SIZE         0x1F    /* total size of CBW */
#define USB_BO_CSW_SIZE         0x0D    /* total size of CSW */
#endif
#ifndef USB_BO_CBW_SIG0
#define USB_BO_CBW_SIG0         0x55    /* flag 'USBC' for CBW */
#define USB_BO_CBW_SIG1         0x53
#define USB_BO_CBW_SIG2         0x42
#define USB_BO_CBW_SIG3         0x43
#define USB_BO_CSW_SIG0         0x55    /* flag 'USBS' for CSW */
#define USB_BO_CSW_SIG1         0x53
#define USB_BO_CSW_SIG2         0x42
#define USB_BO_CSW_SIG3         0x53
#endif

typedef struct _USB_SETUP_REQ {
    UINT8 bRequestType;
    UINT8 bRequest;
    UINT8 wValueL;
    UINT8 wValueH;
    UINT8 wIndexL;
    UINT8 wIndexH;
    UINT8 wLengthL;
    UINT8 wLengthH;
} USB_SETUP_REQ, *PUSB_SETUP_REQ;

typedef USB_SETUP_REQ xdata *PXUSB_SETUP_REQ;

typedef struct _USB_DEVICE_DESCR {
    UINT8 bLength;
    UINT8 bDescriptorType;
    UINT8 bcdUSBL;
    UINT8 bcdUSBH;
    UINT8 bDeviceClass;
    UINT8 bDeviceSubClass;
    UINT8 bDeviceProtocol;
    UINT8 bMaxPacketSize0;
    UINT8 idVendorL;
    UINT8 idVendorH;
    UINT8 idProductL;
    UINT8 idProductH;
    UINT8 bcdDeviceL;
    UINT8 bcdDeviceH;
    UINT8 iManufacturer;
    UINT8 iProduct;
    UINT8 iSerialNumber;
    UINT8 bNumConfigurations;
} USB_DEV_DESCR, *PUSB_DEV_DESCR;

typedef USB_DEV_DESCR xdata *PXUSB_DEV_DESCR;

typedef struct _USB_CONFIG_DESCR {
    UINT8 bLength;
    UINT8 bDescriptorType;
    UINT8 wTotalLengthL;
    UINT8 wTotalLengthH;
    UINT8 bNumInterfaces;
    UINT8 bConfigurationValue;
    UINT8 iConfiguration;
    UINT8 bmAttributes;
    UINT8 MaxPower;
} USB_CFG_DESCR, *PUSB_CFG_DESCR;

typedef USB_CFG_DESCR xdata *PXUSB_CFG_DESCR;

typedef struct _USB_INTERF_DESCR {
    UINT8 bLength;
    UINT8 bDescriptorType;
    UINT8 bInterfaceNumber;
    UINT8 bAlternateSetting;
    UINT8 bNumEndpoints;
    UINT8 bInterfaceClass;
    UINT8 bInterfaceSubClass;
    UINT8 bInterfaceProtocol;
    UINT8 iInterface;
} USB_ITF_DESCR, *PUSB_ITF_DESCR;

typedef USB_ITF_DESCR xdata *PXUSB_ITF_DESCR;

typedef struct _USB_ENDPOINT_DESCR {
    UINT8 bLength;
    UINT8 bDescriptorType;
    UINT8 bEndpointAddress;
    UINT8 bmAttributes;
    UINT8 wMaxPacketSizeL;
    UINT8 wMaxPacketSizeH;
    UINT8 bInterval;
} USB_ENDP_DESCR, *PUSB_ENDP_DESCR;

typedef USB_ENDP_DESCR xdata *PXUSB_ENDP_DESCR;

typedef struct _USB_CONFIG_DESCR_LONG {
    USB_CFG_DESCR   cfg_descr;
    USB_ITF_DESCR   itf_descr;
    USB_ENDP_DESCR  endp_descr[1];
} USB_CFG_DESCR_LONG, *PUSB_CFG_DESCR_LONG;

typedef USB_CFG_DESCR_LONG xdata *PXUSB_CFG_DESCR_LONG;

typedef struct _USB_HUB_DESCR {
    UINT8 bDescLength;
    UINT8 bDescriptorType;
    UINT8 bNbrPorts;
    UINT8 wHubCharacteristicsL;
    UINT8 wHubCharacteristicsH;
    UINT8 bPwrOn2PwrGood;
    UINT8 bHubContrCurrent;
    UINT8 DeviceRemovable;
    UINT8 PortPwrCtrlMask;
} USB_HUB_DESCR, *PUSB_HUB_DESCR;

typedef USB_HUB_DESCR xdata *PXUSB_HUB_DESCR;

typedef struct _USB_HID_DESCR {
    UINT8 bLength;
    UINT8 bDescriptorType;
    UINT8 bcdHIDL;
    UINT8 bcdHIDH;
    UINT8 bCountryCode;
    UINT8 bNumDescriptors;
    UINT8 bDescriptorTypeX;
    UINT8 wDescriptorLengthL;
    UINT8 wDescriptorLengthH;
} USB_HID_DESCR, *PUSB_HID_DESCR;

typedef USB_HID_DESCR xdata *PXUSB_HID_DESCR;

typedef struct _UDISK_BOC_CBW {         /* command of BulkOnly USB-FlashDisk */
    UINT8 mCBW_Sig0;
    UINT8 mCBW_Sig1;
    UINT8 mCBW_Sig2;
    UINT8 mCBW_Sig3;
    UINT8 mCBW_Tag0;
    UINT8 mCBW_Tag1;
    UINT8 mCBW_Tag2;
    UINT8 mCBW_Tag3;
    UINT8 mCBW_DataLen0;
    UINT8 mCBW_DataLen1;
    UINT8 mCBW_DataLen2;
    UINT8 mCBW_DataLen3;                /* uppest byte of data length, always is 0 */
    UINT8 mCBW_Flag;                    /* transfer direction and etc. */
    UINT8 mCBW_LUN;
    UINT8 mCBW_CB_Len;                  /* length of command block */
    UINT8 mCBW_CB_Buf[16];              /* command block buffer */
} UDISK_BOC_CBW, *PUDISK_BOC_CBW;

typedef UDISK_BOC_CBW xdata *PXUDISK_BOC_CBW;

typedef struct _UDISK_BOC_CSW {         /* status of BulkOnly USB-FlashDisk */
    UINT8 mCSW_Sig0;
    UINT8 mCSW_Sig1;
    UINT8 mCSW_Sig2;
    UINT8 mCSW_Sig3;
    UINT8 mCSW_Tag0;
    UINT8 mCSW_Tag1;
    UINT8 mCSW_Tag2;
    UINT8 mCSW_Tag3;
    UINT8 mCSW_Residue0;                /* return: remainder bytes */
    UINT8 mCSW_Residue1;
    UINT8 mCSW_Residue2;
    UINT8 mCSW_Residue3;                /* uppest byte of remainder length, always is 0 */
    UINT8 mCSW_Status;                  /* return: result status */
} UDISK_BOC_CSW, *PUDISK_BOC_CSW;

typedef UDISK_BOC_CSW xdata *PXUDISK_BOC_CSW;

#ifdef __cplusplus
}
#endif

#endif  // __USB_DEF__
