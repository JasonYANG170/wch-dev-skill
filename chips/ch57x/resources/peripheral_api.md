# CH57x Peripheral Driver API Quick Reference

> CH572 (RISC-V) and CH579 (ARM Cortex-M0) share most peripheral APIs. CH579-specific sections are marked with **[CH579]**.

## System Clock

```c
// Header: CH57x_sys.h
void SetSysClock(SYS_CLKTypeDef sc);
uint32_t GetSysClock(void);

// Clock sources
CLK_SOURCE_LSI           // Internal 32kHz RC
CLK_SOURCE_LSE           // External 32.768kHz crystal
CLK_SOURCE_HSE_8MHz      // External 8MHz crystal
CLK_SOURCE_PLL_60MHz     // PLL 60MHz (most common)
CLK_SOURCE_PLL_48MHz     // PLL 48MHz (USB compatible)
CLK_SOURCE_PLL_32MHz     // PLL 32MHz
CLK_SOURCE_PLL_24MHz     // PLL 24MHz
```

## GPIO

```c
// Header: CH57x_gpio.h

// Mode configuration
void GPIOA_ModeCfg(uint32_t pin, GPIOModeTypeDef mode);
void GPIOB_ModeCfg(uint32_t pin, GPIOModeTypeDef mode);

// Modes
GPIO_ModeIN_Floating     // Floating input
GPIO_ModeIN_PU           // Input with pull-up
GPIO_ModeIN_PD           // Input with pull-down
GPIO_ModeOut_PP_5mA      // Push-pull output 5mA
GPIO_ModeOut_PP_20mA     // Push-pull output 20mA

// Bit operations (macros)
GPIOA_SetBits(pin)       // Set pin high
GPIOA_ResetBits(pin)     // Set pin low (via clear register)
GPIOA_InverseBits(pin)   // Toggle pin
GPIOB_SetBits(pin)
GPIOB_ResetBits(pin)
GPIOB_InverseBits(pin)

// Read pin state
GPIOA_ReadPort()         // Read all GPIOA pins
GPIOB_ReadPort()         // Read all GPIOB pins

// Pin definitions
GPIO_Pin_0  through GPIO_Pin_23  // PA0-PA23 / PB0-PB23

// Interrupt configuration
void GPIOA_ITModeCfg(uint32_t pin, GPIOITModeTpDef mode);
void GPIOB_ITModeCfg(uint32_t pin, GPIOITModeTpDef mode);

GPIO_ITMode_LowLevel     // Low level trigger
GPIO_ITMode_HighLevel    // High level trigger
GPIO_ITMode_FallEdge     // Falling edge trigger
GPIO_ITMode_RiseEdge     // Rising edge trigger

// Interrupt flag operations (macros)
GPIOA_ReadITFlagBit(pin)    // Read interrupt flag
GPIOA_ClearITFlagBit(pin)   // Clear interrupt flag
GPIOB_ReadITFlagBit(pin)
GPIOB_ClearITFlagBit(pin)
```

## UART

```c
// Header: CH57x_uart0.h / CH57x_uart1.h / CH57x_uart2.h / CH57x_uart3.h

// Default init (115200, 8N1)
void UART0_DefInit(void);
void UART1_DefInit(void);
void UART2_DefInit(void);
void UART3_DefInit(void);

// Baud rate
void UART0_BaudRateCfg(uint32_t baudrate);

// Trigger level
UART_1BYTE_TRIG
UART_2BYTE_TRIG
UART_4BYTE_TRIG
UART_7BYTE_TRIG
void UART0_ByteTrigCfg(UARTByteTRIGTypeDef b);

// Send/Receive
void UART0_SendByte(uint8_t data);        // blocking single byte
void UART0_SendString(uint8_t *buf, uint16_t l);  // blocking multi-byte
uint8_t UART0_RecvByte(void);             // read single byte

// Interrupt configuration
void UART0_INTCfg(FunctionalState s, uint8_t i);
// Interrupt flags: RB_IER_RECV_RDY | RB_IER_THR_EMPTY | RB_IER_LINE_STAT

// Interrupt status
uint8_t UART0_GetITFlag(void);   // returns UART_II_xxx
// UART_II_RECV_RDY, UART_II_THR_EMPTY, UART_II_LINE_STAT, UART_II_MODEM_CHG, UART_II_NOINT

// UART1 is typically used for debug output (printf via PRINT macro)
```

