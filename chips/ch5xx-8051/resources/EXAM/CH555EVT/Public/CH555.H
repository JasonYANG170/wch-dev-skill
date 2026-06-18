/*--------------------------------------------------------------------------
CH555.H
Header file for CH555 microcontrollers.
****************************************
**  Copyright  (C)  W.ch  1999-2019   **
**  Web:              http://wch.cn   **
****************************************
--------------------------------------------------------------------------*/

// 3 blocks: __BASE_TYPE__, __CH555_H__, __USB_DEF__

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

#ifndef __CH555_H__
#define __CH555_H__

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
sfr A_INV           = 0xFD;         // ReadOnly: bit inversion of ACC (bit0<->bit7,bit1<->bit6,bit2<->bit5,bit3<->bit4, [7..0]->[0..7])
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
#define bXIR_XSFR         0x02      // force MOVX_@R0/@R1 only for xSFR area: 0=MOVX_@R0/@R1 for standard xdata area include xRAM&xSFR, 1=MOVX_@R0/@R1 for xSFR only
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
//    00 - software reset, by bSW_RESET=1 @(bBOOT_LOAD=0 or bWDOG_EN=1)
//    01 - power on reset
//    10 - watch-dog timer overflow reset
//    11 - external input manual reset by RST# pin
#define GF1               0x08      // general purpose flag bit 1
#define GF0               0x04      // general purpose flag bit 0
#define PD                0x02      // power-down enable bit, auto clear by wake-up hardware
sfr POWER_CFG       = 0xBA;         // power config, Write@SafeMode
#define bPWR_DN_MODE      0x80      // power down mode: 0=deep sleep and slow waking, 1=standby and fast waking
#define bCMP_RESULT       0x40      // ReadOnly: result of comparator: 0=lower then reference voltage, 1=higher then reference voltage
#define bLV_RST_OFF       0x20      // disable low voltage reset: 0=enable LVR, 1=disable LVR
#define bLDO_3V3_OFF      0x10      // disable 5V->3.3V LDO: 0=enable LDO for USB I/O, 1=disable 3.3V LDO, short V33 and VDD power
#define bLDO_CORE_VOL     0x08      // core voltage mode: 0=normal, 1=raised for performance (Fsys=48MHz)
#define MASK_ULLDO_VOL    0x07      // bit mask of ULLDO voltage selection
sfr CLOCK_CFG       = 0xB9;         // system clock config: lower 3 bits for system clock Fsys, Write@SafeMode
#define bOSC_EN_INT       0x80      // internal oscillator enable and original clock selection: 1=enable & select internal clock, 0=disable & select external clock
#define bOSC_EN_XT        0x40      // external oscillator enable, need quartz crystal or ceramic resonator between XI and XO pins
#define bWDOG_IF_TO       0x20      // ReadOnly: watch-dog timer overflow interrupt flag, cleared by reload watch-dog count or auto cleared when MCU enter interrupt routine
#define MASK_SYS_CK_SEL   0x07      // bit mask of system clock Fsys selection
/*
   Fxt  = 24MHz (12MHz~30MHz for non-USB application), from external oscillator @XI&XO
   Fosc = bOSC_EN_INT ? 24MHz : Fxt
   Fpll = Fosc * 4 => 96MHz (48MHz~120MHz for non-USB application)
   Fusb4x = Fpll / 2 => 48MHz (Fixed)
              MASK_SYS_CK_SEL[2] [1] [0]
   Fsys = Fpll/2   =  48MHz:  1   1   1
   Fsys = Fpll/3   =  32MHz:  1   1   0
   Fsys = Fpll/4   =  24MHz:  1   0   1
   Fsys = Fpll/6   =  16MHz:  1   0   0
   Fsys = Fpll/8   =  12MHz:  0   1   1
   Fsys = Fpll/32  =   3MHz:  0   1   0
   Fsys = Fpll/128 = 750KHz:  0   0   1
   Fsys = Fpll/512 =187.5KHz: 0   0   0
*/
sfr WAKE_CTRL       = 0xA9;         // wake-up control, Write@SafeMode
#define bWAK_RXD1_LO      0x40      // enable wake-up by RXD1 low level
#define bWAK_P1_5_LO      0x20      // enable wake-up by pin P1.5 low level
#define bWAK_P1_4_LO      0x10      // enable wake-up by pin P1.4 low level
#define bWAK_BY_USB       0x08      // enable wake-up by USB event
#define bWAK_P3_3_LO      0x04      // enable wake-up by pin P3.3 low level
#define bWAK_INT0_EDGE    0x02      // enable wake-up by INT0 edge
#define bWAK_RXD0_LO      0x01      // enable wake-up by RXD0 low level
sfr RESET_KEEP      = 0xFE;         // value keeper during reset
sfr WDOG_COUNT      = 0xFF;         // watch-dog count, count by clock frequency Fsys/131072

#define CMP_DCDC          XBYTE[0x21EB] // comparator and DC-DC control
#define pCMP_DCDC         PBYTE[0xEB]
#define bDCDC_ACT         0x80      // ReadOnly: DC-DC output action status: 0=free, 1=action
#define bDCDC_PIN         0x40      // DC-DC output pin and polarity selection: 0=DCO output bDCDC_ACT, 1=DCO output ! bDCDC_ACT, P6.4 output bDCDC_ACT
#define bDCDC_FREQ1       0x20      // DC-DC reference clock frequency selection high bit
#define bDCDC_FREQ0       0x10      // DC-DC reference clock frequency selection low bit
#define MASK_DCDC_FREQ    0x30      // bit mask of DC-DC reference clock frequency selection
// bDCDC_FREQ1 & bDCDC_FREQ0: DC-DC reference clock frequency if bCMP_VR_SEL2 & bCMP_VR_SEL1 & bCMP_VR_SEL0 != 000
//    00 - disable DC-DC
//    01 - reference 3MHz, max output frequency is 1MHz
//    10 - reference 1.5MHz, max output frequency is 500KHz
//    11 - reference 750KHz, max output frequency is 250KHz
// bDCDC_FREQ1 & bDCDC_FREQ0: bDCDC_ACT control if bCMP_VR_SEL2 & bCMP_VR_SEL1 & bCMP_VR_SEL0 == 000
//    00 - bDCDC_ACT=0
//    x1/1x - bDCDC_ACT=1
#define bCMP_PIN          0x08      // comparator input pin selection: 0=VDD, 1=analog pin input shared with ADC
#define bCMP_VR_SEL2      0x04      // comparator reference voltage selection high bit
#define bCMP_VR_SEL1      0x02      // comparator reference voltage selection middle bit
#define bCMP_VR_SEL0      0x01      // comparator reference voltage selection low bit
#define MASK_CMP_VREF     0x07      // bit mask of comparator reference voltage selection
// bCMP_VR_SEL2 & bCMP_VR_SEL1 & bCMP_VR_SEL0: comparator reference voltage selection
//    000 - disable comparator, bDCDC_ACT controlled by bDCDC_FREQ1|bDCDC_FREQ0
//    001 - reference 1.2V without voltage divider, bDCDC_ACT controlled by DC-DC
//    010 - voltage divider by resistance for compare 3.3V, bDCDC_ACT controlled by DC-DC
//    011 - voltage divider by resistance for compare 5.0V, bDCDC_ACT controlled by DC-DC
//    100 - voltage divider by resistance for compare 5.4V, bDCDC_ACT controlled by DC-DC
//    101 - voltage divider by resistance for compare 5.8V, bDCDC_ACT controlled by DC-DC
//    110 - voltage divider by resistance for compare 6.2V, bDCDC_ACT controlled by DC-DC
//    111 - voltage divider by resistance for compare 6.6V, bDCDC_ACT controlled by DC-DC

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
 sbit IE_PWM_I2C    = IE_EX^5;      // enable LED/I2C interrupt
 sbit IE_UART1      = IE_EX^4;      // enable UART1 interrupt
 sbit IE_ADC        = IE_EX^3;      // enable ADC interrupt
 sbit IE_USB        = IE_EX^1;      // enable USB interrupt
 sbit IE_SPI0       = IE_EX^0;      // enable SPI0 interrupt
