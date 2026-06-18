# CH58x/CH59x Development Common Pitfalls

> **Chip coverage**: CH583, CH585, CH592, CH595. Sections marked **CH585** or **CH595** are chip-specific.

## 1. BLE Init Order Must Be Exact

```c
// ❌ WRONG — HAL_Init before BLE init
HAL_Init();
CH58X_BLEInit();

// ✅ CORRECT
SetSysClock(CLK_SOURCE_PLL_60MHz);
CH58X_BLEInit();
HAL_Init();
GAPRole_PeripheralInit();
Peripheral_Init();
Main_Circulation();
```

**Why**: BLE stack must initialize hardware abstraction layer resources before HAL uses them. Reversing the order causes undefined behavior in the BLE controller.

## 2. Main_Circulation Never Returns

```c
// ❌ WRONG — main() returns after init
int main(void) {
    init_all();
    // system halts here
}

// ✅ CORRECT
int main(void) {
    init_all();
    Main_Circulation();  // runs TMOS event loop forever
}
```

**Why**: `Main_Circulation()` contains `TMOS_SystemProcess()` which is the BLE event scheduler. Without it, no BLE events are processed.

## 3. config.h BLE_MEMHEAP_SIZE Must Be Large Enough

```c
// ❌ WRONG — too small
#define BLE_MEMHEAP_SIZE    (1024*2)

// ✅ CORRECT — minimum 6KB for most applications
#define BLE_MEMHEAP_SIZE    (1024*6)
// For MESH: 10KB+
// For multi-connection: 8KB+
```

**Why**: BLE stack allocates internal buffers from this heap. Too small causes silent corruption or hard fault.

## 4. ATT_MTU = BLE_BUFF_MAX_LEN - 4

```c
// ❌ WRONG — BUFF_MAX_LEN too small for desired MTU
#define BLE_BUFF_MAX_LEN    23  // actual MTU = 19

// ✅ CORRECT
#define BLE_BUFF_MAX_LEN    27  // MTU = 23 (default)
#define BLE_BUFF_MAX_LEN    251 // MTU = 247 (max)
```

**Why**: 4 bytes are reserved for ATT header. Setting BUFF_MAX_LEN=23 gives effective MTU of 19, not 23.

## 5. GATT Service Registration Order

```c
// ❌ WRONG — custom service before standard services
MyCustomService_AddService(GATT_ALL_SERVICES);
GGS_AddService(GATT_ALL_SERVICES);

// ✅ CORRECT — GAP and GATT services first
GGS_AddService(GATT_ALL_SERVICES);
GATTServApp_AddService(GATT_ALL_SERVICES);
DevInfo_AddService();
MyCustomService_AddService(GATT_ALL_SERVICES);  // custom last
```

**Why**: GATT server expects GAP and GATT services to be registered first. Custom services depend on them.

## 6. Interrupt Handler Must Use .highcode Section

```c
// ❌ WRONG — interrupt in Flash
void UART1_IRQHandler(void) { }

// ✅ CORRECT — interrupt in RAM for fast execution
__attribute__((interrupt("WCH-Interrupt-fast")))
__attribute__((section(".highcode")))
void UART1_IRQHandler(void) { }
```

**Why**: Flash access has wait states. Interrupt handlers in `.highcode` section run from RAM for deterministic timing.

## 7. GPIO Mode Must Be Set Before Peripheral Use

```c
// ❌ WRONG — using UART without GPIO config
UART1_DefInit();

// ✅ CORRECT — configure GPIO first
GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);       // UART1 RX
GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);   // UART1 TX
UART1_DefInit();
```

**Why**: GPIO pins default to floating input. Without configuring alternate function, peripheral signals cannot reach the pins.

## 8. Flash Write Erases Entire Sector

```c
// ❌ WRONG — thinking Flash can overwrite individual bytes
FLASH_ROM_WRITE(addr, &single_byte, 1);

// ✅ CORRECT — read-modify-write
uint8_t buf[256];
FLASH_ROM_READ(sector_addr, buf, 256);
buf[offset] = new_value;
FLASH_ROM_ERASE(sector_addr, 256);
FLASH_ROM_WRITE(sector_addr, buf, 256);
```

**Why**: Flash can only be written after erase. Erase granularity is 256 bytes. Partial updates require reading the entire sector first.

## 9. IAP App Must Have Different Linker Script

```c
// ❌ WRONG — default linker script
// FLASH ORIGIN = 0x00000000 — overwrites bootloader!

// ✅ CORRECT — offset to 0x1000
// FLASH (rx) : ORIGIN = 0x00001000, LENGTH = 444K
```

**Why**: Bootloader occupies 0x0000-0x0FFF. Application must not overlap or it will corrupt the bootloader.

