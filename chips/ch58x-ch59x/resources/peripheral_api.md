# CH58x/CH59x Peripheral Driver API Quick Reference

> **Chip coverage**: CH583, CH585, CH592, CH595. Sections marked **CH585 only** or **CH595 only** are chip-specific.

## System Clock

```c
// Header: CH58x_clk.h / CH59x_clk.h

void SetSysClock(SYS_CLKTypeDef sc);
uint32_t GetSysClock(void);

// Clock sources
CLK_SOURCE_PLL_60MHz    // 60MHz PLL (default for BLE)
CLK_SOURCE_PLL_48MHz    // 48MHz PLL
CLK_SOURCE_PLL_32MHz    // 32MHz PLL
CLK_SOURCE_HSE_16MHz    // 16MHz HSE direct
CLK_SOURCE_HSI_8MHz     // 8MHz HSI (internal)

// HSE/LSE configuration
void HSECFG_Current(HSECurrentTypeDef c);
void HSECFG_Capacitance(HSECapTypeDef c);
void LSECFG_Current(LSECurrentTypeDef c);
void LSECFG_Capacitance(LSECapTypeDef c);

// 32K clock selection
void LClk32K_Select(LClk32KTypeDef hc);
// LSI_CLK_32K_Quart   — internal RC
// LSI_CLK_32K_External — external 32K crystal

// LSI calibration
void Calibration_LSI(Cali_LevelTypeDef cali_Lv);
```

## GPIO

```c
// Header: CH58x_gpio.h / CH59x_gpio.h

// Mode configuration
void GPIOA_ModeCfg(uint32_t pin, GPIOModeTypeDef mode);
void GPIOB_ModeCfg(uint32_t pin, GPIOModeTypeDef mode);

// GPIO modes
GPIO_ModeIN_Floating    // Input floating
GPIO_ModeIN_PU          // Input pull-up
GPIO_ModeIN_PD          // Input pull-down
GPIO_ModeOut_PP_5mA     // Output push-pull 5mA
GPIO_ModeOut_PP_20mA    // Output push-pull 20mA
GPIO_ModeOut_OD_5mA     // Output open-drain 5mA

// Interrupt configuration
void GPIOA_ITModeCfg(uint32_t pin, GPIOITModeTpDef mode);
void GPIOB_ITModeCfg(uint32_t pin, GPIOITModeTpDef mode);

// Interrupt modes
GPIO_ITMode_FallEdge    // Falling edge
GPIO_ITMode_RiseEdge    // Rising edge
GPIO_ITMode_LowLevel    // Low level
GPIO_ITMode_HighLevel   // High level

// Pin remapping
void GPIOPinRemap(FunctionalState s, uint16_t perph);

// Pin operations (macros)
GPIOA_SetBits(pin)
GPIOA_ResetBits(pin)
GPIOA_InverseBits(pin)
GPIOA_ReadPort()          // read all pins
GPIOA_ReadPortPin(pin)    // read specific pin
GPIOA_ClearITFlagBit(pin)

// Pin definitions
GPIO_Pin_0  through GPIO_Pin_23
```

## UART

```c
// Header: CH58x_uart.h / CH59x_uart.h
// Supports UART0, UART1, UART2, UART3

// Default init (115200 baud, 8N1)
void UART0_DefInit(void);
void UART1_DefInit(void);
void UART2_DefInit(void);
void UART3_DefInit(void);

// Baudrate configuration
void UART0_BaudRateCfg(uint32_t baudrate);
void UART1_BaudRateCfg(uint32_t baudrate);

// Interrupt configuration
void UART0_INTCfg(FunctionalState s, uint8_t i);
// i: UART_IT_RX_BYTE (receive byte ready)
//    UART_IT_TX_BYTE (transmit byte ready)
//    UART_IT_FIFO_OV (FIFO overflow)

// Byte trigger level
void UART0_ByteTrigCfg(UARTByteTRIGTypeDef b);
// UART_1ByteTrig, UART_2ByteTrig, UART_4ByteTrig, UART_7ByteTrig

// Send/Receive
void UART0_SendString(uint8_t *buf, uint16_t l);
uint16_t UART0_RecvString(uint8_t *buf);
void UART0_SendByte(uint8_t b);        // macro
uint8_t UART0_RecvByte(void);          // macro

// Status
uint8_t UART0_GetITFlag(void);         // macro
uint8_t UART0_GetLinSTA(void);         // macro
void UART0_Reset(void);

// Default debug UART: UART1 @ 115200 baud
// PA8 = RX, PA9 = TX
```