## SPI

```c
// Header: CH57x_spi0.h

// Master mode
void SPI0_MasterDefInit(void);    // Mode 0+3, full duplex, 8MHz
void SPI0_CLKCfg(uint8_t c);     // Clock divisor

// Data mode
Mode0_LowBitINFront     // Mode 0, LSB first
Mode0_HighBitINFront    // Mode 0, MSB first
Mode3_LowBitINFront     // Mode 3, LSB first
Mode3_HighBitINFront    // Mode 3, MSB first
void SPI0_DataMode(ModeBitOrderTypeDef m);

// Master transfer
void SPI0_MasterSendByte(uint8_t d);
uint8_t SPI0_MasterRecvByte(void);
void SPI0_MasterTrans(uint8_t *pbuf, uint16_t len);
void SPI0_MasterRecv(uint8_t *pbuf, uint16_t len);

// DMA transfer
void SPI0_MasterDMATrans(uint8_t *pbuf, uint16_t len);
void SPI0_MasterDMARecv(uint8_t *pbuf, uint16_t len);

// Slave mode
void SPI0_SlaveInit(void);
void SPI0_SlaveSendByte(uint8_t d);
uint8_t SPI0_SlaveRecvByte(void);
```

## ADC

```c
// Header: CH57x_adc.h

// Initialization modes
void ADC_ExtSingleChSampInit(ADC_SampClkTypeDef sp, ADC_SignalPGATypeDef ga);
void ADC_ExtDiffChSampInit(ADC_SampClkTypeDef sp, ADC_SignalPGATypeDef ga);
void ADC_InterTSSampInit(void);     // Internal temperature sensor
void ADC_InterBATSampInit(void);    // Battery voltage (VDD)
void TouchKey_ChSampInit(void);     // Touch key

// Sampling clock
SampleFreq_3_2           // 3.2MHz
SampleFreq_8             // 8MHz
SampleFreq_5_33          // 5.33MHz
SampleFreq_4             // 4MHz

// PGA gain
ADC_PGA_1_4              // 1/4 gain
ADC_PGA_1_2              // 1/2 gain
ADC_PGA_0                // 1x gain
ADC_PGA_2                // 2x gain

// Channel and conversion
void ADC_ChannelCfg(uint8_t d);        // Select channel (0-11, 12=TS, 13=BAT, 14=TouchKey)
uint16_t ADC_ExcutSingleConver(void);  // Single conversion, returns ADC value
signed short ADC_DataCalib_Rough(void); // Calibrated reading

// Temperature conversion helper
int adc_to_temperature_celsius(uint16_t adc_val);

// DMA
void ADC_DMACfg(uint8_t s, uint16_t startAddr, uint16_t endAddr, ADC_DMAModeTypeDef m);
```

## Timer

```c
// Header: CH57x_timer0.h / timer1.h / timer2.h / timer3.h

// Timer init (period in system clock cycles)
void TMR0_TimerInit(uint32_t t);

// PWM mode
void TMR0_PWMInit(PWMX_PolarTypeDef pr, PWM_RepeatTsTypeDef ts);
void TMR0_PWMCycleCfg(uint32_t cyc);
void TMR0_PWMActDataWidth(uint32_t d);

// Capture mode
void TMR0_CapInit(CapModeTypeDef cap);
uint32_t TMR0_CAPGetData(void);

// Control
void TMR0_Enable(void);
void TMR0_Disable(void);
uint32_t TMR0_GetCurrentTimer(void);

// Interrupt
void TMR0_ITCfg(FunctionalState s, uint8_t f);
// Flags: TMR0_3_IT_CYC_END, TMR0_3_IT_DATA_ACT, TMR0_3_IT_FIFO_HF, TMR0_3_IT_FIFO_OV

// DMA (TMR1, TMR2 only)
void TMR1_DMACfg(uint8_t s, uint16_t startAddr, uint16_t endAddr, DMAModeTypeDef m);
```

## PWM (PWM4-PWM11)