sfr IP_EX           = 0xE9;         // extend interrupt priority
#define bIP_LEVEL         0x80      // ReadOnly: current interrupt nested level: 0=no interrupt or two levels, 1=one level
#define bIP_GPIO          0x40      // GPIO input interrupt priority level
#define bIP_PWM_I2C       0x20      // LED/I2C interrupt priority level
#define bIP_UART1         0x10      // UART1 interrupt priority level
#define bIP_ADC           0x08      // ADC interrupt priority level
#define bIP_USB           0x02      // USB interrupt priority level
#define bIP_SPI0          0x01      // SPI0 interrupt priority level
sfr GPIO_IE         = 0xB2;         // GPIO interrupt enable, Write@SafeMode
#define bIE_IO_EDGE       0x80      // enable GPIO edge interrupt: 0=low/high level, 1=falling/rising edge
#define bIE_RXD1_LO       0x40      // enable interrupt by RXD1 low level / falling edge
#define bIE_P1_5_LO       0x20      // enable interrupt by pin P1.5 low level / falling edge
#define bIE_P1_4_LO       0x10      // enable interrupt by pin P1.4 low level / falling edge
#define bIE_P0_3_LO       0x08      // enable interrupt by pin P0.3 low level / falling edge
#define bIE_P5_3X5X7      0x04      // enable interrupt by pin P5.3/P5.5/P5.7 level changing
#define bIE_P7_1_LO       0x02      // enable interrupt by pin P7.1 low level / falling edge if bOSC_EN_XT=0
#define bIE_CMP_RES_LO    0x02      // enable interrupt by bCMP_RESULT low / falling edge if MASK_CMP_VREF!=000
#define bIE_RXD0_LO       0x01      // enable interrupt by RXD0 low level / falling edge

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
sfr P0              = 0x80;         // port 0 input & output
 sbit P0_7          = P0^7;
 sbit P0_6          = P0^6;
 sbit P0_5          = P0^5;
 sbit P0_4          = P0^4;
 sbit P0_3          = P0^3;
 sbit P0_2          = P0^2;
 sbit P0_1          = P0^1;
 sbit P0_0          = P0^0;
 sbit SDA0          = P0^1;         // SDA input/output for I2CS
 sbit SCL0          = P0^0;         // SCL input for I2CS
 sbit TXD_          = P0^3;         // alternate pin for TXD of UART0
 sbit RXD_          = P0^2;         // alternate pin for RXD of UART0
 sbit AIN13         = P0^5;         // AIN13 for ADC
 sbit AIN12         = P0^4;         // AIN12 for ADC
 sbit AIN11         = P0^3;         // AIN11 for ADC
 sbit AIN10         = P0^2;         // AIN10 for ADC
 sbit AIN9          = P0^1;         // AIN9 for ADC
 sbit AIN8          = P0^0;         // AIN8 for ADC
sfr P0_MOD_OC       = 0xC4;         // port 0 output mode: 0=push-pull, 1=open-drain
sfr P0_DIR_PU       = 0xC5;         // port 0 direction for push-pull or pullup enable for open-drain
#define bSDA0             0x02      // SDA input/output for I2CS
#define bSCL0             0x01      // SCL input for I2CS
#define bTXD_             0x08      // alternate pin for TXD of UART0
#define bRXD_             0x04      // alternate pin for RXD of UART0
#define bAIN13            0x20      // AIN13 for ADC
#define bAIN12            0x10      // AIN12 for ADC
#define bAIN11            0x08      // AIN11 for ADC
#define bAIN10            0x04      // AIN10 for ADC
#define bAIN9             0x02      // AIN9 for ADC
#define bAIN8             0x01      // AIN8 for ADC
sfr P1              = 0x90;         // port 1 input & output
 sbit P1_7          = P1^7;
 sbit P1_6          = P1^6;
 sbit P1_5          = P1^5;
 sbit P1_4          = P1^4;
 sbit P1_3          = P1^3;
 sbit P1_2          = P1^2;
 sbit P1_1          = P1^1;
 sbit P1_0          = P1^0;
 sbit TXD1_         = P1^7;         // alternate pin for TXD1
 sbit RXD1_         = P1^6;         // alternate pin for RXD1
 sbit SCK           = P1^7;         // serial clock for SPI0
 sbit MISO          = P1^6;         // master serial data input or slave serial data output for SPI0
 sbit MOSI          = P1^5;         // master serial data output or slave serial data input for SPI0
 sbit SCS           = P1^4;         // slave chip-selection input for SPI0
 sbit INT0_         = P1^2;         // alternate pin for INT0
 sbit T2EX          = P1^1;         // external trigger input for timer2 reload & capture
 sbit CAP2          = P1^1;         // capture2 input for timer2
 sbit T2            = P1^0;         // external count input
 sbit CAP1          = P1^0;         // capture1 input for timer2
 sbit AIN7          = P1^7;         // AIN7 for ADC
 sbit AIN6          = P1^6;         // AIN6 for ADC
 sbit AIN5          = P1^5;         // AIN5 for ADC
 sbit AIN4          = P1^4;         // AIN4 for ADC
 sbit AIN3          = P1^3;         // AIN3 for ADC
 sbit AIN2          = P1^2;         // AIN2 for ADC
 sbit AIN1          = P1^1;         // AIN1 for ADC
 sbit AIN0          = P1^0;         // AIN0 for ADC
sfr P1_MOD_OC       = 0x92;         // port 1 output mode: 0=push-pull, 1=open-drain
sfr P1_DIR_PU       = 0x93;         // port 1 direction for push-pull or pullup enable for open-drain
// Pn_MOD_OC & Pn_DIR_PU: pin input & output configuration for Pn (n=0/1/2/3)
//   0 0:  float input only, without pullup resistance
//   0 1:  push-pull output, strong driving high level and low level
//   1 0:  open-drain output and input without pullup resistance
//   1 1:  quasi-bidirectional (standard 8051 mode), open-drain output and input with pullup resistance, just driving high level strongly for 2 clocks if turning output level from low to high
#define bTXD1_            0x80      // alternate pin for TXD1
#define bRXD1_            0x40      // alternate pin for RXD1
#define bSCK              0x80      // serial clock for SPI0
#define bMISO             0x40      // master serial data input or slave serial data output for SPI0
#define bMOSI             0x20      // master serial data output or slave serial data input for SPI0
#define bSCS              0x10      // slave chip-selection input for SPI0
#define bINT0_            0x04      // alternate pin for INT0
#define bT2EX             0x02      // external trigger input for timer2 reload & capture
#define bCAP2             bT2EX     // capture2 input for timer2
#define bT2               0x01      // external count input or clock output for timer2
#define bCAP1             bT2       // capture1 input for timer2
#define bAIN7             0x80      // AIN7 for ADC
#define bAIN6             0x40      // AIN6 for ADC
#define bAIN5             0x20      // AIN5 for ADC
#define bAIN4             0x10      // AIN4 for ADC
#define bAIN3             0x08      // AIN3 for ADC
#define bAIN2             0x04      // AIN2 for ADC
#define bAIN1             0x02      // AIN1 for ADC
#define bAIN0             0x01      // AIN0 for ADC
sfr P2              = 0xA0;         // port 2 input & output
 sbit P2_7          = P2^7;
 sbit P2_6          = P2^6;
 sbit P2_5          = P2^5;
 sbit P2_4          = P2^4;
 sbit P2_3          = P2^3;
 sbit P2_2          = P2^2;
 sbit P2_1          = P2^1;
 sbit P2_0          = P2^0;
 sbit TXD1          = P2^7;         // TXD output for UART1
 sbit RXD1          = P2^6;         // RXD input for UART1
 sbit T2EX_         = P2^5;         // alternate pin for T2EX
 sbit CAP2_         = P2^5;         // alternate pin for CAP2
 sbit T2_           = P2^4;         // alternate pin for T2
 sbit CAP1_         = P2^4;         // alternate pin for CAP1
sfr P2_MOD_OC       = 0x94;         // port 2 output mode: 0=push-pull, 1=open-drain
sfr P2_DIR_PU       = 0x95;         // port 2 direction for push-pull or pullup enable for open-drain
#define bTXD1             0x80      // TXD output for UART1
#define bRXD1             0x40      // RXD input for UART1
#define bT2EX_            0x20      // alternate pin for T2EX
#define bCAP2_            0x20      // alternate pin for CAP2
#define bT2_              0x10      // alternate pin for T2
#define bCAP1_            0x10      // alternate pin for CAP1
sfr P3              = 0xB0;         // port 3 input & output
 sbit P3_7          = P3^7;
 sbit P3_6          = P3^6;
 sbit P3_5          = P3^5;
 sbit P3_4          = P3^4;
 sbit P3_3          = P3^3;
 sbit P3_2          = P3^2;
 sbit P3_1          = P3^1;
 sbit P3_0          = P3^0;
 sbit SCK1          = P3^7;         // serial clock output for SPI1
 sbit MISO1         = P3^6;         // master serial data input for SPI1
 sbit MOSI1         = P3^5;         // master serial data output for SPI1
 sbit T1            = P3^5;         // external count input for timer1
 sbit T0            = P3^4;         // external count input for timer0
 sbit INT1          = P3^3;         // external interrupt 1 input
 sbit INT0          = P3^2;         // external interrupt 0 input
 sbit TXD           = P3^1;         // TXD output for UART0
 sbit RXD           = P3^0;         // RXD input for UART0
sfr P3_MOD_OC       = 0x96;         // port 3 output mode: 0=push-pull, 1=open-drain
sfr P3_DIR_PU       = 0x97;         // port 3 direction for push-pull or pullup enable for open-drain
#define bSCK1             0x80      // serial clock output for SPI1
#define bMISO1            0x40      // master serial data input for SPI1
#define bMOSI1            0x20      // master serial data output for SPI1
#define bT1               0x20      // external count input for timer1
#define bT0               0x10      // external count input for timer0
#define bINT1             0x08      // external interrupt 1 input
#define bINT0             0x04      // external interrupt 0 input
#define bTXD              0x02      // TXD output for UART0
#define bRXD              0x01      // RXD input for UART0
sfr P4              = 0xC0;         // port 4 input & output
 sbit P4_6          = P4^6;
 sbit P4_5          = P4^5;
 sbit P4_4          = P4^4;
 sbit P4_3          = P4^3;
 sbit P4_2          = P4^2;
 sbit P4_1          = P4^1;
 sbit P4_0          = P4^0;