## SPI

```c
// Header: CH58x_spi.h / CH59x_spi.h
// SPI0 and SPI1

// Master mode init
void SPI0_MasterDefInit(void);
void SPI1_MasterDefInit(void);

// Clock divider
void SPI0_CLKCfg(uint8_t c);

// Data mode (CPOL/CPHA + bit order)
void SPI0_DataMode(ModeBitOrderTypeDef m);

// Byte transfer
void SPI0_MasterSendByte(uint8_t d);
uint8_t SPI0_MasterRecvByte(void);

// FIFO transfer
void SPI0_MasterTrans(uint8_t *pbuf, uint16_t len);
void SPI0_MasterRecv(uint8_t *pbuf, uint16_t len);

// DMA transfer
void SPI0_MasterDMATrans(uint8_t *pbuf, uint16_t len);
void SPI0_MasterDMARecv(uint8_t *pbuf, uint16_t len);

// Slave mode
void SPI0_SlaveInit(void);
void SPI0_SlaveSendByte(uint8_t d);
uint8_t SPI0_SlaveRecvByte(void);
void SPI0_SlaveTrans(uint8_t *pbuf, uint16_t len);
void SPI0_SlaveRecv(uint8_t *pbuf, uint16_t len);
void SPI0_SlaveDMATrans(uint8_t *pbuf, uint16_t len);
void SPI0_SlaveDMARecv(uint8_t *pbuf, uint16_t len);

// 2-wire mode
void SPI_2WIRE_MasterOutputInit(void);
void SPI_2WIRE_MasterInputInit(void);

// Interrupt/DMA config (macros)
SPI0_ITCfg(s, f)
SPI0_GetITFlag(f)
SPI0_ClearITFlag(f)
```

## I2C

```c
// Header: CH58x_i2c.h / CH59x_i2c.h

// Initialization
void I2C_Init(I2C_ModeTypeDef mode, uint32_t clockSpeed,
              I2C_DutyTypeDef duty, I2C_AckTypeDef ack,
              I2C_AckAddrTypeDef ackAddr, uint16_t ownAddr);

// Modes
I2C_Mode_I2C      // Standard I2C
I2C_Mode_SMBusHost
I2C_Mode_SMBusDevice

// Control
void I2C_Cmd(FunctionalState s);
void I2C_GenerateSTART(FunctionalState s);
void I2C_GenerateSTOP(FunctionalState s);
void I2C_AcknowledgeConfig(FunctionalState s);

// Data transfer
void I2C_SendData(uint8_t data);
uint8_t I2C_ReceiveData(void);
void I2C_Send7bitAddress(uint8_t addr, uint8_t dir);

// Status
uint8_t I2C_CheckEvent(uint32_t event);
uint32_t I2C_GetLastEvent(void);
FlagStatus I2C_GetFlagStatus(uint32_t flag);
void I2C_ClearFlag(uint32_t flag);

// Interrupt
void I2C_ITConfig(I2C_ITTypeDef it, FunctionalState s);
ITStatus I2C_GetITStatus(uint32_t it);
void I2C_ClearITPendingBit(uint32_t it);
```

## ADC

