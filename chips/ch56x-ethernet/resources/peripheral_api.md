# CH56x Peripheral API Reference

Complete function reference extracted from CH56x EVT peripheral driver headers.

## System (CH56x_sys.h)

```c
// Chip identification
UINT8 SYS_GetChipID();                     // Get chip ID (fixed value)
UINT8 SYS_GetAccessID();                   // Get security access ID
UINT8 SYS_GetInfoSta(SYS_InfoStaTypeDef i); // Get system info status

// Delay functions
void Delay_Init(uint32_t systemclck);      // Initialize delay timer
void mDelayuS(uint32_t n);                 // Microsecond delay
void mDelaymS(uint32_t n);                 // Millisecond delay
#define DelayMs(x) mDelaymS(x)             // Alias
#define DelayUs(x) mDelayuS(x)             // Alias

// Reset
void SYS_ResetExecute(void);               // Software reset
UINT8 SYS_GetLastResetSta();               // Get last reset reason
#define SYS_ResetKeepBuf(d) (R8_GLOB_RESET_KEEP = d) // Keep value across reset

// Watchdog (WWDG)
#define WWDG_SetCounter(c) (R8_WDOG_COUNT = c) // Load watchdog counter
void WWDG_ITCfg(UINT8 s);                 // Watchdog interrupt enable
void WWDG_ResetCfg(UINT8 s);              // Watchdog reset enable
#define WWDG_GetFlowFlag() (R8_RST_WDOG_CTRL&RB_WDOG_INT_FLAG)
void WWDG_ClearFlag(void);                 // Clear watchdog flag

// SYS_InfoStaTypeDef values:
// INFO_RESET_EN  - External reset input enabled
// INFO_BOOT_EN   - Bootloader enabled
// INFO_DEBUG_EN  - Debug interface enabled
// INFO_LOADER    - Currently in bootloader area
```

## Clock (CH56x_clk.h)

```c
void SystemInit(uint32_t systemclck);      // Initialize system clock
UINT32 GetSysClock(void);                  // Get current clock frequency

// Available clock sources:
// CLK_SOURCE_PLL_15MHz   (15MHz, power-on default)
// CLK_SOURCE_PLL_30MHz   (30MHz)
// CLK_SOURCE_PLL_60MHz   (60MHz)
// CLK_SOURCE_PLL_80MHz   (80MHz)
// CLK_SOURCE_PLL_96MHz   (96MHz)
// CLK_SOURCE_PLL_120MHz  (120MHz, maximum)
```

## GPIO (CH56x_gpio.h)