sfr P4_MOD_OC       = 0xC2;         // port 4 output mode: 0=push-pull, 1=open-drain
sfr P4_DIR_PU       = 0xC3;         // port 4 direction for push-pull or pullup enable for open-drain
sfr P4_LED_KEY      = 0xC1;         // port 4 LED drive mode or keyboard input mode
// P4_MOD_OC & P4_DIR_PU & P4_LED_KEY: pin input & output configuration for P4
//   0 0 x:  float input only, without pullup resistance
//   0 1 0:  push-pull output, strong driving high level and low level
//   1 0 0:  open-drain output and input without pullup resistance
//   1 1 0:  quasi-bidirectional (standard 8051 mode), open-drain output and input with pullup resistance, just driving high level strongly for 2 clocks if turning output level from low to high
//   0 1 1:  push-pull output, strong driving high level and limited driving low level for LED
//   1 0 1:  open-drain output and input without pullup resistance, key input @high-resistance
//   1 1 1:  quasi-bidirectional (standard 8051 mode), open-drain output and input with pullup resistance, just driving high level strongly for 2 clocks if turning output level from low to high, key input @pullup
sfr P5_IN           = 0xAA;         // ReadOnly: port 5 input
sfr P5_OUT_PU       = 0xAB;         // port 5 output data for output direction or pullup enable for input direction
sfr P5_DIR          = 0xAC;         // port 5 direction: 0=input, 1=output
// P5_DIR P5_OUT_PU function for P5
//  0      1       input with pullup resistance (7K5)
//  1      0       push-pull output low
//  1      1       push-pull output high
//  0      0       float input without pullup resistance
sfr P6_IN           = 0xAD;         // ReadOnly: port 6 input
sfr P6_OUT_PU       = 0xAE;         // port 6 output data for output direction or pullup enable for input direction
sfr P6_DIR          = 0xAF;         // port 6 direction: 0=input, 1=output
// P6_dir P6_OUT_PU function for P6
//  0      0       float input without pullup resistance (weak pulldown 1000K) (default) @P6.0~7
//  0      0       input with pullup resistance (1K5) by USB bUX_D?_PU_EN=1 @P6.0/1
//  0      1       input with pullup resistance (7K5)
//  1      0       push-pull output low
//  1      1       push-pull output high
#define bDCO_             0x10      // alternate pin for DCO output if bDCDC_PIN=1, polarity controlled by P6_OUT_PU[4]
sfr P7              = 0xF1;         // port 7 input & output & buzzer frequency
#define bXO               0x20      // XO pin for external crystal oscillator
#define bXI               0x10      // XI pin for external crystal oscillator
#define bRST              0x20      // manual reset input, low action
#define bALE              0x02      // ALE/clock output if P5_DIR[2]=0 & bALE_CLK_EN=1
#define bP7_1_IN          0x20      // ReadOnly: P7.1 input
#define bP7_0_IN          0x10      // ReadOnly: P7.0 input
#define bP7_1_DIR         0x08      // P7.1 direction: 0=input, 1=output
#define bP7_0_DIR         0x04      // P7.0 direction: 0=input, 1=output
#define bP7_1_OUT_PU      0x02      // P7.1 output data for output direction or pullup enable for input direction
#define bP7_0_OUT_PU      0x01      // P7.0 output data for output direction or pullup enable for input direction
// bP7_n_DIR bP7_n_OUT_PU bOSC_EN_XT function for P7 (n=0/1)
//  0      0       0             float input without pullup resistance
//  0      1       0             input with pullup resistance (default)
//  1      0       0             push-pull output low
//  1      1       0             push-pull output high
//  x/0    x/0     1             XI/XO for external crystal oscillator
sfr XBUS_AUX        = 0xA2;         // xBUS auxiliary setting
#define bUART0_TX         0x80      // ReadOnly: indicate UART0 transmittal status
#define bUART0_RX         0x40      // ReadOnly: indicate UART0 receiving status
#define bSAFE_MOD_ACT     0x20      // ReadOnly: safe mode action status
#define bALE_CLK_EN       0x10      // enable ALE(P7.1) output clock
#define bALE_CLK_SEL      0x08      // ALE clock frequency select if bALE_CLK_EN=1: 0-1/12 Fsys, 1-1/4 Fsys
#define GF2               0x08      // general purpose flag bit 2 if bALE_CLK_EN=0
#define bDPTR_AUTO_INC    0x04      // enable DPTR auto increase if finished MOVX_@DPTR instruction
#define DPS               0x01      // dual DPTR selection: 0=DPTR0 selected, 1=DPTR1 selected

#define PIN_FUNC          XBYTE[0x21E9] // pin function selection
#define pPIN_FUNC         PBYTE[0xE9]
#define bUART1_PIN_X      0x20      // UART1 alternate pin enable: 0=RXD1/TXD1 on P2.6/P2.7, 1=RXD1/TXD1 on P1.6/P1.7
#define bUART0_PIN_X      0x10      // UART0 alternate pin enable: 0=RXD0/TXD0 on P3.0/P3.1, 1=RXD0/TXD0 on P0.2/P0.3
#define bIO_INT_ACT       0x08      // ReadOnly: GPIO interrupt request action status
#define bINT0_PIN_X       0x04      // INT0 alternate pin enable: 0=INT0 on P3.2, 1=INT0 on P1.2
#define bT2EX_PIN_X       0x02      // T2EX/CAP2 alternate pin enable: 0=T2EX/CAP2 on P1.1, 1=T2EX/CAP2 on P2.5
#define bT2_PIN_X         0x01      // T2/CAP1 alternate pin enable: 0=T2/CAP1 on P1.0, 1=T2/CAP1 on P2.4

#define PORT_CFG          XBYTE[0x21EA] // port interrupt and wakeup config
#define pPORT_CFG         PBYTE[0xEA]
#define bP4_IE_LEVEL      0x80      // P4 level changing interrupt/wakeup enable
#define bP2L_IE_LEVEL     0x40      // P2 low 4 bits level changing interrupt/wakeup enable
#define bP1L_IE_LEVEL     0x20      // P1 low 4 bits level changing interrupt/wakeup enable
#define bP0_IE_LEVEL      0x10      // P0 level changing interrupt/wakeup enable

#define ANA_PIN           XBYTE[0x21E8] // analog pin digital input control
#define pANA_PIN          PBYTE[0xE8]
#define bP70_P71_DI_DIS   0x80      // control P7.0/P7.1 digital input: 0=enable digital input, 1=disable digital input
#define bAIN12_13_DI_DIS  0x40      // control AIN12/AIN13 digital input: 0=enable digital input, 1=disable digital input
#define bAIN10_11_DI_DIS  0x20      // control AIN10/AIN11 digital input: 0=enable digital input, 1=disable digital input
#define bAIN8_9_DI_DIS    0x10      // control AIN8/AIN9 digital input: 0=enable digital input, 1=disable digital input
#define bAIN6_7_DI_DIS    0x08      // control AIN6/AIN7 digital input: 0=enable digital input, 1=disable digital input
#define bAIN4_5_DI_DIS    0x04      // control AIN4/AIN5 digital input: 0=enable digital input, 1=disable digital input
#define bAIN2_3_DI_DIS    0x02      // control AIN2/AIN3 digital input: 0=enable digital input, 1=disable digital input
#define bAIN0_1_DI_DIS    0x01      // control AIN0/AIN1 digital input: 0=enable digital input, 1=disable digital input

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

/*  SPI0/Master0/Slave Registers  */
sfr SPI0_STAT       = 0xF8;         // SPI0 status
 sbit S0_FST_ACT    = SPI0_STAT^7;  // ReadOnly: indicate first byte received status for SPI0
 sbit S0_IF_OV      = SPI0_STAT^6;  // interrupt flag for slave mode FIFO overflow, direct bit address clear or write 1 to clear
 sbit S0_IF_FIRST   = SPI0_STAT^5;  // interrupt flag for first byte received, direct bit address clear or write 1 to clear
 sbit S0_IF_BYTE    = SPI0_STAT^4;  // interrupt flag for a byte data exchanged, direct bit address clear or write 1 to clear or accessing FIFO to clear if bS0_AUTO_IF=1
 sbit S0_FREE       = SPI0_STAT^3;  // ReadOnly: SPI0 free status
 sbit S0_T_FIFO     = SPI0_STAT^2;  // ReadOnly: tx FIFO count for SPI0
 sbit S0_R_FIFO     = SPI0_STAT^0;  // ReadOnly: rx FIFO count for SPI0