```c
// Header: CH58x_adc.h / CH59x_adc.h

// External single channel init
void ADC_ExtSingleChSampInit(ADC_SampClkTypeDef sp, ADC_SignalPGATypeDef ga);

// External differential channel init
void ADC_ExtDiffChSampInit(ADC_SampClkTypeDef sp, ADC_SignalPGATypeDef ga);

// Internal temperature sensor
void ADC_InterTSSampInit(void);

// Internal battery voltage
void ADC_InterBATSampInit(void);

// Touch key
void TouchKey_ChSampInit(void);
uint16_t TouchKey_ExcutSingleConver(uint8_t charg, uint8_t disch);

// Single conversion
uint16_t ADC_ExcutSingleConver(void);
signed short ADC_DataCalib_Rough(void);

// Channel select (macro)
ADC_ChannelCfg(ch)

// Read result (macro)
ADC_ReadConverValue()

// Temperature conversion
int adc_to_temperature_celsius(uint16_t adc_val);

// DMA config
void ADC_DMACfg(uint8_t s, uint16_t startAddr, uint16_t endAddr, ADC_DMAModeTypeDef m);
```

## Timer / PWM

```c
// Header: CH58x_timer.h / CH59x_timer.h
// TMR0, TMR1, TMR2, TMR3

// Timer init (period in system clocks)
void TMR0_TimerInit(uint32_t t);
void TMR1_TimerInit(uint32_t t);

// External counter init
void TMR0_EXTSingleCounterInit(CapModeTypeDef cap);

// PWM init
void TMR0_PWMInit(PWMX_PolarTypeDef pr, PWM_RepeatTsTypeDef ts);

// Input capture init
void TMR0_CapInit(CapModeTypeDef cap);

// DMA config (TMR1, TMR2 only)
void TMR1_DMACfg(uint8_t s, uint16_t startAddr, uint16_t endAddr, DMAModeTypeDef m);

// Timer control (macros)
TMR0_Enable()
TMR0_Disable()
TMR0_GetCurrentCount()
TMR0_PWMCycleCfg(cyc)
TMR0_PWMActDataWidth(d)
TMR0_PWMEnable()
TMR0_PWMDisable()

// Interrupt (macros)
TMR0_ITCfg(s, f)
TMR0_GetITFlag(f)
TMR0_ClearITFlag(f)
```

## PWM (Extended Channels 4-11)

```c
// Header: CH58x_pwm.h / CH59x_pwm.h

// Cycle configuration
void PWMX_CycleCfg(PWMX_CycleTypeDef cyc);
void PWMX_16bit_CycleCfg(uint16_t cyc);

// PWM output
void PWMX_ACTOUT(uint8_t ch, uint8_t da, PWMX_PolarTypeDef pr, FunctionalState s);
void PWMX_16bit_ACTOUT(uint8_t ch, uint16_t da, PWMX_PolarTypeDef pr, FunctionalState s);

// Alternate output mode
void PWMX_AlterOutCfg(uint8_t ch, FunctionalState s);

// Clock divider (macro)
PWMX_CLKCfg(d)

// Individual channel data (macros)
PWM4_ActDataWidth(d) through PWM11_ActDataWidth(d)
```

## Flash

```c
// Header: CH58x_flash.h / CH59x_flash.h

// Read flash
void FLASH_ROM_READ(uint32_t StartAddr, void *Buffer, uint32_t len);

// Write flash (erases entire 256-byte sector!)
void FLASH_ROM_WRITE(uint32_t StartAddr, void *Buffer, uint32_t len);

// Erase flash
void FLASH_ROM_ERASE(uint32_t StartAddr, uint32_t len);

// Option byte
uint8_t UserOptionByteConfig(FunctionalState resetEn, FunctionalState bootPin,
                              FunctionalState uartNoKeyEn, uint32_t flashProtSize);
uint8_t UserOptionByteClose_SWD(void);
void UserOptionByte_Active(void);

// Unique ID
void GET_UNIQUE_ID(uint8_t *Buffer);
```

## Power Management