```c
// Pin mode configuration
void GPIOA_ModeCfg(UINT32 pin, GPIOModeTypeDef mode);
void GPIOB_ModeCfg(UINT32 pin, GPIOModeTypeDef mode);

// Output control (macros)
#define GPIOA_SetBits(pin)       (R32_PA_OUT |= pin)    // Set high
#define GPIOA_ResetBits(pin)     (R32_PA_CLR |= pin)    // Set low
#define GPIOA_InverseBits(pin)   (R32_PA_OUT ^= pin)    // Toggle
#define GPIOB_SetBits(pin)       (R32_PB_OUT |= pin)
#define GPIOB_ResetBits(pin)     (R32_PB_CLR |= pin)
#define GPIOB_InverseBits(pin)   (R32_PB_OUT ^= pin)

// Input read (macros)
#define GPIOA_ReadPort()         (R32_PA_PIN)            // Read port A
#define GPIOB_ReadPort()         (R32_PB_PIN)            // Read port B
#define GPIOA_ReadPortPin(pin)   (R32_PA_PIN & pin)     // Read specific pin
#define GPIOB_ReadPortPin(pin)   (R32_PB_PIN & pin)

// Interrupt configuration
void GPIOA_ITModeCfg(UINT32 pin, GPIOITModeTpDef mode);
void GPIOB_ITModeCfg(UINT32 pin, GPIOITModeTpDef mode);

// Interrupt flags (macros for specific pins)
#define GPIOA_2_ReadITFlagBit()   (R8_GPIO_INT_FLAG & 0x01)
#define GPIOA_3_ReadITFlagBit()   (R8_GPIO_INT_FLAG & 0x02)
#define GPIOA_4_ReadITFlagBit()   (R8_GPIO_INT_FLAG & 0x04)
#define GPIOB_3_ReadITFlagBit()   (R8_GPIO_INT_FLAG & 0x08)
#define GPIOB_4_ReadITFlagBit()   (R8_GPIO_INT_FLAG & 0x10)
#define GPIOB_11_ReadITFlagBit()  (R8_GPIO_INT_FLAG & 0x20)
#define GPIOB_12_ReadITFlagBit()  (R8_GPIO_INT_FLAG & 0x40)
#define GPIOB_15_ReadITFlagBit()  (R8_GPIO_INT_FLAG & 0x80)

#define GPIOA_2_ClearITFlagBit()  (R8_GPIO_INT_FLAG = 0x01)
#define GPIOA_3_ClearITFlagBit()  (R8_GPIO_INT_FLAG = 0x02)
#define GPIOA_4_ClearITFlagBit()  (R8_GPIO_INT_FLAG = 0x04)
#define GPIOB_3_ClearITFlagBit()  (R8_GPIO_INT_FLAG = 0x08)
#define GPIOB_4_ClearITFlagBit()  (R8_GPIO_INT_FLAG = 0x10)
#define GPIOB_11_ClearITFlagBit() (R8_GPIO_INT_FLAG = 0x20)
#define GPIOB_12_ClearITFlagBit() (R8_GPIO_INT_FLAG = 0x40)
#define GPIOB_15_ClearITFlagBit() (R8_GPIO_INT_FLAG = 0x80)

// Pin remap and MCO
void GPIOPinRemap(UINT8 s, UINT16 perph);  // Peripheral pin remap
void GPIOMco(UINT8 s, UINT16 freq);        // Clock output

// GPIOModeTypeDef values:
// GPIO_ModeIN_Floating    - Floating input
// GPIO_ModeIN_PU_NSMT     - Pull-up, no Schmitt
// GPIO_ModeIN_PD_NSMT     - Pull-down, no Schmitt
// GPIO_ModeIN_PU_SMT      - Pull-up, Schmitt
// GPIO_ModeIN_PD_SMT      - Pull-down, Schmitt
// GPIO_Slowascent_PP_8mA  - Push-pull 8mA slow
// GPIO_Slowascent_PP_16mA - Push-pull 16mA slow
// GPIO_Highspeed_PP_8mA   - Push-pull 8mA fast
// GPIO_Highspeed_PP_16mA  - Push-pull 16mA fast
// GPIO_ModeOut_OP_8mA     - Open-drain 8mA
// GPIO_ModeOut_OP_16mA    - Open-drain 16mA

// GPIOITModeTpDef values:
// GPIO_ITMode_LowLevel    - Low level
// GPIO_ITMode_HighLevel   - High level
// GPIO_ITMode_FallEdge    - Falling edge
// GPIO_ITMode_RiseEdge    - Rising edge
```

## UART (CH56x_uart.h)

Four UARTs: UART0, UART1, UART2, UART3. Identical API for each.

```c
// Initialization
void UART0_DefInit(void);                  // Default init (115200 baud, 8N1)
void UART0_BaudRateCfg(UINT32 baudrate);   // Set baud rate
void UART0_ByteTrigCfg(UARTByteTRIGTypeDef b); // Set FIFO trigger level
void UART0_INTCfg(UINT8 s, UINT8 i);      // Configure interrupt
void UART0_Reset(void);                    // Software reset

// Data transfer
#define UART0_SendByte(b)      (R8_UART0_THR = b)   // Send single byte
void UART0_SendString(PUINT8 buf, UINT16 l);        // Send multiple bytes
#define UART0_RecvByte()       (R8_UART0_RBR)        // Receive single byte
UINT16 UART0_RecvString(PUINT8 buf);                 // Receive multiple bytes

// FIFO control (macros)
#define UART0_CLR_RXFIFO()     (R8_UART0_FCR |= RB_FCR_RX_FIFO_CLR)
#define UART0_CLR_TXFIFO()     (R8_UART0_FCR |= RB_FCR_TX_FIFO_CLR)

// Status (macros)
#define UART0_GetITFlag()      (R8_UART0_IIR & RB_IIR_INT_MASK)
#define UART0_GetLinSTA()      (R8_UART0_LSR)        // Line status
#define UART0_GetMSRSTA()      (R8_UART0_MSR)        // Modem status (UART0 only)

// Status flags:
// STA_ERR_BREAK   - Break error
// STA_ERR_FRAME   - Frame error
// STA_ERR_PAR     - Parity error
// STA_ERR_FIFOOV  - RX FIFO overflow
// STA_TXFIFO_EMP  - TX FIFO empty
// STA_TXALL_EMP   - TX all empty
// STA_RECV_DATA   - Data available

// UARTByteTRIGTypeDef values:
// UART_1BYTE_TRIG  - 1 byte trigger
// UART_2BYTE_TRIG  - 2 byte trigger
// UART_4BYTE_TRIG  - 4 byte trigger
// UART_7BYTE_TRIG  - 7 byte trigger
```