sfr SPI0_DATA       = 0xF9;         // FIFO data port: reading for receiving, writing for transmittal
sfr SPI0_CTRL       = 0xFA;         // SPI0 control
#define bS0_MISO_OE       0x80      // SPI0 MISO output enable
#define bS0_MOSI_OE       0x40      // SPI0 MOSI output enable
#define bS0_SCK_OE        0x20      // SPI0 SCK output enable
#define bS0_DATA_DIR      0x10      // SPI0 data direction: 0=out(master_write), 1=in(master_read)
#define bS0_MST_CLK       0x08      // SPI0 master clock mode: 0=mode 0 with default low, 1=mode 3 with default high
#define bS0_2_WIRE        0x04      // enable SPI0 two wire mode: 0=3 wire (SCK+MOSI+MISO), 1=2 wire (SCK+MISO)
#define bS0_CLR_ALL       0x02      // force clear FIFO and count of SPI0
#define bS0_AUTO_IF       0x01      // enable FIFO accessing to auto clear S0_IF_BYTE interrupt flag
sfr SPI0_CK_SE      = 0xFB;         // SPI0 clock divisor setting
//sfr SPI0_S_PRE      = 0xFB;         // preset value for SPI slave
#define SPI0_S_PRE        SPI0_CK_SE
sfr SPI0_SETUP      = 0xFC;         // SPI0 setup
#define bS0_MODE_SLV      0x80      // SPI0 slave mode: 0=master, 1=slave
#define bS0_IE_FIFO_OV    0x40      // enable interrupt for slave mode FIFO overflow
#define bS0_IE_FIRST      0x20      // enable interrupt for first byte received for SPI0 slave mode
#define bS0_IE_BYTE       0x10      // enable interrupt for a byte received
#define bS0_BIT_ORDER     0x08      // SPI0 bit data order: 0=MSB first, 1=LSB first
#define bS0_SLV_SELT      0x02      // ReadOnly: SPI0 slave mode chip selected status: 0=unselected, 1=selected
#define bS0_SLV_PRELOAD   0x01      // ReadOnly: SPI0 slave mode data pre-loading status just after chip-selection

/*  UART1 Registers  */
sfr SCON1           = 0xBC;         // UART1 control (serial port control)
#define bU1SM0            0x80      // UART1 mode, selection data bit: 0=8 bits data, 1=9 bits data
#define bU1U0X2           0x40      // UART1/UART0 clock double frequency mode: 0=Fsys, 1=2*Fsys
#define bU1SMOD           0x20      // UART1 2X baud rate selection: 0=slow(Fsys/32/(256-SBAUD1)), 1=fast(Fsys/16/(256-SBAUD1))
#define bU1REN            0x10      // enable UART1 receiving
#define bU1TB8            0x08      // the 9th transmitted data bit in 9 bits data mode
#define bU1RB8            0x04      // 9th data bit received in 9 bits data mode, or stop bit received for 8 bits data mode
#define bU1TIS            0x02      // WriteOnly: write 1 to preset transmit interrupt flag
#define bU1RIS            0x01      // WriteOnly: write 1 to preset receive interrupt flag
sfr SBUF1           = 0xBD;         // UART1 data buffer: reading for receiving, writing for transmittal
sfr SBAUD1          = 0xBE;         // UART1 baud rate setting
sfr SIF1            = 0xBF;         // UART1 interrupt flag
#define bU1TI             0x02      // transmit interrupt flag, set by hardware after completion of a serial transmittal, need software write 1 to clear
#define bU1RI             0x01      // receive interrupt flag, set by hardware after completion of a serial receiving, need software write 1 to clear

/*  SPI1/Master1 Registers  */
sfr SPI1_STAT       = 0xB4;         // SPI1 status
#define bS1_IF_BYTE       0x10      // interrupt flag for a byte data exchanged, write 1 to clear or accessing FIFO to clear if bS1_AUTO_IF=1
#define bS1_FREE          0x08      // ReadOnly: SPI1 free status
sfr SPI1_DATA       = 0xB5;         // data port: reading for receiving, writing for transmittal
sfr SPI1_CTRL       = 0xB6;         // SPI1 control
#define bS1_MISO_OE       0x80      // SPI1 MISO output enable
#define bS1_SCK_OE        0x20      // SPI1 SCK output enable, MOSI output enable if bS1_2_WIRE=0
#define bS1_DATA_DIR      0x10      // SPI1 data direction: 0=out(master_write), 1=in(master_read)
#define bS1_MST_CLK       0x08      // SPI1 master clock mode: 0=mode 0 with default low, 1=mode 3 with default high
#define bS1_2_WIRE        0x04      // enable SPI1 two wire mode: 0=3 wire (SCK+MOSI+MISO), 1=2 wire (SCK+MISO)
#define bS1_CLR_ALL       0x02      // force clear FIFO and count of SPI1
#define bS1_AUTO_IF       0x01      // enable FIFO accessing to auto clear bS1_IF_BYTE interrupt flag
sfr SPI1_CK_SE      = 0xB7;         // SPI1 clock divisor setting

/*  ADC Registers  */
sfr ADC_CTRL        = 0xF3;         // ADC control and status
#define bADC_IF           0x20      // interrupt flag for ADC finished, write 1 to clear or write ADC_CHAN to clear
#define bADC_START        0x10      // set 1 to start ADC, auto cleared when ADC finished
#define bADC_EN           0x08      // control ADC power: 0=shut down ADC, 1=enable power for ADC
#define bADC_CLK1         0x02      // ADC clock frequency selection high bit
#define bADC_CLK0         0x01      // ADC clock frequency selection low bit
#define MASK_ADC_CLK      0x03      // bit mask of ADC clock frequency selection
// bADC_CLK1 & bADC_CLK0: ADC clock frequency selection
//   00: slowest clock 750KHz, 512 Fosc cycles for each ADC
//   01: slower clock 1.5MHz, 256 Fosc cycles for each ADC
//   10: faster clock 3MHz, 128 Fosc cycles for each ADC
//   11: fastest clock 6MHz, 64 Fosc cycles for each ADC
sfr16 ADC_DAT       = 0xF4;         // ReadOnly: ADC data
sfr ADC_DAT_L       = 0xF4;         // ReadOnly: ADC data low byte
sfr ADC_DAT_H       = 0xF5;         // ReadOnly: ADC data high byte
sfr ADC_CHAN        = 0xF6;         // analog signal channel seletion
// ADC_CHAN[3:0]: ADC signal input channel selection if bADC_EN=1
//   0000: connect AIN0(P1.0)
//   0001: connect AIN1(P1.1)
//   0010: connect AIN2(P1.2)
//   0011: connect AIN3(P1.3)
//   0100: connect AIN4(P1.4)
//   0101: connect AIN5(P1.5)
//   0110: connect AIN6(P1.6)
//   0111: connect AIN7(P1.7)
//   1000: connect AIN8(P0.0)
//   1001: connect AIN9(P0.1)
//   1010: connect AIN10(P0.2)
//   1011: connect AIN11(P0.3)
//   1100: connect AIN12(P0.4)
//   1101: connect AIN13(P0.5)
//   1110: connect V33
//   1111: connect 1.8V reference voltage

/*  RGB LED Registers  */
sfr LED_COMMON      = 0xA6;         // LED common drive pin selection
// LED_COMMON[4:0]: LED common drive pin selection
//   01110: act COM14(P7.0)
//   01111: act COM15(P7.1)
//   10000: act COM16(P0.0)
//   10001: act COM17(P0.1)
//   10010: act COM18(P0.2)
//   10011: act COM19(P0.3)
//   10100: act COM20(P0.4)
//   10101: act COM21(P0.5)
//   10110: act COM22(P0.6)
//   10111: act COM23(P0.7)
//   11000: act COM24(P3.0)
//   11001: act COM25(P3.1)
//   11010: act COM26(P3.2)
//   11011: act COM27(P3.3)
//   11100: act COM28(P3.4)
//   11101: act COM29(P3.5)
//   11110: act COM30(P3.6)
//   11111: act COM31(P3.7)
//   xxxxx: all inaction (default)
sfr LED_PWM_OE      = 0xA7;         // LED RGB PWM output pin enable
#define bLED_PWM7_OE      0x80      // LED PWM7 group output enable
#define bLED_PWM6_OE      0x40      // LED PWM6 group output enable
#define bLED_PWM5_OE      0x20      // LED PWM5 group output enable
#define bLED_PWM4_OE      0x10      // LED PWM4 group output enable
#define bLED_PWM3_OE      0x08      // LED PWM3 group output enable
#define bLED_PWM2_OE      0x04      // LED PWM2 group output enable
#define bLED_PWM1_OE      0x02      // LED PWM1 group output enable
#define bLED_PWM0_OE      0x01      // LED PWM0 & global group output enable, auto clear at cycle end if bLED_PWM_INHIB=1
sfr16 LED_DMA       = 0xC6;         // LED buffer current address, little-endian
sfr LED_DMA_L       = 0xC6;         // LED buffer current address low byte
sfr LED_DMA_H       = 0xC7;         // LED buffer current address high byte
sfr LED_STATUS      = 0xF7;         // LED status
#define bLED_IF           0x80      // interrupt flag for LED inhibition, write 1 to clear or write LED_COMMON to clear, manual set by bLED_IF_SET
#define bLED_IF_SET       0x40      // WriteOnly: write 1 to force set bLED_IF
#define bLED_INHIB        0x10      // ReadOnly: LED inhibition status: 0=scanning, 1=inhibition for load new data and switch common
#define MASK_LED_INTEN    0x0F      // ReadOnly: bit mask of LED intenisy count high 4 bits