```c
// Header: CH57x_pwm.h

// Clock configuration
void PWMX_CLKCfg(uint8_t d);     // Clock divisor

// Cycle configuration
void PWMX_CycleCfg(PWMX_CycleTypeDef cyc);
// PWMX_Cycle_256, PWMX_Cycle_255, PWMX_Cycle_128, PWMX_Cycle_64, PWMX_Cycle_32

// Output control
void PWMX_ACTOUT(uint8_t ch, uint8_t da, PWMX_PolarTypeDef pr, FunctionalState s);
// Channels: CH_PWM4 through CH_PWM11
// Polar: High_Level, Low_Level

// Individual channel data width (macro)
PWM4_ActDataWidth(d)
PWM5_ActDataWidth(d)
// ... through PWM11
```

## Flash / EEPROM

```c
// Header: CH57x_flash.h

// Flash (Code area) operations
void FLASH_ROM_READ(uint32_t StartAddr, void *Buffer, uint32_t len);
uint8_t FLASH_ROM_WRITE(uint32_t StartAddr, void *Buffer, uint32_t len);
uint8_t FLASH_ROM_ERASE(uint32_t StartAddr, uint32_t len);
uint8_t FLASH_ROM_VERIFY(uint32_t StartAddr, void *Buffer, uint32_t len);

// EEPROM (DataFlash) operations
uint8_t EEPROM_READ(uint32_t Addr, void *Buffer, uint32_t len);
uint8_t EEPROM_WRITE(uint32_t Addr, void *Buffer, uint32_t len);
uint8_t EEPROM_ERASE(uint32_t Addr, uint32_t len);

// Unique ID
void GET_UNIQUE_ID(uint8_t *Buffer);  // 8-byte unique chip ID

// Note: Flash write/erase operates on 256-byte sectors
// Note: DataFlash address range: 0x77E00 - 0x77FFF (for BLE SNV storage)
```

## Power Management

```c
// Header: CH57x_pwr.h

// Low power modes
void LowPower_Idle(void);          // CPU stops, peripherals continue
void LowPower_Halt(void);          // Low power, fast wakeup
void LowPower_Sleep(uint8_t rm);   // Lowest power, RTC wakeup
void LowPower_Shutdown(uint8_t rm); // Deep sleep, reset wakeup only

// Sleep RAM retention flags
RB_PWR_RAM2K     // Retain 2KB RAM
RB_PWR_RAM16K    // Retain 16KB RAM
RB_PWR_EXTEND    // Retain extended RAM
RB_PWR_XROM      // Retain XROM

// DC/DC converter
void PWR_DCDCCfg(FunctionalState s);

// Peripheral clock gating
void PWR_PeriphClkCfg(FunctionalState s, uint16_t perph);
// Bits: BIT_SLP_CLK_TMR0, BIT_SLP_CLK_UART0, BIT_SLP_CLK_SPI0, BIT_SLP_CLK_USB, etc.

// Wakeup source configuration
void PWR_PeriphWakeUpCfg(FunctionalState s, uint8_t perph, WakeUP_ModeypeDef mode);
// Sources: RB_SLP_USB_WAKE, RB_SLP_RTC_WAKE, RB_SLP_GPIO_WAKE, RB_SLP_BAT_WAKE
```

## USB

```c
// Header: CH57x_usbdev.h / CH57x_usbhostBase.h / CH57x_usbhostClass.h

// Device mode
void USB_DeviceInit(void);
void USB_DeviceSetConfig(uint8_t cfg);

// Host mode
void USB_HostInit(void);
uint8_t USB_HostEnumDevice(void);
```

---

## **[CH579]** SPI0 / SPI1

CH579 has two SPI peripherals. SPI0 is full-featured (Master + Slave, DMA). SPI1 is Master-only with FIFO (no DMA, no Slave mode).