## SPI (CH56x_spi.h)

Two SPIs: SPI0, SPI1. Identical API for each.

```c
// Master mode
void SPI0_MasterDefInit(void);             // Default master init
void SPI0_DataMode(ModeBitOrderTypeDef m); // Set data mode
void SPI0_MasterSendByte(UINT8 d);         // Send single byte
UINT8 SPI0_MasterRecvByte(void);           // Receive single byte
void SPI0_MasterTrans(UINT8 *pbuf, UINT16 len); // Send buffer
void SPI0_MasterRecv(UINT8 *pbuf, UINT16 len);  // Receive buffer
void SPI0_MasterDMATrans(PUINT8 pbuf, UINT16 len); // DMA send
void SPI0_MasterDMARecv(PUINT8 pbuf, UINT16 len);  // DMA receive

// Slave mode
void SPI0_SlaveInit(void);                 // Initialize as slave
void SPI0_SlaveSendByte(UINT8 d);
UINT8 SPI0_SlaveRecvByte(void);
void SPI0_SlaveTrans(UINT8 *pbuf, UINT16 len);
void SPI0_SlaveRecv(PUINT8 pbuf, UINT16 len);
#define SetFirst0Data(d) (R8_SPI0_SLAVE_PRE = d)

// Interrupt (macros)
#define SPI0_ITCfg(s,f)    ((s)?(R8_SPI0_INTER_EN|=f):(R8_SPI0_INTER_EN&=~f))
#define SPI0_GetITFlag(f)  (R8_SPI0_INT_FLAG&f)
#define SPI0_ClearITFlag(f)(R8_SPI0_INT_FLAG = f)

// SPI interrupt flags:
// SPI0_IT_FST_BYTE  - First byte
// SPI0_IT_FIFO_OV   - FIFO overflow
// SPI0_IT_DMA_END   - DMA end
// SPI0_IT_FIFO_HF   - FIFO half full
// SPI0_IT_BYTE_END  - Byte end
// SPI0_IT_CNT_END   - Counter end

// ModeBitOrderTypeDef values:
// Mode0_LowBitINFront   - CPOL=0 CPHA=0, LSB first
// Mode0_HighBitINFront  - CPOL=0 CPHA=0, MSB first
// Mode3_LowBitINFront   - CPOL=1 CPHA=1, LSB first
// Mode3_HighBitINFront  - CPOL=1 CPHA=1, MSB first
```

## Timer (CH56x_timer.h)

Three timers: TMR0, TMR1, TMR2. Identical API for each.

```c
// Timer mode
void TMR0_TimerInit(UINT32 t);             // Timer init (period in ticks)
void TMR0_EXTSignalCounterInit(UINT32 c);  // External counter init
#define TMR0_GetCurrentCount() R32_TMR0_COUNT  // Read counter

// PWM mode
#define TMR0_PWMCycleCfg(cyc) (R32_TMR0_CNT_END=cyc) // PWM period
void TMR0_PWMInit(PWM_PolarTypeDef pr, PWM_RepeatTsTypeDef ts);
#define TMR0_PWMActDataWidth(d) (R32_TMR0_FIFO = d) // PWM duty

// Input capture
#define TMR0_CAPTimeoutCfg(cyc) (R32_TMR0_CNT_END=cyc)
void TMR0_CapInit(CapModeTypeDef cap);
#define TMR0_CAPGetData()       R32_TMR0_FIFO        // Read captured value
#define TMR0_CAPDataCounter()   R8_TMR0_FIFO_COUNT   // Number of captured values

// Control (macros)
#define TMR0_Disable()   (R8_TMR0_CTRL_MOD &= ~RB_TMR_COUNT_EN)
#define TMR0_ITCfg(s,f)  ((s)?(R8_TMR0_INTER_EN|=f):(R8_TMR0_INTER_EN&=~f))
#define TMR0_ClearITFlag(f) (R8_TMR0_INT_FLAG = f)
#define TMR0_GetITFlag(f)   (R8_TMR0_INT_FLAG & f)

// TMR1 and TMR2 additionally support DMA:
void TMR1_DMACfg(UINT8 s, UINT16 startAddr, UINT16 endAddr, DMAModeTypeDef m);
void TMR2_DMACfg(UINT8 s, UINT16 startAddr, UINT16 endAddr, DMAModeTypeDef m);

// DMAModeTypeDef:
// Mode_Single - Single transfer
// Mode_LOOP   - Continuous loop

// PWM_PolarTypeDef:
// high_on_low - Default low, active high
// low_on_high - Default high, active low

// CapModeTypeDef:
// CAP_NULL               - No capture
// Edge_To_Edge           - Any edge
// FallEdge_To_FallEdge   - Falling to falling
// RiseEdge_To_RiseEdge   - Rising to rising
```