#define XSFR_LED_BASE     0x21D0    // RGB PWM LED register base address

#define LED_CTRL          XBYTE[0x21D1] // LED control
#define pLED_CTRL         PBYTE[0xD1]
#define bLED_IE_INHIB     0x80      // interrupt enable for LED inhibition
#define bLED_BLUE_EN      0x40      // blue color PWM group output enable
#define bLED_GREEN_EN     0x20      // green color PWM group output enable
#define bLED_RED_EN       0x10      // red color PWM group output enable
#define bLED_COM_AHEAD    0x08      // LED common output ahead mode: 0=normal, 1=ahead for MOS gate charging
#define bLED_PWM_INHIB    0x04      // LED PWM inhibition mode: 0=keep output, 1=auto inhibit at cycle end
#define bLED_EN           0x01      // LED enable

#define LED_CYCLE         XBYTE[0x21D2] // LED cycle config
#define pLED_CYCLE        PBYTE[0xD2]
#define bLED_COLOR_CYC    0x40      // LED color PWM cycle: 0=256 intenisy PWM cycles, 1=128 intenisy PWM cycles
#define bLED_INTEN_CYC1   0x20      // LED intenisy PWM cycle selection high bit
#define bLED_INTEN_CYC0   0x10      // LED intenisy PWM cycle selection low bit
#define MASK_LED_INT_CYC  0x30      // bit mask of LED intenisy PWM cycle selection
// bLED_INTEN_CYC1 & bLED_INTEN_CYC0: LED intenisy PWM cycle
//   00: 256 LED clock cycles
//   01: 128 LED clock cycles
//   1x: 64 LED clock cycles
#define bLED_CLK_FREQ1    0x02      // LED clock frequency selection high bit
#define bLED_CLK_FREQ0    0x01      // LED clock frequency selection low bit
#define MASK_LED_CLK_FREQ 0x03      // bit mask of LED clock frequency selection
// bLED_CLK_FREQ1 & bLED_CLK_FREQ0: LED clock frequency for intenisy PWM
//   00: Fsys
//   01: Fsys/2
//   10: Fsys/3
//   11: Fsys/4

#define LED_FRAME         XBYTE[0x21D3] // LED frame config
#define pLED_FRAME        PBYTE[0xD3]
#define bLED_INH_TMR2     0x40      // LED inhibition timer selection high bit
#define bLED_INH_TMR1     0x20      // LED inhibition timer selection middle bit
#define bLED_INH_TMR0     0x10      // LED inhibition timer selection low bit
#define MASK_LED_INH_TMR  0x70      // bit mask of LED inhibition timer selection
// bLED_INH_TMR2 & bLED_INH_TMR1 & bLED_INH_TMR0: LED inhibition timer (unit: intenisy PWM cycle)
//   000~011: 1~4 intenisy PWM cycles
//   100: 6 intenisy PWM cycles
//   101: 8 intenisy PWM cycles
//   110: 10 intenisy PWM cycles
//   111: 12 intenisy PWM cycles
#define bLED_PWM_REPT2    0x04      // LED PWM repeat times selection high bit
#define bLED_PWM_REPT1    0x02      // LED PWM repeat times selection middle bit
#define bLED_PWM_REPT0    0x01      // LED PWM repeat times selection low bit
#define MASK_LED_PWM_REPT 0x07      // bit mask of LED PWM repeat times selection
// bLED_PWM_REPT2 & bLED_PWM_REPT1 & bLED_PWM_REPT0: LED PWM repeat times
//   000~111: same PWM data repeat 1~8 times

#define LED_INT_ADJ       XBYTE[0x21D8] // LED intensity adjustment
#define LED_RED_ADJ       XBYTE[0x21D9] // LED red color adjustment
#define LED_GRE_ADJ       XBYTE[0x21DA] // LED green color adjustment
#define LED_BLU_ADJ       XBYTE[0x21DB] // LED blue color adjustment
#define pLED_INT_ADJ      PBYTE[0xD8]
#define pLED_RED_ADJ      PBYTE[0xD9]
#define pLED_GRE_ADJ      PBYTE[0xDA]
#define pLED_BLU_ADJ      PBYTE[0xDB]

#define LED_FRA_STA       XBYTE[0x21DC] // ReadOnly: LED frame status
#define pLED_FRA_STA      PBYTE[0xDC]
#define MASK_LED_REPEAT   0x70      // ReadOnly: bit mask of LED PWM repeat times
#define MASK_LED_INHIB    0x0F      // ReadOnly: bit mask of LED inhibition count

#define LED_COL_CNT       XBYTE[0x21DD] // ReadOnly: LED color PWM count
#define pLED_COL_CNT      PBYTE[0xDD]

/*  I2C Registers  */
sfr I2CX_INT        = 0xB3;         // I2C slave and LED interrupt request
#define bI2CS_INT_ACT     0x20      // ReadOnly: I2C slave interrupt request: 0=free, 1=action
#define bLED_INT_ACT      0x02      // ReadOnly: RGB LED interrupt request: 0=free, 1=action
sfr I2CS_INT_ST     = 0xBB;         // I2C slave interrupt status mapped from I2CS_STAT

#define XSFR_I2CS_BASE    0x2230    // I2CS register base address

#define I2CS_CTRL         XBYTE[0x2232] // I2CS control
#define pI2CS_CTRL        PBYTE[0x32]
#define bI2CS_IE_TRAN     0x80      // byte transmitted interrupt enable for I2C slave
#define bI2CS_IE_RECV     0x40      // byte received interrupt enable for I2C slave
#define bI2CS_IE_ADDR     0x20      // data address received interrupt enable for I2C slave
#define bI2CS_IE_DEV_A    0x10      // device address received interrupt enable for I2C slave, enable general address
#define bI2CS_IE_STASTO   0x08      // START/STOP condition received interrupt enable for I2C slave
#define bI2CS_SDA_IN      0x04      // ReadOnly: current SDA status after synchronization
#define bI2CS_DMA_EN      0x02      // DMA enable for I2C slave
#define bI2CS_EN          0x01      // I2C slave enable

#define I2CS_DEV_A        XBYTE[0x2233] // I2CS device address
#define pI2CS_DEV_A       PBYTE[0x33]
#define MASK_I2CS_DEV_A   0xFE      // I2C slave device address: 00=general address, other=device address to match
#define bI2CS_DA_4BIT     0x01      // I2C slave device address mode: 0=7 bits address, 1=4 bits address (ignore low 3 bits)

#define I2CS_ADDR         XBYTE[0x2235] // ReadOnly: I2CS data address
#define I2CS_DATA         XBYTE[0x2236] // I2CS data buffer
#define pI2CS_ADDR        PBYTE[0x35]
#define pI2CS_DATA        PBYTE[0x36]

#define I2CS_STAT         XBYTE[0x223A] // I2CS status
#define pI2CS_STAT        PBYTE[0x3A]
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

#define I2CS_DMA_H        XBYTE[0x2138] // I2CS buffer start address high byte, big-endian
#define I2CS_DMA_L        XBYTE[0x2139] // I2CS buffer start address low byte, big-endian
#define MASK_I2CS_AH      0xE0      // ReadOnly: high 3 bits of data address @I2CS_DMA_H if bI2CS_DA_4BIT=1

/*  USB Device Registers  */
#define XSFR_USB_BASE     0x2200    // USB register base address
#define XSFR_USBD0_BASE   0x2200    // USBD0 register base address
#define XSFR_USBD1_BASE   0x2210    // USBD1 register base address
#define XSFR_USBD2_BASE   0x2220    // USBD2 register base address
#define XSFR_USBHB_BASE   0x2230    // USBHB register base address