## 10. BLE Notification Requires Valid Connection Handle

```c
// ❌ WRONG — invalid handle
GATT_Notification(0xFFFF, &noti, FALSE);

// ✅ CORRECT — use handle from connection event
static uint16_t connHandle;
// In GAP_LINK_ESTABLISHED_EVENT:
connHandle = pEvent->linkCmpl.connectionHandle;
GATT_Notification(connHandle, &noti, FALSE);
```

**Why**: 0xFFFF is `INVALID_CONNHANDLE`. Notification must reference an active connection.

## 11. TMOS Event Must Return Unhandled Bits

```c
// ❌ WRONG — returning 0 always
uint16_t ProcessEvent(uint8_t task_id, uint16_t events) {
    if (events & MY_EVT) {
        handle_my_evt();
        return 0;  // loses other pending events
    }
    return 0;
}

// ✅ CORRECT — XOR handled bits
uint16_t ProcessEvent(uint8_t task_id, uint16_t events) {
    if (events & MY_EVT) {
        handle_my_evt();
        return (events ^ MY_EVT);
    }
    return 0;
}
```

**Why**: Returning 0 tells TMOS all events were handled. If other events were pending, they are silently dropped.

## 12. Advertising Data ≤ 31 Bytes

```c
// ❌ WRONG — data too long
uint8_t advData[] = { /* ... 40 bytes ... */ };
GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advData), advData);

// ✅ CORRECT — max 31 bytes for legacy advertising
uint8_t advData[31] = { /* ... */ };
GAPRole_SetParameter(GAPROLE_ADVERT_DATA, 31, advData);
```

**Why**: BLE 4.x legacy advertising PDU is limited to 31 bytes. Excess data is silently truncated.

## 13. UUID Byte Order Is Little-Endian

```c
// ❌ WRONG — big-endian UUID
uint8_t uuid[16] = { 0x12, 0x34, 0x56, 0x78, ... };

// ✅ CORRECT — little-endian (LSB first)
uint8_t uuid[16] = { 0x78, 0x56, 0x34, 0x12, ... };
```

**Why**: BLE uses little-endian byte order for UUIDs. Reversing the byte order causes the UUID to not match.

## 14. CH58x Dual USB — Use Correct USB Instance

```c
// ❌ WRONG — using USB1 functions for USB2 peripheral
USB_DeviceInit();  // initializes USB1

// ✅ CORRECT — use USB2-specific functions
USB2_DeviceInit();     // initializes USB2
USB2_DevTransProcess();
```

**Why**: CH583/CH585 have two independent USB controllers. Each has its own set of functions. Using the wrong one silently fails.

## 15. NFCA Pin Configuration (CH585)

```c
// ❌ WRONG — forgetting NFC_CTR pin config
nfca_pcd_lib_init(&cfg);
nfca_pcd_lib_start();

// ✅ CORRECT — configure NFC_CTR antenna control pin (PA7) if used
// In nfca_pcd_ctr_init():
R32_PA_PU &= ~(GPIO_Pin_7);     // disable pull-up
R32_PA_PD_DRV |= GPIO_Pin_7;    // enable pull-down
R32_PA_DIR |= (GPIO_Pin_7);     // set as output
R32_PA_CLR = GPIO_Pin_7;         // output low
nfca_pcd_set_lp_ctrl(NFCA_PCD_LP_CTRL_0_5_VDD);
```

**Why**: NFC-A uses dedicated analog antenna pins. The optional NFC_CTR pin (PA7) controls antenna signal voltage division for wider working range. If the circuit uses NFC_CTR, it must be configured before PCD communication starts. Without it, card detection range may be severely reduced.

## 16. ENCODER Pin Analog Input Must Be Enabled (CH595)

```c
// ❌ WRONG — configuring GPIO without enabling analog input
GPIOA_ModeCfg(GPIO_Pin_10, GPIO_ModeIN_PD);
GPIOA_ModeCfg(GPIO_Pin_11, GPIO_ModeIN_PD);
ENC_Config(ENABLE, 0xFFFF, Mode_T1T2);

// ✅ CORRECT — also enable analog input for encoder pins
GPIOA_ModeCfg(GPIO_Pin_10, GPIO_ModeIN_PD);
GPIOA_ModeCfg(GPIO_Pin_11, GPIO_ModeIN_PD);
R16_PIN_ANALOG_IE |= RB_PIN_ENC;   // critical: enable analog path
ENC_Config(ENABLE, 0xFFFF, Mode_T1T2);
```

**Why**: The encoder hardware requires the analog input path to be enabled via `R16_PIN_ANALOG_IE |= RB_PIN_ENC`. Without this, the encoder signals are not routed to the decoder logic and counting will not work.