```c
// Header: CH57x_spi.h

// --- SPI0 (full-featured, same API as CH572's single SPI) ---
void SPI0_MasterDefInit(void);    // Mode 0+3, full duplex, 8MHz
void SPI0_CLKCfg(uint8_t c);     // Clock divisor
void SPI0_DataMode(ModeBitOrderTypeDef m);

// Master transfer
void SPI0_MasterSendByte(uint8_t d);
uint8_t SPI0_MasterRecvByte(void);
void SPI0_MasterTrans(uint8_t *pbuf, uint16_t len);
void SPI0_MasterRecv(uint8_t *pbuf, uint16_t len);

// DMA transfer (SPI0 only)
void SPI0_MasterDMATrans(PUINT8 pbuf, uint16_t len);
void SPI0_MasterDMARecv(PUINT8 pbuf, uint16_t len);

// Slave mode (SPI0 only)
void SPI0_SlaveInit(void);
void SPI0_SlaveSendByte(uint8_t d);
uint8_t SPI0_SlaveRecvByte(void);
void SPI0_SlaveTrans(uint8_t *pbuf, uint16_t len);
void SPI0_SlaveRecv(PUINT8 pbuf, uint16_t len);
void SPI0_SlaveDMATrans(PUINT8 pbuf, uint16_t len);
void SPI0_SlaveDMARecv(PUINT8 pbuf, uint16_t len);

// SPI0 first-byte mode (slave)
#define SetFirstData(d)    (R8_SPI0_SLAVE_PRE = d)

// --- SPI1 (Master-only, FIFO, no DMA) ---
void SPI1_MasterDefInit(void);    // Mode 0+3, full duplex, 8MHz
void SPI1_CLKCfg(uint8_t c);
void SPI1_DataMode(ModeBitOrderTypeDef m);

void SPI1_MasterSendByte(uint8_t d);
uint8_t SPI1_MasterRecvByte(void);
void SPI1_MasterTrans(uint8_t *pbuf, uint16_t len);
void SPI1_MasterRecv(uint8_t *pbuf, uint16_t len);
// Note: SPI1 has no DMA functions and no Slave mode

// Interrupt (both SPI0 and SPI1)
#define SPI0_ITCfg(s,f)      ((s)?(R8_SPI0_INTER_EN|=f):(R8_SPI0_INTER_EN&=~f))
#define SPI0_GetITFlag(f)    (R8_SPI0_INT_FLAG&f)
#define SPI0_ClearITFlag(f)  (R8_SPI0_INT_FLAG = f)

#define SPI1_ITCfg(s,f)      ((s)?(R8_SPI1_INTER_EN|=f):(R8_SPI1_INTER_EN&=~f))
#define SPI1_GetITFlag(f)    (R8_SPI1_INT_FLAG&f)
#define SPI1_ClearITFlag(f)  (R8_SPI1_INT_FLAG = f)

// SPI interrupt flags (SPI0 and SPI1)
SPI0_IT_FST_BYTE     // First byte received (slave first-byte mode)
SPI0_IT_FIFO_OV      // FIFO overflow
SPI0_IT_DMA_END      // DMA transfer complete (SPI0 only)
SPI0_IT_FIFO_HF      // FIFO half-full
SPI0_IT_BYTE_END     // Single byte transfer complete
SPI0_IT_CNT_END      // All bytes transfer complete
```

---

## **[CH579]** LCD Segment Display

CH579 has a built-in segment LCD driver supporting up to 24 segments (SEG0-SEG23) on GPIO Port B (PB0-PB23) and up to 4 COM lines (COM0-COM3). The LCD pins are muxed with GPIO -- enabling LCD segments disables those GPIO pins for digital I/O.

```c
// Header: CH57x_lcd.h

// Initialization
void LCD_DefInit(void);   // Default init (3.3V, 1/3 bias, 1/4 duty, 256Hz scan)

// Power control (macros)
LCD_PowerOn()             // Enable LCD analog + digital system
LCD_PowerDown()           // Disable LCD

// Configuration (macros, use enum values)
LCD_PowerCfg(d)           // Drive voltage: LCD_PS_3V3 (3.3V) or LCD_PS_2V5 (2.5V)
LCD_ScanCLKCfg(d)         // Scan clock: LCD_CLK_256, LCD_CLK_512, LCD_CLK_1000, LCD_CLK_128
LCD_DutyCfg(d)            // Duty: LCD_1_2_Duty (COM0-1), LCD_1_3_Duty (COM0-2), LCD_1_4_Duty (COM0-3)
LCD_BiasCfg(d)            // Bias: LCD_1_2_Bias or LCD_1_3_Bias

// Write segment data (macros, one byte per SEG group)
LCD_WriteData0(d)    // SEG0 data
LCD_WriteData1(d)    // SEG1 data
// ... through ...
LCD_WriteData11(d)   // SEG11 data
// Each macro writes one byte to the corresponding LCD RAM register

// Pin mux enable (required before LCD_DefInit)
// Must disable digital input on LCD pins via GPIOAGPPCfg:
GPIOAGPPCfg(ENABLE, RB_PIN_SEG0_3_IE     // SEG0-SEG3
                  | RB_PIN_SEG4_7_IE      // SEG4-SEG7
                  | RB_PIN_SEG8_11_IE     // SEG8-SEG11
                  /* | RB_PIN_SEG12_15_IE ... */);
```