```c
// Header: CH58x_pwr.h / CH59x_pwr.h

// DC/DC converter
void PWR_DCDCCfg(FunctionalState s);

// Unit module power control
void PWR_UnitModCfg(FunctionalState s, uint8_t unit);

// Peripheral clock gate
void PWR_PeriphClkCfg(FunctionalState s, uint16_t perph);

// Wakeup source config
void PWR_PeriphWakeUpCfg(FunctionalState s, uint8_t perph, WakeUP_ModeypeDef mode);

// Voltage monitor
void PowerMonitor(FunctionalState s, VolM_LevelypeDef vl);

// Low power modes
void LowPower_Idle(void);       // Idle mode
void LowPower_Halt(void);       // Halt mode (most peripherals off)
void LowPower_Sleep(uint8_t rm);// Sleep mode (RAM retention)
void LowPower_Shutdown(uint8_t rm); // Shutdown mode (lowest power)
```

## RTC

```c
// Header: CH58x_clk.h / CH59x_clk.h

// Time set/get
void RTC_InitTime(uint16_t y, uint16_t mon, uint16_t d, uint16_t h, uint16_t m, uint16_t s);
void RTC_GetTime(uint16_t *py, uint16_t *pmon, uint16_t *pd, uint16_t *ph, uint16_t *pm, uint16_t *ps);

// Cycle counter
void RTC_SetCycle32k(uint32_t cyc);
uint32_t RTC_GetCycle32k(void);

// Alarm/trigger
void RTC_TRIGFunCfg(uint32_t cyc);
void RTC_TMRFunCfg(RTC_TMRCycTypeDef t);

// Interrupt
uint8_t RTC_GetITFlag(RTC_EVENTTypeDef f);
void RTC_ClearITFlag(RTC_EVENTTypeDef f);
```

## USB Device

```c
// Header: CH58x_usbdev.h / CH59x_usbdev.h

// USB1 device init
void USB_DeviceInit(void);
void USB_DevTransProcess(void);

// Endpoint handlers (USB1)
void DevEP1_OUT_Deal(uint8_t l);
void DevEP1_IN_Deal(uint8_t l);
void DevEP2_OUT_Deal(uint8_t l);
void DevEP2_IN_Deal(uint8_t l);
void DevEP3_OUT_Deal(uint8_t l);
void DevEP3_IN_Deal(uint8_t l);
void DevEP4_OUT_Deal(uint8_t l);
void DevEP4_IN_Deal(uint8_t l);

// USB2 device init (CH58x only)
void USB2_DeviceInit(void);
void USB2_DevTransProcess(void);

// Endpoint handlers (USB2, CH58x only)
void U2DevEP1_OUT_Deal(uint8_t l);
void U2DevEP1_IN_Deal(uint8_t l);
```

## USB Host

```c
// Header: CH58x_usbhost.h / CH59x_usbhost.h

// USB1 host init
void USB_HostInit(void);

// Root hub control
uint8_t AnalyzeRootHub(void);
void DisableRootHubPort(void);
uint8_t EnableRootHubPort(void);
void ResetRootHubPort(void);

// Transfer
uint8_t USBHostTransact(uint8_t endp_pid, uint8_t tog, uint32_t timeout);
uint8_t HostCtrlTransfer(uint8_t *DataBuf, uint8_t *RetLen);

// Device enumeration
uint8_t InitRootDevice(void);
uint8_t CtrlGetDeviceDescr(void);
uint8_t CtrlGetConfigDescr(void);
uint8_t CtrlSetUsbAddress(uint8_t addr);
uint8_t CtrlSetUsbConfig(uint8_t cfg);

// USB2 host (CH58x only)
void USB2_HostInit(void);
uint8_t InitRootU2Device(void);
```

## LCD (CH585/CH592/CH595)