## 17. ENCODER Pin Conflict with Other Peripherals (CH595)

```c
// ❌ WRONG — using PA10/PA11 for UART0 while also using encoder
GPIOA_ModeCfg(GPIO_Pin_10, GPIO_ModeOut_PP_5mA);  // UART0 TX
UART0_DefInit();                                     // conflicts with encoder T1
ENC_Config(ENABLE, 0xFFFF, Mode_T1T2);              // encoder uses PA10/PA11

// ✅ CORRECT — check pin multiplexing before use
// PA10 = ENC_T1, PA11 = ENC_T2 (default)
// PB10 = ENC_T1, PB11 = ENC_T2 (remapped)
// Choose encoder pins that don't conflict with other peripherals
```

**Why**: ENCODER T1/T2 pins are fixed to PA10/PA11 (or PB10/PB11 when remapped). These pins may overlap with UART0 or other peripherals. Always check the pin multiplexing table before assigning pins.

## 18. KEYSCAN Power Adjustment Required Before Sleep (CH595)

```c
// ❌ WRONG — using keyscan wakeup without power adjustment
SetSysClock(SYSCLK_FREQ);
KeyScan_Cfg(ENABLE, KEYSCAN_ALL, KEYSCAN_DIV16, KEYSCAN_REP7);
KeyPress_Wake(ENABLE);
LowPower_Sleep(RB_PWR_RAM24K | RB_PWR_RAM8K);

// ✅ CORRECT — call KeyScanPowAdj() before keyscan config
KeyScanPowAdj();   // adjust ULDO voltage (+3) for keyscan in sleep
SetSysClock(SYSCLK_FREQ);
KeyScan_Cfg(ENABLE, KEYSCAN_ALL, KEYSCAN_DIV16, KEYSCAN_REP7);
KeyPress_Wake(ENABLE);
LowPower_Sleep(RB_PWR_RAM24K | RB_PWR_RAM8K | RB_XT_PRE_EN);
```

**Why**: The keyscan hardware runs from the ULDO regulator during sleep. `KeyScanPowAdj()` increases the ULDO voltage by 3 steps to ensure reliable key detection. Without it, keys may not wake the chip from sleep. Note: this adjustment affects LSI frequency.

## 19. KEYSCAN Pin Configuration Matrix (CH595)

```c
// ❌ WRONG — configuring keyscan pins as output
GPIOA_ModeCfg(GPIO_Pin_4, GPIO_ModeOut_PP_5mA);  // conflicts with keyscan
KeyScan_Cfg(ENABLE, KEYSCAN_ALL, KEYSCAN_DIV16, KEYSCAN_REP7);

// ✅ CORRECT — configure all keyscan pins as input pull-up first
GPIOA_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_PU);
GPIOB_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_PU);
KeyScan_Cfg(ENABLE, KEYSCAN_ALL, KEYSCAN_DIV16, KEYSCAN_REP7);
```

**Why**: KEYSCAN uses PA4-PA8 as matrix scan pins. These must be configured as input with pull-up before enabling the keyscan engine. If any scan pin is configured as output, the matrix scan will produce incorrect key values or miss keys entirely.

## 20. LED Controller Requires Debug Mode Disabled (CH585/CH595)

```c
// ❌ WRONG — enabling LED without disabling debug mode
ch59x_led_controller_init(CH59X_LED_OUT_MODE_SINGLE, 128);
LED_DMACfg(ENABLE, (uint32_t)&tx_MainData, (uint32_t)&tx_AuxData, 4, Mode_Els);
LED_ENABLE();

// ✅ CORRECT — disable debug mode first (LED CLK = PA15 = SWD pin)
sys_safe_access_enable();
R16_SLP_WAKE_CFG &= ~RB_DEBUG_EN;
sys_safe_access_disable();

GPIOA_ModeCfg(GPIO_Pin_15, GPIO_ModeOut_PP_5mA);  // LED CLK
GPIOA_ModeCfg(GPIO_Pin_14, GPIO_ModeOut_PP_5mA);  // LED DATA0
ch59x_led_controller_init(CH59X_LED_OUT_MODE_SINGLE, 128);
LED_DMACfg(ENABLE, (uint32_t)&tx_MainData, (uint32_t)&tx_AuxData, 4, Mode_Els);
LED_ENABLE();
```

**Why**: The LED controller clock pin (PA15) shares the SWD debug interface. Debug mode must be disabled via `R16_SLP_WAKE_CFG &= ~RB_DEBUG_EN` before the LED controller can use this pin. Without this, the SWD interface holds the pin and LED output will not work. Note: disabling debug mode prevents SWD debugging — re-enable for programming/debugging.