**Pin mapping**: SEG0=PB0, SEG1=PB1, ..., SEG23=PB23. When LCD is enabled on a segment, that PB pin can no longer be used as GPIO.

---

## **[CH579]** Ethernet (WCHNET Library)

CH579 uses an SPI-based 10/100M Ethernet controller with the WCHNET protocol stack library. The library provides TCP/IP, UDP, DHCP, DNS, MQTT, and FTP functionality.

```c
// Headers: wchnet.h, eth_driver.h, net_config.h
// Library: CH57xNET.lib (pre-compiled)

// --- Initialization ---
// 1. Configure net_config.h (socket count, TCP MSS, buffer sizes)
// 2. Call initialization sequence:
void ETH_Init(void);                          // Initialize ETH peripheral + PHY
uint8_t WCHNET_Init(const uint8_t *ip, ...); // Initialize WCHNET stack

// --- Socket API ---
uint8_t WCHNET_SocketCreate(uint8_t type, ...);  // Create socket (TCP/UDP/IPRAW)
uint8_t WCHNET_SocketConnect(uint8_t sockid, ...); // TCP connect
uint8_t WCHNET_SocketListen(uint8_t sockid);       // TCP listen
uint8_t WCHNET_SocketSend(uint8_t sockid, const void *buf, uint16_t len);
uint8_t WCHNET_SocketRecv(uint8_t sockid, void *buf, uint16_t *len);
uint8_t WCHNET_SocketClose(uint8_t sockid, uint8_t flags);

// --- Protocol types ---
PROTO_TYPE_IP_RAW    // 0: IP raw data
PROTO_TYPE_UDP       // 2: UDP
PROTO_TYPE_TCP       // 3: TCP

// --- Timer ---
void WCHNET_TimeIsr(uint10ms);  // Call periodically (10ms recommended)

// --- PHY state ---
PHY_LINK_SUCCESS     // PHY link established
PHY_AUTO_SUCCESS     // Auto-negotiation complete

// --- net_config.h key options ---
// WCHNET_NUM_TCP          Number of TCP connections
// WCHNET_NUM_UDP          Number of UDP connections
// WCHNET_NUM_IPRAW        Number of IP raw sockets
// WCHNET_MAX_SOCKET_NUM   Total sockets (max 31)
// WCHNET_TCP_MSS          TCP segment size (60-1460, default 536)
// WCHNET_MEM_HEAP_SIZE    Protocol stack heap size (auto-calculated)
// WCHNET_PING_ENABLE      Enable PING (1=enable)

// --- Interrupt handler ---
// Ethernet interrupt: ETH_IRQHandler
// Must call WCHNET_ETHIsr() inside the handler
```

**Note**: The WCHNET library is pre-compiled (`CH57xNET.lib`). Applications configure `net_config.h` and link against the library. See `NET/NetLib/wchnet.h` for the full API.

---

## **[CH579]** Flash

CH579 flash API differs from CH572. CH579 uses direct flash write functions (no EEPROM/DataFlash API -- DataFlash is accessed via the same functions at the DataFlash address).

```c
// Header: CH57x_flash.h (CH579 variant)

// Unique ID and MAC
void GetUniqueID(PUINT8 buf);    // 6-byte ID + 2-byte checksum (8 bytes total)
void GetMACAddress(PUINT8 buf);  // 6-byte MAC address (for Ethernet)

// Flash operations
UINT8 FlashBlockErase(UINT32 addr);                    // Erase one block
UINT8 FlashWriteDW(UINT32 addr, UINT32 dat);           // Write one double-word (4 bytes)
UINT8 FlashWriteBuf(UINT32 addr, PUINT32 pdat, UINT16 len);  // Write buffer

// DataFlash region
// Address: 0x3E800, Size: 2KB (0x800)
// Bootloader: 0x3F000, Size: 4KB (0x1000)
```