#define D0_EP0RES         XBYTE[0x2200] // receiving/transmittal respond for endpoint 0, USB sub-device 0#
#define D0_EP1RES         XBYTE[0x2201] // receiving/transmittal respond for endpoint 1, USB sub-device 0#
#define D0_EP2RES         XBYTE[0x2202] // receiving/transmittal respond for endpoint 2, USB sub-device 0#
#define D0_EP3RES         XBYTE[0x2203] // receiving/transmittal respond for endpoint 3, USB sub-device 0#
#define D0_EP4RES         XBYTE[0x2204] // receiving/transmittal respond for endpoint 4, USB sub-device 0#
#define D0_EP5RES         XBYTE[0x2205] // receiving/transmittal respond for endpoint 5, USB sub-device 0#
#define D0_EP6RES         XBYTE[0x2206] // receiving/transmittal respond for endpoint 6, USB sub-device 0#
#define pD0_EP0RES        PBYTE[0x00]
#define pD0_EP1RES        PBYTE[0x01]
#define pD0_EP2RES        PBYTE[0x02]
#define pD0_EP3RES        PBYTE[0x03]
#define pD0_EP4RES        PBYTE[0x04]
#define pD0_EP5RES        PBYTE[0x05]
#define pD0_EP6RES        PBYTE[0x06]
#define D1_EP0RES         XBYTE[0x2210] // receiving/transmittal respond for endpoint 0, USB sub-device 1#
#define D1_EP1RES         XBYTE[0x2211] // receiving/transmittal respond for endpoint 1, USB sub-device 1#
#define D1_EP2RES         XBYTE[0x2212] // receiving/transmittal respond for endpoint 2, USB sub-device 1#
#define D1_EP3RES         XBYTE[0x2213] // receiving/transmittal respond for endpoint 3, USB sub-device 1#
#define D1_EP4RES         XBYTE[0x2214] // receiving/transmittal respond for endpoint 4, USB sub-device 1#
#define D1_EP5RES         XBYTE[0x2215] // receiving/transmittal respond for endpoint 5, USB sub-device 1#
#define D1_EP6RES         XBYTE[0x2216] // receiving/transmittal respond for endpoint 6, USB sub-device 1#
#define pD1_EP0RES        PBYTE[0x10]
#define pD1_EP1RES        PBYTE[0x11]
#define pD1_EP2RES        PBYTE[0x12]
#define pD1_EP3RES        PBYTE[0x13]
#define pD1_EP4RES        PBYTE[0x14]
#define pD1_EP5RES        PBYTE[0x15]
#define pD1_EP6RES        PBYTE[0x16]
#define D2_EP0RES         XBYTE[0x2220] // receiving/transmittal respond for endpoint 0, USB sub-device 2#
#define D2_EP1RES         XBYTE[0x2221] // receiving/transmittal respond for endpoint 1, USB sub-device 2#
#define D2_EP2RES         XBYTE[0x2222] // receiving/transmittal respond for endpoint 2, USB sub-device 2#
#define D2_EP3RES         XBYTE[0x2223] // receiving/transmittal respond for endpoint 3, USB sub-device 2#
#define D2_EP4RES         XBYTE[0x2224] // receiving/transmittal respond for endpoint 4, USB sub-device 2#
#define D2_EP5RES         XBYTE[0x2225] // receiving/transmittal respond for endpoint 5, USB sub-device 2#
#define D2_EP6RES         XBYTE[0x2226] // receiving/transmittal respond for endpoint 6, USB sub-device 2#
#define pD2_EP0RES        PBYTE[0x20]
#define pD2_EP1RES        PBYTE[0x21]
#define pD2_EP2RES        PBYTE[0x22]
#define pD2_EP3RES        PBYTE[0x23]
#define pD2_EP4RES        PBYTE[0x24]
#define pD2_EP5RES        PBYTE[0x25]
#define pD2_EP6RES        PBYTE[0x26]
#define HB_EP0RES         XBYTE[0x2230] // receiving/transmittal respond for endpoint 0, USB hub
#define HB_EP1RES         XBYTE[0x2231] // receiving/transmittal respond for endpoint 1, USB hub
#define HB_EP4RES         XBYTE[0x2234] // receiving/transmittal respond for endpoint 4, USB hub
#define pHB_EP0RES        PBYTE[0x30]
#define pHB_EP1RES        PBYTE[0x31]
#define pHB_EP4RES        PBYTE[0x34]

#define pDi_EP0RES(i)     PBYTE[i*0x10+0x0]
#define pDi_EP1RES(i)     PBYTE[i*0x10+0x1]
#define pDi_EP2RES(i)     PBYTE[i*0x10+0x2]
#define pDi_EP3RES(i)     PBYTE[i*0x10+0x3]
#define pDi_EP4RES(i)     PBYTE[i*0x10+0x4]
#define pDi_EP5RES(i)     PBYTE[i*0x10+0x5]
#define pDi_EP6RES(i)     PBYTE[i*0x10+0x6]

#define bUEP_R_TOG        0x80      // expected data toggle flag of USB endpoint 0 receiving (OUT): 0=DATA0, 1=DATA1
#define bUEP_T_TOG        0x40      // prepared data toggle flag of USB endpoint 0 transmittal (IN): 0=DATA0, 1=DATA1
#define bUEP_R_RES1       0x08      // handshake response type high bit for USB endpoint 0 receiving (OUT)
#define bUEP_R_RES0       0x04      // handshake response type low bit for USB endpoint 0 receiving (OUT)
#define MASK_UEP_R_RES    0x0C      // bit mask of handshake response type for USB endpoint 0 receiving (OUT)
#define UEP_R_RES_ACK     0x00
#define UEP_R_RES_TOUT    0x04
#define UEP_R_RES_NAK     0x08
#define UEP_R_RES_STALL   0x0C
// bUEP_R_RES1 & bUEP_R_RES0: handshake response type for USB endpoint 0 receiving (OUT)
//   00: ACK (ready)
//   01: no response, time out to host, for non-zero endpoint isochronous transactions
//   10: NAK (busy)
//   11: STALL (error)
#define bUEP_T_RES1       0x02      // handshake response type high bit for USB endpoint 0 transmittal (IN)
#define bUEP_T_RES0       0x01      // handshake response type low bit for USB endpoint 0 transmittal (IN)
#define MASK_UEP_T_RES    0x03      // bit mask of handshake response type for USB endpoint 0 transmittal (IN)
#define UEP_T_RES_ACK     0x00
#define UEP_T_RES_TOUT    0x01
#define UEP_T_RES_NAK     0x02
#define UEP_T_RES_STALL   0x03
// bUEP_T_RES1 & bUEP_T_RES0: handshake response type for USB endpoint 0 transmittal (IN)
//   00: DATA0 or DATA1 then expecting ACK (ready)
//   01: DATA0 or DATA1 then expecting no response, time out from host, for non-zero endpoint isochronous transactions
//   10: NAK (busy)
//   11: STALL (error)

#define bUEP_X_TOG        0x40      // prepared data toggle flag of USB endpoint receiving/transmittal (OUT/IN): 0=DATA0, 1=DATA1
#define bUEP_X_AUTO_TOG   0x10      // enable automatic toggle after successful transfer completion on endpoint 1/2/3/4/5/6: 0=manual toggle, 1=automatic toggle
#define bUEP_X_RES1       0x02      // handshake response type high bit for USB endpoint receiving/transmittal (OUT/IN)
#define bUEP_X_RES0       0x01      // handshake response type low bit for USB endpoint receiving/transmittal (OUT/IN)
#define MASK_UEP_X_RES    0x03      // bit mask of handshake response type for USB endpoint receiving/transmittal (OUT/IN)
#define UEP_X_RES_ACK     0x00
#define UEP_X_RES_TOUT    0x01
#define UEP_X_RES_NAK     0x02
#define UEP_X_RES_STALL   0x03
// bUEP_X_RES1 & bUEP_X_RES0: handshake response type for USB endpoint receiving/transmittal (OUT/IN)
//   00: ACK (ready) for receiving, DATA0 or DATA1 then expecting ACK (ready) for transmittal
//   01: no response for receiving, DATA0 or DATA1 then expecting no response for transmittal, time out from host, for non-zero endpoint isochronous transactions
//   10: NAK (busy)
//   11: STALL (error)

#define D0_ADDR           XBYTE[0x2207] // device address, USB sub-device 0#
#define D1_ADDR           XBYTE[0x2217] // device address, USB sub-device 1#
#define D2_ADDR           XBYTE[0x2227] // device address, USB sub-device 2#
#define HB_ADDR           XBYTE[0x2237] // device address, USB hub
#define pD0_ADDR          PBYTE[0x07]
#define pD1_ADDR          PBYTE[0x17]
#define pD2_ADDR          PBYTE[0x27]
#define pHB_ADDR          PBYTE[0x37]

#define pDi_ADDR(i)       PBYTE[i*0x10+0x7]

