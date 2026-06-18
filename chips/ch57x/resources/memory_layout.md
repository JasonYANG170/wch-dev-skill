# CH57x Memory Layout Reference

## Chip Memory Map

### CH572 (RISC-V)

### Flash (Code Area)
| Region | Start Address | End Address | Size | Description |
|--------|---------------|-------------|------|-------------|
| Boot/System | 0x00000000 | 0x00000FFF | 4KB | System bootloader (ROM) |
| Application | 0x00001000 | 0x0006FFFF | 444KB | User application code |
| DataFlash | 0x00077E00 | 0x00077FFF | 512B | NV storage, calibration data |

### RAM
| Region | Start Address | End Address | Size | Description |
|--------|---------------|-------------|------|-------------|
| RAM | 0x20003800 | 0x20007FFF | 18KB | Main RAM (data, bss, stack) |

### CH579 (ARM Cortex-M0)

| Region | Start Address | End Address | Size | Description |
|--------|---------------|-------------|------|-------------|
| Application | 0x00000000 | 0x0003E7FF | 250KB | User application code |
| DataFlash | 0x0003E800 | 0x0003EFFF | 2KB | NV storage, calibration data |
| Bootloader | 0x0003F000 | 0x0003FFFF | 4KB | System bootloader (ROM) |
| RAM Bank 0 | 0x20000000 | 0x20003FFF | 16KB | Main RAM (data, bss, stack) |
| RAM Bank 1 | 0x20004000 | 0x20007FFF | 16KB | Extended RAM (NMI code, extra data) |

**Key differences from CH572**:
- CH579 has 256KB Flash total (vs CH572's 448KB)
- CH579 has 32KB RAM in two 16KB banks (vs CH572's single 18KB)
- CH579 DataFlash is 2KB at 0x3E800 (vs CH572's 512B at 0x77E00)
- CH579 bootloader is at 0x3F000 (vs CH572's 0x0000-0x0FFF)
- CH579 uses ARM Cortex-M0 (not RISC-V), so linker scripts use Keil scatter files (`.sct`)

## Standard Application Linker Script

From `resources/EXAM/SRC/Ld/Link.ld`:

```
ENTRY( _start )
__stack_size = 512;

MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 448K
    RAM (xrw) : ORIGIN = 0x20003800, LENGTH = 18K
}

SECTIONS
{
    .highcode : { *(.vector); *(.highcode); } >RAM AT>FLASH
    .text : { *(.text); *(.rodata); } >FLASH
    .data : { *(.data); *(.sdata); } >RAM AT>FLASH
    .bss : { *(.bss); *(.sbss); } >RAM
    .stack : { . = ORIGIN(RAM) + LENGTH(RAM) - __stack_size; } >RAM
}
```

Key points:
- `.highcode` section is loaded from Flash but executed from RAM (for fast interrupt handlers)
- Stack grows downward from top of RAM (512 bytes default)
- `.data` is initialized from Flash at startup

### **[CH579]** Keil Scatter File

From `CH579EVT/EVT/EXAM/SRC/sct/CH57x.sct`:

```
LR_IROM1 0x00000000 0x0003E800  {    ; Load region: 250KB application
  ER_IROM1 0x00000000 0x0003E800  {  ; Code in Flash
   *.o (RESET, +First)
   *(InRoot$$Sections)
   .ANY (+RO)
  }
  RW_IRAM1 0x20000000 0x00004000  {  ; RAM Bank 0: 16KB
   .ANY (+RW +ZI)
  }
  RW_IRAM2 0x20004000 0x00004000  {  ; RAM Bank 1: 16KB (NMI code)
   *.o (NMICode)
   .ANY (+RW +ZI)
  }
}
```

Key points:
- CH579 uses Keil scatter files (`.sct`) instead of GCC linker scripts (`.ld`)
- RAM is split into two 16KB banks: Bank 0 for general data, Bank 1 for NMI (Non-Maskable Interrupt) code and extra data
- The `NMICode` section in Bank 1 ensures NMI handlers are in a separate RAM bank

## IAP Memory Layout

### **[CH572]** Bootloader (UART_IAP / USB_IAP)
```
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 4K    // Bootloader at start
    RAM (xrw) : ORIGIN = 0x20003800, LENGTH = 18K
}
```

### **[CH572]** Application (with IAP)
```
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00001000, LENGTH = 444K  // App starts after bootloader
    RAM (xrw) : ORIGIN = 0x20003800, LENGTH = 18K
}
```

### **[CH572]** OTA Dual-Image Layout

```
Address     Size      Region
0x00000000  12KB      IAP Bootloader
0x00003000  ---       (reserved)
0x00001000  216KB     Image A (running application)
0x00039000  216KB     Image B (backup / OTA target)
0x0006D000  12KB      IAP image (for jump-to-IAP)
0x00070000  ---       Application code end
0x00077E00  512B      DataFlash (image flags, NV)
```

Image Flag values in DataFlash:
- `0x01`: Image A is active
- `0x02`: Image B is active
- `0x03`: IAP image is active

### **[CH579]** IAP Layout

CH579 bootloader is at the top of Flash (0x3F000), not at the bottom. Applications start at 0x00000000.

```
Address     Size      Region
0x00000000  250KB     Application code
0x0003E800  2KB       DataFlash (NV storage)
0x0003F000  4KB       Bootloader (ROM)
```

## Section Placement Guidelines

### `.highcode` Section
Use for time-critical code that must execute from RAM:
- Interrupt handlers (`__attribute__((section(".highcode")))`)
- Flash erase/write routines
- BLE stack critical paths

### `.text` Section
Normal code execution from Flash:
- Application logic
- Peripheral drivers
- Utility functions

### `.data` Section
Initialized global/static variables:
- Stored in Flash, copied to RAM at startup
- Keep small to save Flash space

### `.bss` Section
Zero-initialized global/static variables:
- No Flash storage needed
- Zeroed at startup

## BLE Stack Memory

The BLE stack requires a dedicated heap (`MEM_BUF`):
```c
// In config.h
#define BLE_MEMHEAP_SIZE    (1024*6)   // 6KB BLE heap (CH572 default)

// In application
static uint8_t MEM_BUF[BLE_MEMHEAP_SIZE * 4];  // Allocated in .bss
```

**[CH579]** default BLE heap is 8KB (`BLE_MEMHEAP_SIZE = 1024*8`), larger than CH572's 6KB, because CH579 has more RAM available (32KB vs 18KB).

BLE heap usage depends on:
- Number of connections
- ATT MTU size
- Number of GATT services/characteristics
- Buffer count

## Flash Sector Operations

Flash operates on 256-byte sectors:
- **Read**: Any address, any length
- **Write**: Must be 256-byte aligned, writes full sector
- **Erase**: Must be 256-byte aligned, erases full sector

DataFlash (EEPROM) operates similarly but in a protected region.

**[CH579]** flash API uses different function names (`FlashBlockErase`, `FlashWriteDW`, `FlashWriteBuf`) instead of `FLASH_ROM_ERASE`/`FLASH_ROM_WRITE`.

## Unique ID

**[CH572]**: 8-byte unique ID (6-byte ID + 2-byte checksum):
```c
uint8_t uid[8];
GET_UNIQUE_ID(uid);
```

**[CH579]**: 8-byte unique ID (6-byte ID + 2-byte checksum) via `GetUniqueID()`:
```c
uint8_t uid[8];
GetUniqueID(uid);   // CH579 uses different function name
```

**[CH579]** also provides Ethernet MAC address:
```c
uint8_t mac[6];
GetMACAddress(mac);  // Reads chip MAC for Ethernet use
```