```c
// Header: CH59x_lcd.h

// LCD init
void LCD_Init(LCDDutyTypeDef duty, LCDBiasTypeDef bias);

// Duty options
LCD_Duty_1_4    // 1/4 duty
LCD_Duty_1_3    // 1/3 duty

// Bias options
LCD_Bias_1_3    // 1/3 bias
LCD_Bias_1_2    // 1/2 bias

// Data write (macros)
LCD_WriteData0(d) through LCD_WriteData10(d)

// Power control (macros)
LCD_PowerOn()
LCD_PowerDown()

// Configuration (macros)
LCD_PowerCfg(d)
LCD_ScanCLKCfg(d)
LCD_DutyCfg(d)
LCD_BiasCfg(d)
```

## NFC-A (CH585 only)

```c
// Header: CH58x_NFCA_LIB.h
// NFC-A library for PCD (reader) and PICC (card emulation)
// Pin: NFC antenna uses dedicated analog pins; NFC_CTR on PA7 (optional)

// ===================== PCD (Reader) =====================

// Init PCD library
uint8_t nfca_pcd_lib_init(nfca_pcd_config_t *cfg);

// Start/stop carrier wave
void nfca_pcd_lib_start(void);
void nfca_pcd_lib_stop(void);
void nfca_pcd_antenna_on(void);
void nfca_pcd_antenna_off(void);

// Communicate (send/receive)
uint8_t nfca_pcd_communicate(uint16_t data_bits_num, NFCA_PCD_REC_MODE_Def mode, uint8_t offset);
nfca_pcd_controller_state_t nfca_pcd_get_communicate_status(void);
uint16_t nfca_pcd_get_recv_data_len(void);
uint16_t nfca_pcd_get_recv_bits(void);

// Antenna tuning
void nfca_pcd_set_out_drv(NFCA_PCD_DRV_CTRL_Def drv);   // output level: LEVEL0-3
void nfca_pcd_set_recv_gain(NFCA_PCD_REC_GAIN_Def gain); // 12/18/24/30 dB
void nfca_pcd_set_lp_ctrl(NFCA_PCD_LP_CTRL_Def lp);     // signal detect threshold
void nfca_pcd_set_rec_threshold(NFCA_PCD_REC_THRESHOLD_Def th); // 100/150/200/250mV

// Timeout
void nfca_pcd_set_wait_ms(uint8_t ms);   // max 38ms
void nfca_pcd_set_wait_us(uint16_t us);  // max 38000us

// LPCD (Low Power Card Detection)
void nfca_pcd_lpcd_calibration(void);    // calibrate ADC baseline
uint8_t nfca_pcd_lpcd_check(void);       // returns 1 if card present

// Interrupt handler (call in NFC_IRQHandler)
void NFC_IRQLibHandler(void);

// ===================== PICC (Card Emulation) =====================

uint8_t nfca_picc_lib_init(nfca_picc_config_t *cfg);
void nfca_picc_lib_start(void);
void nfca_picc_lib_stop(void);
void nfca_picc_register_callback(nfca_picc_cb_t *cb);
uint16_t nfca_picc_tx_prepare_raw_buf(uint8_t *out, uint8_t *in, uint8_t *parity, uint16_t length, uint8_t offset);
void nfca_picc_tx_set_raw_buf(uint8_t *data, uint16_t length);
void nfca_picc_enable_rsp_rs(void);
void nfca_picc_disable_rsp_rs(void);

// PICC interrupt handlers (call in TMR0/TMR3 IRQ)
void nfca_picc_rx_irq_handler(void);
void nfca_picc_tx_irq_handler(void);

// ===================== CRYPTO1 (Mifare Classic) =====================

uint8_t nfca_pcd_crypto1_setup(nfca_crypto1_cipher_t *cipher, uint8_t *key, uint8_t *tag_uid,
                                uint8_t *tag_clg, uint8_t *tag_clg_parity, uint32_t reader_clg_rand,
                                uint8_t *reader_clg, uint8_t *reader_clg_parity,
                                uint8_t *tag_rsp, uint8_t *tag_rsp_parity);
void nfca_crypto1_encrypt(nfca_crypto1_cipher_t *cipher, uint8_t *in, uint8_t *out, uint8_t *out_parity, uint8_t len);
uint8_t nfca_crypto1_decrypt(nfca_crypto1_cipher_t *cipher, uint8_t *in, uint8_t *out, uint8_t *in_parity, uint8_t len);

// ===================== SOFT PCD (Software Decode PCD) =====================

uint8_t nfca_soft_pcd_lib_init(nfca_soft_pcd_config_t *cfg);
uint8_t nfca_soft_pcd_communicate(uint16_t data_bits_num, NFCA_PCD_REC_MODE_Def mode, uint8_t offset);
nfca_pcd_controller_state_t nfca_soft_pcd_get_communicate_status(void);
uint16_t nfca_soft_pcd_get_recv_data_len(void);
void nfca_soft_pcd_set_wait_ms(uint8_t ms);
void nfca_soft_pcd_set_wait_us(uint16_t us);
void NFCSoftPCD_IRQLibHandler(void);   // call in TMR0 IRQ
void nfca_soft_pcd_rx_irq_handler(void);
```