#define D0_EP0T_L         XBYTE[0x2208] // transmittal length for endpoint 0, USB sub-device 0#
#define D0_EP1T_L         XBYTE[0x2209] // transmittal length for endpoint 1, USB sub-device 0#
#define D0_EP2T_L         XBYTE[0x220A] // transmittal length for endpoint 2, USB sub-device 0#
#define D0_EP3T_L         XBYTE[0x220B] // transmittal length for endpoint 3, USB sub-device 0#
#define D0_EP5T_L         XBYTE[0x220D] // transmittal length for endpoint 5, USB sub-device 0#
#define D0_EP6T_L         XBYTE[0x220E] // WriteOnly: transmittal length for endpoint 6, USB sub-device 0#, readable if bUX_EP6I_EN=1 & bUX_IE_TRANSFER=0
//#define D0_RX_LEN         XBYTE[0x220E] // ReadOnly: length of data received, USB sub-device 0#, invisible if bUX_EP6I_EN=1 & bUX_IE_TRANSFER=0, keep last data during SETUP token
#define D0_RX_LEN         D0_EP6T_L
#define pD0_EP0T_L        PBYTE[0x08]
#define pD0_EP1T_L        PBYTE[0x09]
#define pD0_EP2T_L        PBYTE[0x0A]
#define pD0_EP3T_L        PBYTE[0x0B]
#define pD0_EP5T_L        PBYTE[0x0D]
#define pD0_EP6T_L        PBYTE[0x0E]
#define pD0_RX_LEN        PBYTE[0x0E]
#define D1_EP0T_L         XBYTE[0x2218] // transmittal length for endpoint 0, USB sub-device 1#
#define D1_EP1T_L         XBYTE[0x2219] // transmittal length for endpoint 1, USB sub-device 1#
#define D1_EP2T_L         XBYTE[0x221A] // transmittal length for endpoint 2, USB sub-device 1#
#define D1_EP3T_L         XBYTE[0x221B] // transmittal length for endpoint 3, USB sub-device 1#
#define D1_EP5T_L         XBYTE[0x221D] // transmittal length for endpoint 5, USB sub-device 1#
#define D1_EP6T_L         XBYTE[0x221E] // WriteOnly: transmittal length for endpoint 6, USB sub-device 1#, readable if bUX_EP6I_EN=1 & bUX_IE_TRANSFER=0
//#define D1_RX_LEN         XBYTE[0x221E] // ReadOnly: length of data received, USB sub-device 1#, invisible if bUX_EP6I_EN=1 & bUX_IE_TRANSFER=0, keep last data during SETUP token
#define D1_RX_LEN         D1_EP6T_L
#define pD1_EP0T_L        PBYTE[0x18]
#define pD1_EP1T_L        PBYTE[0x19]
#define pD1_EP2T_L        PBYTE[0x1A]
#define pD1_EP3T_L        PBYTE[0x1B]
#define pD1_EP5T_L        PBYTE[0x1D]
#define pD1_EP6T_L        PBYTE[0x1E]
#define pD1_RX_LEN        PBYTE[0x1E]
#define D2_EP0T_L         XBYTE[0x2228] // transmittal length for endpoint 0, USB sub-device 2#
#define D2_EP1T_L         XBYTE[0x2229] // transmittal length for endpoint 1, USB sub-device 2#
#define D2_EP2T_L         XBYTE[0x222A] // transmittal length for endpoint 2, USB sub-device 2#
#define D2_EP3T_L         XBYTE[0x222B] // transmittal length for endpoint 3, USB sub-device 2#
#define D2_EP5T_L         XBYTE[0x222D] // transmittal length for endpoint 5, USB sub-device 2#
#define D2_EP6T_L         XBYTE[0x222E] // WriteOnly: transmittal length for endpoint 6, USB sub-device 2#, readable if bUX_EP6I_EN=1 & bUX_IE_TRANSFER=0
//#define D2_RX_LEN         XBYTE[0x222E] // ReadOnly: length of data received, USB sub-device 2#, invisible if bUX_EP6I_EN=1 & bUX_IE_TRANSFER=0, keep last data during SETUP token
#define D2_RX_LEN         D2_EP6T_L
#define pD2_EP0T_L        PBYTE[0x28]
#define pD2_EP1T_L        PBYTE[0x29]
#define pD2_EP2T_L        PBYTE[0x2A]
#define pD2_EP3T_L        PBYTE[0x2B]
#define pD2_EP5T_L        PBYTE[0x2D]
#define pD2_EP6T_L        PBYTE[0x2E]
#define pD2_RX_LEN        PBYTE[0x2E]
#define HB_EP0T_L         XBYTE[0x2238] // transmittal length for endpoint 0, USB hub
#define HB_EP1T_L         XBYTE[0x2239] // transmittal length for endpoint 1, USB hub
#define HB_RX_LEN         XBYTE[0x223E] // ReadOnly: length of data received, USB hub, keep last data during SETUP token
#define pHB_EP0T_L        PBYTE[0x38]
#define pHB_EP1T_L        PBYTE[0x39]
#define pHB_RX_LEN        PBYTE[0x3E]

#define pDi_EP0T_L(i)     PBYTE[i*0x10+0x8]
#define pDi_EP1T_L(i)     PBYTE[i*0x10+0x9]
#define pDi_EP2T_L(i)     PBYTE[i*0x10+0xA]
#define pDi_EP3T_L(i)     PBYTE[i*0x10+0xB]
#define pDi_EP5T_L(i)     PBYTE[i*0x10+0xD]
#define pDi_EP6T_L(i)     PBYTE[i*0x10+0xE]
#define pDi_RX_LEN(i)     PBYTE[i*0x10+0xE]

#define D0_STATUS         XBYTE[0x220F] // ReadOnly: USB transfer status, USB sub-device 0#
#define D1_STATUS         XBYTE[0x221F] // ReadOnly: USB transfer status, USB sub-device 1#
#define D2_STATUS         XBYTE[0x222F] // ReadOnly: USB transfer status, USB sub-device 2#
#define HB_STATUS         XBYTE[0x223F] // ReadOnly: USB transfer status, USB hub
#define pD0_STATUS        PBYTE[0x0F]
#define pD1_STATUS        PBYTE[0x1F]
#define pD2_STATUS        PBYTE[0x2F]
#define pHB_STATUS        PBYTE[0x3F]

#define pDi_STATUS(i)     PBYTE[i*0x10+0xF]

#define bUXS_IS_NAK       0x80      // ReadOnly: indicate current USB transfer is NAK received
#define bUXS_TOG_OK       0x40      // ReadOnly: indicate current USB transfer toggle is OK, keep last status during SETUP token
#define bUXS_SETUP_ACT    0x20      // ReadOnly: indicate SETUP token & 8 bytes setup request received
#define bUXS_TOKEN1       0x10      // ReadOnly: current token PID code high bit received, clear bUX_IF_D?_TRANS to set free
#define bUXS_TOKEN0       0x08      // ReadOnly: current token PID code low bit received, clear bUX_IF_D?_TRANS to set free
#define MASK_UXS_TOKEN    0x18      // ReadOnly: bit mask of current token PID code received
#define UXS_TOKEN_OUT     0x00
#define UXS_TOKEN_SOF     0x08
#define UXS_TOKEN_IN      0x10
#define UXS_TOKEN_FREE    0x18
// bUXS_TOKEN1 & bUXS_TOKEN0: current token PID code received, keep last status during SETUP token, clear bUX_IF_D?_TRANS ( bUX_IF_D?_TRANS from 1 to 0 ) to set free
//   00: OUT token PID received
//   01: SOF token PID received
//   10: IN token PID received
//   11: free
#define MASK_UXS_ENDP     0x07      // ReadOnly: bit mask of current transfer endpoint number, keep last status during SETUP token

#define D0_EP_MOD         XBYTE[0x220C] // USB endpoint mode, USB sub-device 0#
#define D1_EP_MOD         XBYTE[0x221C] // USB endpoint mode, USB sub-device 1#
#define D2_EP_MOD         XBYTE[0x222C] // USB endpoint mode, USB sub-device 2#
#define pD0_EP_MOD        PBYTE[0x0C]
#define pD1_EP_MOD        PBYTE[0x1C]
#define pD2_EP_MOD        PBYTE[0x2C]
#define bUX_EP6I_EN       0x40      // USB endpoint 6 IN enable: 0=EP6 disable, 1=EP6 IN enable
#define bUX_EP5I_EN       0x20      // USB endpoint 5 IN enable: 0=EP5 disable, 1=EP5 IN enable
#define bUX_EP4O_EN       0x10      // USB endpoint 4 OUT enable: 0=EP4 disable, 1=EP4 OUT enable
#define bUX_EP3O_EN       0x08      // USB endpoint 3 OUT enable & endpoint 3 IN disable: 0=EP3 is IN only, 1=EP3 is OUT only
#define bUX_EP2O_EN       0x04      // USB endpoint 2 OUT enable & endpoint 2 IN disable: 0=EP2 is IN only, 1=EP2 is OUT only
#define bUX_EP1O_EN       0x02      // USB endpoint 1 OUT enable & endpoint 1 IN disable: 0=EP1 is IN only, 1=EP1 is OUT only
#define bUX_DEV_EN        0x01      // USB sub-device enable

#define USB_IE            XBYTE[0x223B] // USB interrupt enable, USB hub
#define pUSB_IE           PBYTE[0x3B]
#define bUX_IE_SOF        0x80      // enable USB interrupt for SOF received
#define bUX_IE_NAK        0x40      // enable USB interrupt for NAK responded
#define bUX_SIE_FREE      0x20      // ReadOnly: indicate USB SIE free status
#define bUX_IE_FIFO_OV    0x10      // enable USB interrupt for FIFO overflow
#define bUX_R_FIFO_RDY    0x08      // ReadOnly: indicate USB receiving FIFO ready status (not empty)
#define bUX_IE_SUSPEND    0x04      // enable interrupt for USB suspend or resume event
#define bUX_IE_TRANSFER   0x02      // enable interrupt for USB transfer completion
#define bUX_IE_BUS_RST    0x01      // enable interrupt for USB bus reset event

