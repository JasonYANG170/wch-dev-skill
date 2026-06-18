# CH57x Development Common Pitfalls

Detailed explanations of common errors with wrong/correct code examples.

## 1. BLE Initialization Order

The BLE stack requires a strict initialization sequence. Violating this order causes hard faults or undefined behavior.

```c
// ❌ WRONG — HAL_Init before BLE lib init
HAL_Init();
CH57X_BLEInit();

// ❌ WRONG — GAPRole init before HAL
CH57X_BLEInit();
GAPRole_PeripheralInit();
HAL_Init();

// ✅ CORRECT — exact order
SetSysClock(CLK_SOURCE_PLL_60MHz);
CH57X_BLEInit();          // 1. BLE library first
HAL_Init();                // 2. HAL second (depends on BLE)
GAPRole_PeripheralInit();  // 3. GAP role third
Peripheral_Init();         // 4. Application init last
Main_Circulation();        // 5. Event loop (never returns)
```

**Why**: `HAL_Init()` registers TMOS event handlers that depend on the BLE stack being initialized. `GAPRole_*Init()` registers GAP role callbacks that depend on HAL.

## 2. Main Loop Must Never Return

The BLE stack runs inside `TMOS_SystemProcess()`. If main() returns, the system halts.

```c
// ❌ WRONG — main returns after init
int main(void) {
    init_all();
    while(1) { __WFI(); }  // WFI alone doesn't process BLE events
}

// ✅ CORRECT
int main(void) {
    init_all();
    Main_Circulation();  // internally calls TMOS_SystemProcess() in a loop
}
```

**Why**: `Main_Circulation()` is the BLE event loop. It processes all BLE tasks, timers, and messages. Without it, no BLE events are handled.

## 3. BLE_MEMHEAP_SIZE Too Small

The BLE stack needs sufficient heap memory. Too little causes silent corruption or hard faults.

```c
// ❌ WRONG — too small for most applications
#define BLE_MEMHEAP_SIZE    (1024*2)  // 2KB

// ✅ CORRECT — minimum for simple peripheral
#define BLE_MEMHEAP_SIZE    (1024*4)  // 4KB

// ✅ RECOMMENDED — for most applications
#define BLE_MEMHEAP_SIZE    (1024*6)  // 6KB

// ✅ FOR MESH — needs more
#define BLE_MEMHEAP_SIZE    (1024*12) // 12KB
```

**Why**: BLE heap stores connection structures, GATT attribute tables, and packet buffers. Each connection uses ~1KB.

## 4. ATT_MTU vs BLE_BUFF_MAX_LEN

ATT_MTU is always `BLE_BUFF_MAX_LEN - 4` (4 bytes for ATT header).

```c
// ❌ WRONG — expecting MTU 23 with BUFF_MAX_LEN 23
#define BLE_BUFF_MAX_LEN    23  // actual MTU = 19!

// ✅ CORRECT — for default MTU 23
#define BLE_BUFF_MAX_LEN    27  // MTU = 27 - 4 = 23

// ✅ FOR MTU 247 (BLE 5.0 Data Length Extension)
#define BLE_BUFF_MAX_LEN    251 // MTU = 251 - 4 = 247
```

**Why**: 4 bytes are reserved for the ATT header (opcode + handle). The remaining bytes carry the actual payload.

## 5. GATT Service Registration Order

Standard services (GAP, GATT) must be registered before custom services.

```c
// ❌ WRONG — custom service first
MyService_AddService(GATT_ALL_SERVICES);
GGS_AddService(GATT_ALL_SERVICES);
GATTServApp_AddService(GATT_ALL_SERVICES);

// ✅ CORRECT — standard services first
GGS_AddService(GATT_ALL_SERVICES);           // GAP service
GATTServApp_AddService(GATT_ALL_SERVICES);   // GATT service
DevInfo_AddService();                        // Device Info
MyService_AddService(GATT_ALL_SERVICES);     // Custom last
```

**Why**: The GATT server maintains a handle-based attribute table. Standard services occupy fixed handle ranges. Custom services are assigned handles after standard services.

## 6. Interrupt Handler Section

Interrupt handlers must execute from RAM for deterministic timing.

```c
// ❌ WRONG — interrupt in Flash section
void UART1_IRQHandler(void) {
    // May have Flash access latency during interrupt
}

// ✅ CORRECT — interrupt in RAM section
__attribute__((interrupt("WCH-Interrupt-fast")))
__attribute__((section(".highcode")))
void UART1_IRQHandler(void) {
    // Executes from RAM, no Flash latency
}
```

**Why**: Flash has wait states that vary with clock speed. Interrupt handlers need deterministic timing. The `.highcode` section is copied to RAM at startup.

## 7. GPIO Mode Before Peripheral Use

Peripheral pins must have the correct GPIO mode before the peripheral is initialized.