## ENCODER (CH595 only)

```c
// Header: CH59x_enc.h
// Rotary encoder interface — quadrature decoding with T1/T2 edge counting
// Pin: PA10 (T1) and PA11 (T2), or PB10/PB11 (remapped)
// Enable analog input: R16_PIN_ANALOG_IE |= RB_PIN_ENC

// Mode selection
typedef enum {
    Mode_IDLE = 0,   // disabled
    Mode_T2,         // T2 single-edge counting
    Mode_T1,         // T1 single-edge counting
    Mode_T1T2,       // T1+T2 quadrature counting (normal encoder)
} ENCModeTypeDef;

// Init encoder
void ENC_Config(uint8_t s, uint32_t encReg, ENCModeTypeDef m);
// s: ENABLE/DISABLE
// encReg: max count value (typically 0xFFFF)
// m: Mode_T1T2 for normal rotary encoder

// Sleep/wake
void ENC_Wake(uint8_t s);  // enable encoder wakeup from sleep

// Read status (macros)
ENC_GetCurrentDir       // 0=forward, 1=reverse
ENC_GetCurrentCount     // current counter value (R32_ENC_REG_CCNT)
ENC_GetCountandReset()  // read and reset counter

// Interrupt (macros)
ENC_ITCfg(s, f)         // enable/disable interrupt (RB_IE_DIR_INC, RB_IE_DIR_DEC)
ENC_GetITFlag(f)        // read interrupt flag
ENC_ClearITFlag(f)      // clear interrupt flag

// IRQ handler
void ENCODER_IRQHandler(void);
```

## KEYSCAN (CH595 only)

```c
// Header: CH59x_keyscan.h
// Matrix key scan engine — uses PA4-PA8 as scan pins
// Pin: PA4 (KEYSCAN_PA4), PA5, PA6, PA7, PA8 (KEYSCAN_ALL = all 5 pins)

// Pin selection (bitmask)
KEYSCAN_PA4     0x100
KEYSCAN_PA5     0x200
KEYSCAN_PA6     0x400
KEYSCAN_PA7     0x800
KEYSCAN_PA8     0x1000
KEYSCAN_ALL     0x1F00   // all 5 scan pins

// Clock division
KEYSCAN_DIV2    0x10
KEYSCAN_DIV4    0x30
KEYSCAN_DIV8    0x70
KEYSCAN_DIV16   0xF0

// Repetition (debounce)
KEYSCAN_REP1    0x02     // 1 confirmation
KEYSCAN_REP7    0x0E     // 7 confirmations (most robust)

// Init
void KeyScan_Cfg(uint8_t s, uint16_t keyScanPin, uint16_t ClkDiv, uint16_t Rep);
// Example: KeyScan_Cfg(ENABLE, KEYSCAN_ALL, KEYSCAN_DIV16, KEYSCAN_REP7);

// Wakeup from sleep
void KeyPress_Wake(uint8_t s);

// Power adjustment (call before KeyScan_Cfg if using sleep wakeup)
void KeyScanPowAdj(void);  // adjusts ULDO voltage for keyscan in sleep

// Read key value (macros)
KeyValue        // (R32_KEY_SCAN_NUMB & RB_KEY_SCAN_NUMB) — pressed key ID
KeyScan_Cnt     // (R32_KEY_SCAN_NUMB >> 20) — scan count

// Interrupt (macros)
KeyScan_ITCfg(s, f)         // RB_KEY_PRESSED_IE, RB_KEY_RELEASED_IE
KeyScan_GetITFlag(f)        // RB_KEY_PRESSED_IF, RB_KEY_RELEASED_IF
KeyScan_ClearITFlag(f)

// IRQ handler
void KEYSCAN_IRQHandler(void);
```