#define USB_CTRL          XBYTE[0x223C] // USB base control, USB hub
#define pUSB_CTRL         PBYTE[0x3C]
#define bUX_DP_PU_EN      0x80      // DP internal pullup resistance (1K5) enable, and force USB device enable
#define bUX_DM_PU_EN      0x40      // DM internal pullup resistance (1K5) enable, and force USB device enable
#define bUX_HUB_EP4_EN    0x10      // USB hub endpoint 4 enable, and force USB device enable
#define bUX_RESET_SIE     0x04      // force reset USB SIE, need software clear
#define bUX_CLR_ALL       0x02      // force clear FIFO and count of USB
#define bUX_BUS_RESET     0x01      // ReadOnly: indicate USB bus reset status

#define USB_IF            XBYTE[0x223D] // USB interrupt flag, USB hub
#define pUSB_IF           PBYTE[0x3D]
#define bUX_IF_D2_TRANS   0x80      // USB sub-device 2# transfer completion interrupt flag, write D2_STATUS to clear
#define bUX_IF_D1_TRANS   0x40      // USB sub-device 1# transfer completion interrupt flag, write D1_STATUS to clear
#define bUX_IF_D0_TRANS   0x20      // USB sub-device 0# transfer completion interrupt flag, write D0_STATUS to clear
#define bUX_IF_FIFO_OV    0x10      // USB FIFO overflow interrupt flag, write 1 to clear
#define bUX_SUSPEND       0x08      // ReadOnly: indicate USB suspend status
#define bUX_IF_SUSPEND    0x04      // USB suspend or resume event interrupt flag, write 1 to clear
#define bUX_IF_HB_TRANS   0x02      // USB hub transfer completion interrupt flag, write HB_STATUS to clear
#define bUX_IF_BUS_RST    0x01      // USB bus reset event interrupt flag, write 1 to clear

#define pUSB_BUF_BASE     ((PUINT8X)0x1000)    // USB buffer base address
#define pUSB_BUF_DEV0     ((PUINT8X)0x1000)    // USB sub-device 0# buffer base address
#define pUSB_BUF_DEV1     ((PUINT8X)0x1100)    // USB sub-device 1# buffer base address
#define pUSB_BUF_DEV2     ((PUINT8X)0x1200)    // USB sub-device 2# buffer base address
#define pUSB_BUF_HUB      ((PUINT8X)0x1300)    // USB hub buffer base address

#define pUSB_BUF_DEVi(i)  ((PUINT8X)(i*0x100+0x1000))

#define UX_EP0_ADDR       0x00      // USB endpoint 0 buffer start offset address
#define UX_EP0_SIZE_D4    0x40      // USB endpoint 0 maximum packet size if no endpoint 4
#define UX_EP0_SIZE_E4    0x20      // USB endpoint 0 maximum packet size if endpoint 4 enable
#define UX_EP4_ADDR       0x20      // USB endpoint 4 buffer start offset address
#define UX_EP4_SIZE       0x20      // USB endpoint 4 maximum packet size
#define UX_EP1_ADDR       0x40      // USB endpoint 1 buffer start offset address
#define UX_EP1_SIZE_D5    0x40      // USB endpoint 1 maximum packet size if no endpoint 5
#define UX_EP1_SIZE_E5    0x20      // USB endpoint 1 maximum packet size if endpoint 5 enable
#define UX_EP5_ADDR       0x60      // USB endpoint 5 buffer start offset address
#define UX_EP5_SIZE       0x20      // USB endpoint 5 maximum packet size
#define UX_EP2_ADDR       0x80      // USB endpoint 2 buffer start offset address
#define UX_EP2_SIZE_D6    0x40      // USB endpoint 2 maximum packet size if no endpoint 6
#define UX_EP2_SIZE_E6    0x20      // USB endpoint 2 maximum packet size if endpoint 6 enable
#define UX_EP6_ADDR       0xA0      // USB endpoint 6 buffer start offset address
#define UX_EP6_SIZE       0x20      // USB endpoint 6 maximum packet size
#define UX_EP3_ADDR       0xC0      // USB endpoint 3 buffer start offset address
#define UX_EP3_SIZE       0x40      // USB endpoint 3 maximum packet size

#define pD0_EP0_BUF       (pUSB_BUF_DEV0+UX_EP0_ADDR)     // point USB sub-device 0# endpoint 0 buffer
#define pD1_EP0_BUF       (pUSB_BUF_DEV1+UX_EP0_ADDR)     // point USB sub-device 1# endpoint 0 buffer
#define pD2_EP0_BUF       (pUSB_BUF_DEV2+UX_EP0_ADDR)     // point USB sub-device 2# endpoint 0 buffer
#define pHB_EP0_BUF       (pUSB_BUF_HUB+UX_EP0_ADDR)      // point USB hub endpoint 0 buffer
#define pHB_EP1_BUF       (pUSB_BUF_HUB+UX_EP1_ADDR)      // point USB hub endpoint 1 buffer

#define pDi_EP0_BUF(i)    ((PUINT8X)(i*0x100+0x1000+UX_EP0_ADDR))

#define pD0_SETUP_REQ     ((PXUSB_SETUP_REQ)pD0_EP0_BUF)  // point USB sub-device 0# SETUP request buffer
#define pD1_SETUP_REQ     ((PXUSB_SETUP_REQ)pD1_EP0_BUF)  // point USB sub-device 1# SETUP request buffer
#define pD2_SETUP_REQ     ((PXUSB_SETUP_REQ)pD2_EP0_BUF)  // point USB sub-device 2# SETUP request buffer
#define pHB_SETUP_REQ     ((PXUSB_SETUP_REQ)pHB_EP0_BUF)  // point USB hub SETUP request buffer

/*----- XDATA: xRAM ------------------------------------------*/

#define XDATA_RAM_SIZE    0x2000    // size of expanded xRAM, xdata SRAM embedded chip

/*----- Reference Information --------------------------------------------*/
#define ID_CH555          0x56      // chip ID

/* Interrupt routine address and interrupt number */
#define INT_ADDR_INT0     0x0003    // interrupt vector address for INT0
#define INT_ADDR_TMR0     0x000B    // interrupt vector address for timer0
#define INT_ADDR_INT1     0x0013    // interrupt vector address for INT1
#define INT_ADDR_TMR1     0x001B    // interrupt vector address for timer1
#define INT_ADDR_UART0    0x0023    // interrupt vector address for UART0
#define INT_ADDR_TMR2     0x002B    // interrupt vector address for timer2
#define INT_ADDR_SPI0     0x0033    // interrupt vector address for SPI0
#define INT_ADDR_USB      0x003B    // interrupt vector address for USB
#define INT_ADDR_ADC      0x004B    // interrupt vector address for ADC
#define INT_ADDR_UART1    0x0053    // interrupt vector address for UART1
#define INT_ADDR_PWM_I2C  0x005B    // interrupt vector address for LED/I2C
#define INT_ADDR_GPIO     0x0063    // interrupt vector address for GPIO
#define INT_ADDR_WDOG     0x006B    // interrupt vector address for watch-dog timer
#define INT_NO_INT0       0         // interrupt number for INT0
#define INT_NO_TMR0       1         // interrupt number for timer0
#define INT_NO_INT1       2         // interrupt number for INT1
#define INT_NO_TMR1       3         // interrupt number for timer1
#define INT_NO_UART0      4         // interrupt number for UART0
#define INT_NO_TMR2       5         // interrupt number for timer2
#define INT_NO_SPI0       6         // interrupt number for SPI0
#define INT_NO_USB        7         // interrupt number for USB
#define INT_NO_ADC        9         // interrupt number for ADC
#define INT_NO_UART1      10        // interrupt number for UART1
#define INT_NO_PWM_I2C    11        // interrupt number for LED/I2C
#define INT_NO_GPIO       12        // interrupt number for GPIO
#define INT_NO_WDOG       13        // interrupt number for watch-dog timer

/* Special Program Space */
#define DATA_FLASH_ADDR   0xF000    // start address of Data-Flash
#define BOOT_LOAD_ADDR    0xF400    // start address of boot loader program
#define ROM_CFG_ADDR      0xFFFE    // chip configuration information address
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

#endif  // __CH555_H__

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
#define	USB_DEVICE_ADDR			0x02	/* 默认的USB设备地址 */
#endif
#ifndef DEFAULT_ENDP0_SIZE
#define DEFAULT_ENDP0_SIZE      8       /* default maximum packet size for endpoint 0 */
#endif
#ifndef MAX_PACKET_SIZE
#define MAX_PACKET_SIZE         64      /* maximum packet size */
#endif
#ifndef USB_BO_CBW_SIZE
#define USB_BO_CBW_SIZE			0x1F	/* 命令块CBW的总长度 */
#define USB_BO_CSW_SIZE			0x0D	/* 命令状态块CSW的总长度 */
#endif
#ifndef USB_BO_CBW_SIG0
#define USB_BO_CBW_SIG0         0x55    /* 命令块CBW识别标志'USBC' */
#define USB_BO_CBW_SIG1         0x53
#define USB_BO_CBW_SIG2         0x42
#define USB_BO_CBW_SIG3         0x43
#define USB_BO_CSW_SIG0         0x55    /* 命令状态块CSW识别标志'USBS' */
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