```c
// ❌ WRONG — peripheral init without GPIO config
UART1_DefInit();  // PA8/PA9 still in default floating input mode

// ✅ CORRECT — GPIO config first
GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);       // RX: pull-up input
GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);   // TX: push-pull output
UART1_DefInit();
```

**Why**: GPIO pins default to floating input mode. UART TX needs push-pull output. UART RX needs pull-up (idle high).

## 8. Flash Sector Erase

Flash writes require erasing the entire 256-byte sector first.

```c
// ❌ WRONG — partial write without erase
uint8_t data = 0x42;
FLASH_ROM_WRITE(0x10000, &data, 1);  // Erases entire 256-byte sector!

// ✅ CORRECT — read-modify-write
uint8_t buf[256];
FLASH_ROM_READ(0x10000, buf, 256);   // Read existing data
buf[0] = 0x42;                        // Modify target byte
FLASH_ROM_ERASE(0x10000, 256);        // Erase sector
FLASH_ROM_WRITE(0x10000, buf, 256);   // Write back entire sector
```

**Why**: Flash can only change bits from 1→0 during write. To change 0→1, you must erase (sets all bits to 1). Erase operates on 256-byte sectors.

## 9. IAP Application Address

IAP applications must be linked at 0x1000, not 0x0000.

```c
// ❌ WRONG — default linker script, app starts at 0x0000
// This overwrites the bootloader!
MEMORY {
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 448K
}

// ✅ CORRECT — IAP application starts at 0x1000
MEMORY {
    FLASH (rx) : ORIGIN = 0x00001000, LENGTH = 444K
}
```

**Why**: The bootloader occupies 0x0000-0x0FFF. If the application is linked at 0x0000, it overwrites the bootloader on first flash.

## 10. Connection Handle in Notifications

Notifications require a valid connection handle from the connection event.

```c
// ❌ WRONG — hardcoded or uninitialized handle
static uint16_t connHandle = 0;
GATT_Notification(connHandle, &noti, FALSE);  // handle 0 may not be valid

// ✅ CORRECT — store handle from connection event
static uint16_t connHandle = INVALID_CONNHANDLE;

// In peripheralStateNotificationCB:
case GAPROLE_CONNECTED:
    connHandle = pEvent->linkCmpl.connectionHandle;
    break;
case GAPROLE_DISCONNECTED:
    connHandle = INVALID_CONNHANDLE;
    break;

// Before sending notification:
if (connHandle != INVALID_CONNHANDLE) {
    GATT_Notification(connHandle, &noti, FALSE);
}
```

**Why**: `INVALID_CONNHANDLE` (0xFFFF) means no active connection. Sending with an invalid handle causes the BLE stack to return an error.

## 11. TMOS Event Return Value

Event handlers must return unhandled event bits, not 0.

```c
// ❌ WRONG — always returns 0
uint16_t ProcessEvent(uint8_t task_id, uint16_t events) {
    if (events & START_DEVICE_EVT) {
        startDevice();
        return 0;  // loses SYS_EVENT_MSG and other pending events
    }
    return 0;
}

// ✅ CORRECT — XOR handled bits
uint16_t ProcessEvent(uint8_t task_id, uint16_t events) {
    if (events & SYS_EVENT_MSG) {
        uint8_t *pMsg = tmos_msg_receive(taskId);
        if (pMsg) {
            tmos_msg_deallocate(pMsg);
        }
        return (events ^ SYS_EVENT_MSG);
    }
    if (events & START_DEVICE_EVT) {
        startDevice();
        return (events ^ START_DEVICE_EVT);
    }
    return 0;
}
```

**Why**: TMOS uses a bitmask for pending events. Returning 0 discards all pending events. XOR removes only the handled event, preserving others.

## 12. ConnectNumber Encoding

The `ConnectNumber` field packs peripheral and central connection counts into a single byte.

```c
// ❌ WRONG — separate assignments
bleConfig_t cfg;
cfg.ConnectNumber = PERIPHERAL_MAX_CONNECTION;  // overwrites central count
cfg.ConnectNumber = CENTRAL_MAX_CONNECTION;     // overwrites peripheral count

// ✅ CORRECT — packed encoding
cfg.ConnectNumber = (PERIPHERAL_MAX_CONNECTION & 3) | (CENTRAL_MAX_CONNECTION << 2);
// Bits 0-1: peripheral count (max 3)
// Bits 2-7: central count (max 15)
```

**Why**: The BLE stack uses this packed format to save memory. Peripheral connections are limited to 3 (2 bits), central to 15 (6 bits).

## 13. Advertising Data Length

Advertising data must not exceed 31 bytes.