## LED Controller (CH585/CH595)

```c
// Header: ch59x_led.h
// DMA-driven LED output controller — supports WS2812-style addressable LEDs
// Pin: CLK=PA15, DATA0=PA14, DATA1=PA13, DATA2=PA12, DATA3=PA11

// Output mode
typedef enum {
    CH59X_LED_OUT_MODE_SINGLE = 0,  // single LED channel (DATA0 only)
    CH59X_LED_OUT_MODE_DOUBLE,      // 2 channels (DATA0, DATA1)
    CH59X_LED_OUT_MODE_FOUR,        // 4 channels (DATA0-3)
    CH59X_LED_OUT_MODE_FOUR_EXT,    // 4 channels, main+aux data
} ch59x_led_out_mode_t;

// Transfer mode
typedef enum {
    Mode_Els = 0,        // normal mode
    Mode_FourExtCHn,     // 4-channel extended mode
} LEDModeTypeDef;

// Init LED controller
void ch59x_led_controller_init(ch59x_led_out_mode_t mode, uint8_t led_clk_div);
// mode: output mode (SINGLE/DOUBLE/FOUR/FOUR_EXT)
// led_clk_div: clock divider (e.g., 128)

// Send data via DMA
void LED_DMACfg(uint8_t s, uint32_t startAddrMain, uint32_t startAddrAux, uint16_t len, LEDModeTypeDef m);
// s: ENABLE/DISABLE
// startAddrMain: main data buffer address
// startAddrAux: auxiliary data buffer address (for FOUR_EXT mode)
// len: data length in 32-bit words

// Re-send (in interrupt callback)
void ch59x_led_controller_send(uint32_t startAddrMain, uint32_t startAddrAux, uint16_t len, LEDModeTypeDef m);

// Control (macros)
LED_ENABLE()            // enable LED output
LED_DISABLE()           // disable LED output
LED_GetITFlag(f)        // RB_LED_IF_DMA_END
LED_ClearITFlag(f)

// Bit order and polarity (register direct)
// R8_LED_CTRL_MOD ^= RB_LED_BIT_ORDER;   // toggle LSB/MSB first
// R8_LED_CTRL_MOD ^= RB_LED_OUT_POLAR;   // toggle output polarity

// IRQ handler
void LED_IRQHandler(void);
```

## System

```c
// Header: CH58x_sys.h / CH59x_sys.h

// System reset
void SYS_ResetExecute(void);

// Interrupt control
void SYS_DisableAllIrq(uint32_t *pirqv);
void SYS_RecoverIrq(uint32_t irq_status);

// SysTick
uint32_t SYS_GetSysTickCnt(void);

// Delay functions
void mDelayuS(uint16_t t);
void mDelaymS(uint16_t t);

// Watchdog
void WWDG_ITCfg(FunctionalState s);
void WWDG_ResetCfg(FunctionalState s);
void WWDG_ClearFlag(void);
WWDG_SetCounter(c)    // macro
```