## PWM (CH56x_pwm.h)

Four independent PWM channels: PWM0-PWM3.

```c
void PWMX_CLKCfg(UINT8 d);                // Clock divider
void PWMX_CycleCfg(PWMX_CycleTypeDef cyc); // Period (256 or 255 cycles)
void PWMX_ACTOUT(UINT8 ch, UINT8 da, PWMX_PolarTypeDef pr, UINT8 s);

// Individual duty (macros)
#define PWM0_ActDataWidth(d) (R8_PWM0_DATA = d)
#define PWM1_ActDataWidth(d) (R8_PWM1_DATA = d)
#define PWM2_ActDataWidth(d) (R8_PWM2_DATA = d)
#define PWM3_ActDataWidth(d) (R8_PWM3_DATA = d)

// Channel constants:
// CH_PWM0  0x01
// CH_PWM1  0x02
// CH_PWM2  0x04
// CH_PWM3  0x08

// PWMX_CycleTypeDef:
// PWMX_Cycle_256 - 256 cycles
// PWMX_Cycle_255 - 255 cycles
```

## ECDC - Encryption/Decryption (CH56x_ecdc.h)

```c
void ECDC_Init(UINT8 ecdcmode, UINT8 clkmode, UINT8 keylen,
               PUINT32 pkey, PUINT32 pcount);
void ECDC_SetKey(PUINT32 pkey, UINT8 keylen);
void ECDC_SetCount(PUINT32 pcount);
void ECDC_Excute(UINT8 excutemode, UINT8 endianmode);
void ECDC_SingleRegister(PUINT32 pWdatbuff, PUINT32 pRdatbuff);
void ECDC_SelfDMA(UINT32 ram_addr, UINT32 ram_len);
void ECDC_RloadCount(UINT8 excutemode, UINT8 endianmode, PUINT32 pcount);

// Mode constants:
// MODE_SM4_ECB  0
// MODE_AES_ECB  1
// MODE_SM4_CTR  2
// MODE_AES_CTR  3

// Key length:
// KEYLENGTH_128BIT  0
// KEYLENGTH_192BIT  1
// KEYLENGTH_256BIT  2

// Clock:
// ECDCCLK_DISABLE  1
// ECDCCLK_240MHZ   2
// ECDCCLK_160MHZ   3

// Direction:
// SELFDMA_ENCRY           0x84
// SELFDMA_DECRY           0x8c
// PERIPHERAL_TO_RAM_ENCRY 0x02
// PERIPHERAL_TO_RAM_DECRY 0x0a
// RAM_TO_PERIPHERAL_ENCRY 0x04
// RAM_TO_PERIPHERAL_DECRY 0x0c
```

## Power Management (CH56x_pwr.h)

```c
void PWR_PeriphClkCfg(UINT8 s, UINT16 perph);     // Enable/disable peripheral clock
void PWR_PeriphWakeUpCfg(UINT8 s, UINT16 perph);  // Configure wakeup source
void LowPower_Idle(void);                          // Idle mode (WFI)
void LowPower_Idle_WFE(void);                      // Idle mode (WFE)
void LowPower_Halt(void);                          // Halt mode (WFI)
void LowPower_Halt_WFE(void);                      // Halt mode (WFE)
void LowPower_Sleep(void);                         // Sleep mode (WFI)
void LowPower_Sleep_WFE(void);                     // Sleep mode (WFE)

// Peripheral clock bits:
// BIT_SLP_CLK_TMR0    BIT_SLP_CLK_TMR1    BIT_SLP_CLK_TMR2
// BIT_SLP_CLK_PWMX    BIT_SLP_CLK_UART0   BIT_SLP_CLK_UART1
// BIT_SLP_CLK_UART2   BIT_SLP_CLK_UART3   BIT_SLP_CLK_SPI0
// BIT_SLP_CLK_SPI1    BIT_SLP_CLK_EMMC    BIT_SLP_CLK_HSPI
// BIT_SLP_CLK_USBHS   BIT_SLP_CLK_USBSS   BIT_SLP_CLK_SERD
// BIT_SLP_CLK_DVP     BIT_SLP_CLK_ETH     BIT_SLP_CLK_ECDC
```