```c
// ❌ WRONG — too much data
static uint8_t advertData[] = {
    0x02, 0x01, 0x06,
    0x10, 0x09, 'V','e','r','y',' ','L','o','n','g',' ','N','a','m','e','!','!',
    0x05, 0x02, 0x0F, 0x18, 0x0A, 0x18,
    0x03, 0xFF, 0x01, 0x02, 0x03,  // total > 31 bytes!
};

// ✅ CORRECT — truncate name or use scan response
static uint8_t advertData[] = {
    0x02, 0x01, 0x06,                         // Flags: 3 bytes
    0x06, 0x09, 'C','H','5','7','3',           // Short name: 7 bytes
    0x05, 0x02, 0x0F, 0x18, 0x0A, 0x18,       // Services: 6 bytes
};  // Total: 16 bytes ✓

static uint8_t scanRspData[] = {
    0x10, 0x09, 'V','e','r','y',' ','L','o','n','g',' ','N','a','m','e','!',
};  // Long name in scan response
```

**Why**: BLE 4.0/4.2 advertising PDU has a 31-byte payload limit. Exceeding it causes the data to be silently truncated.

## 14. Custom 128-bit UUID Byte Order

BLE UUIDs are stored in little-endian byte order.

```c
// ❌ WRONG — big-endian UUID
static uint8_t myServiceUUID[ATT_UUID_SIZE] = {
    0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0,
    0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0
};

// ✅ CORRECT — little-endian UUID (reverse byte order)
static uint8_t myServiceUUID[ATT_UUID_SIZE] = {
    0xF0, 0xDE, 0xBC, 0x9A, 0x78, 0x56, 0x34, 0x12,
    0xF0, 0xDE, 0xBC, 0x9A, 0x78, 0x56, 0x34, 0x12
};
```

**Why**: The BLE specification stores UUIDs in little-endian format. The standard Bluetooth UUID base is `00000000-0000-1000-8000-00805F9B34FB`, stored as `FB 34 9B 5F 80 00 00 80 00 10 00 00 XX XX 00 00` in the attribute table.

---

## 15. **[CH579]** LCD Segment Pin Conflicts with GPIO

LCD segment pins (SEG0-SEG23) share Port B (PB0-PB23). Once LCD is enabled on a pin, that pin cannot be used as GPIO.

```c
// ❌ WRONG — using PB5 as GPIO output while LCD uses SEG5
GPIOB_ModeCfg(GPIO_Pin_5, GPIO_ModeOut_PP_5mA);  // PB5 as GPIO
LCD_DefInit();                                      // LCD takes over PB5 (SEG5)
GPIOB_SetBits(GPIO_Pin_5);                          // Does nothing -- LCD controls PB5

// ✅ CORRECT — disable digital input only on LCD pins you actually use
GPIOAGPPCfg(ENABLE, RB_PIN_SEG0_3_IE    // SEG0-3 for LCD
                  | RB_PIN_SEG4_7_IE);   // SEG4-7 for LCD
// PB8+ remain available as GPIO
LCD_DefInit();
```

**Why**: `GPIOAGPPCfg` with `RB_PIN_SEGx_y_IE` disables the digital input buffer on those pins, routing them to the LCD driver. Only enable the SEG groups you actually use -- enabling all of them consumes the entire Port B.

## 16. **[CH579]** SPI1 Limitations (No DMA, No Slave)

CH579 has two SPI peripherals. SPI1 is Master-only with no DMA support. Code written for SPI0 DMA or Slave mode cannot be ported directly to SPI1.

```c
// ❌ WRONG — using DMA on SPI1
SPI1_MasterDMATrans(buf, len);  // Function does not exist!

// ❌ WRONG — using SPI1 as slave
SPI1_SlaveInit();               // Function does not exist!

// ✅ CORRECT — use FIFO transfer on SPI1
SPI1_MasterTrans(buf, len);     // FIFO-based transfer (blocking)
SPI1_MasterRecv(buf, len);      // FIFO-based receive (blocking)

// ✅ CORRECT — use SPI0 for DMA or Slave mode
SPI0_MasterDMATrans(buf, len);  // DMA transfer (SPI0 only)
SPI0_SlaveInit();               // Slave mode (SPI0 only)
```

**Why**: SPI1 is a simplified peripheral with only Master FIFO functions. For DMA transfers or Slave mode, always use SPI0.

## 17. **[CH579]** NET Ethernet SPI Pin Configuration

CH579's Ethernet controller uses an internal SPI interface (not GPIO-mapped SPI0/SPI1). The NET peripheral is built into the chip -- no external SPI wiring is needed. However, the Ethernet PHY requires proper power and crystal configuration.

```c
// ❌ WRONG — trying to use SPI0 pins for Ethernet
SPI0_MasterDefInit();  // This is for external SPI devices, not Ethernet

// ✅ CORRECT — Ethernet uses the internal ETH peripheral
// Configure via eth_driver.c/h and WCHNET library
// No GPIO SPI pin configuration needed for Ethernet
ETH_Init();  // Initialize internal ETH peripheral + PHY
```

**Why**: Despite being described as "SPI-based" in the datasheet, the CH579's Ethernet controller uses an internal SPI bus to communicate with the integrated PHY. Application code uses the WCHNET library API, not the SPI0/SPI1 peripheral drivers.