## Ethernet (CH56x_eth.h)

```c
void ETH_SoftwareReset(void);
void ETH_StructInit(ETH_InitTypeDef *ETH_InitStruct);
uint16_t ETH_ReadPHYRegister(uint16_t PHYAddress, uint16_t PHYReg);
uint32_t ETH_WritePHYRegister(uint16_t PHYAddress, uint16_t PHYReg, uint16_t PHYValue);
void ETH_DMATxDescChainInit(ETH_DMADESCTypeDef *DMATxDescTab, uint8_t* TxBuff, uint32_t TxBuffCount);
void ETH_DMARxDescChainInit(ETH_DMADESCTypeDef *DMARxDescTab, uint8_t *RxBuff, uint32_t RxBuffCount);
void ETH_DMAITConfig(uint32_t ETH_DMA_IT, FunctionalState NewState);
void ETH_DMAClearITPendingBit(uint32_t ETH_DMA_IT);
void RGMII_TXC_Delay(uint8_t clock_polarity, uint8_t delay_time);

// Key register base: ETH ((ETH_TypeDef*)ETH_BASE)
// See CH56x_eth.h for full register and bit definitions
```

## eMMC (CH56x_emmc.h)

```c
UINT8 EMMCIO0Init(void);
UINT8 CheckCMDComp(PSD_PARAMETER pEMMCPara);
void EMMCResetIdle(PSD_PARAMETER pEMMCPara);
UINT8 EMMCReadOCR(PSD_PARAMETER pEMMCPara);
UINT8 EMMCReadCID(PSD_PARAMETER pEMMCPara);
UINT8 EMMCSetRCA(PSD_PARAMETER pEMMCPara);
UINT8 EMMCReadCSD(PSD_PARAMETER pEMMCPara);
UINT8 SelectEMMCCard(PSD_PARAMETER pEMMCPara);
UINT8 ReadEMMCStatus(PSD_PARAMETER pEMMCPara);
UINT8 EMMCSetBusWidth(PSD_PARAMETER pEMMCPara, UINT8 bus_mode);
UINT8 EMMCSetHighSpeed(PSD_PARAMETER pEMMCPara);
UINT8 EMMCCardConfig(PSD_PARAMETER pEMMCPara);
UINT8 EMMCIOTransErrorDeal(PSD_PARAMETER pEMMCPara);
UINT8 EMMCCardReadEXCSD(PSD_PARAMETER pEMMCPara, PUINT8 pRdatbuf);
UINT8 EMMCCardReadOneSec(PSD_PARAMETER pEMMCPara, PUINT8 pRdatbuf, UINT32 Lbaaddr);
UINT8 EMMCCardReadMulSec(PSD_PARAMETER pEMMCPara, PUINT16 pReqnum, PUINT8 pRdatbuf, UINT32 Lbaaddr);
UINT8 EMMCCardWriteMulSec(PSD_PARAMETER pEMMCPara, PUINT16 pReqnum, PUINT8 pWdatbuf, UINT32 Lbaaddr);
UINT8 AES_EMMCWriteMulSec(PSD_PARAMETER pEMMCPara, PUINT32 pReqnum, PUINT32 pWdatbuf, UINT32 Lbaaddr, UINT8 excutemode, UINT8 endianmode, PUINT32 pcount);
UINT8 AES_EMMCReadMulSec(PSD_PARAMETER pEMMCPara, PUINT32 pReqnum, PUINT32 pRdatbuf, UINT32 Lbaaddr, UINT8 excutemode, UINT8 endianmode, PUINT32 pcount);
```

## HSPI - High-Speed SPI (CH56x_hspi.h)

```c
void HSPI_Mode(UINT8 s, HSPI_ModeTypeDef i);
void HSPI_INTCfg(UINT8 s, UINT8 i);

// HSPI_ModeTypeDef:
// DOWN_Mode  0  (download/master mode)
// UP_Mode    1  (upload/slave mode)
```
