# IAP (In-Application Programming) Bootloader

> **Summary**: Implement IAP bootloader and firmware update on CH5xx 8051 chips -- jump to bootloader, user application design, and ISP programming.

## Trigger Intent

- "IAP bootloader"
- "Firmware update"
- "In-application programming"
- "Jump to bootloader"
- "ISP programming"
- "OTA update"

## IAP Memory Layout

### CH554 (16KB Flash)

```
0x0000 - 0x37FF : User Application (14KB)
0x3800 - 0x3FFF : Bootloader (2KB) -- ISP built-in
```

### CH559 (64KB Flash)

```
0x0000 - 0xEFFF : User Application (60KB)
0xF000 - 0xFFFF : Bootloader (4KB) -- ISP built-in
```

### CH549 (32KB Flash)

```
0x0000 - 0x6FFF : User Application (28KB)
0x7000 - 0x7FFF : Bootloader (4KB) -- ISP built-in
```

> **Note**: WCH 8051 chips include a built-in ISP bootloader in the upper Flash region. The user application occupies the lower region.

## Step-by-Step

### 1. Application Jump to Bootloader

```c
#include "..\Public\CH554.H"
#include "..\Public\Debug.H"

// Bootloader address (chip-specific)
#define BOOT_ADDR  0x3800   // CH554

// Function pointer type for jump
typedef void (*pTaskFn)(void);

void JumpToBoot(void) {
    pTaskFn boot_entry;

    EA = 0;                  // Disable all interrupts (REQUIRED)
    boot_entry = (pTaskFn)BOOT_ADDR;
    mDelaymS(100);           // Wait for pending operations
    boot_entry();            // Jump to bootloader -- never returns
}
```

### 2. Trigger Condition (GPIO Pin)

```c
sbit EnableIAP = P1^6;     // Pin to trigger IAP entry

void main() {
    CfgFsys();
    mDelaymS(5);

    // Run user application (blink LED on P1.7)
    P1_MOD_OC &= ~bSCK;    // P1.7 = push-pull output
    P1_DIR_PU |= bSCK;

    while (1) {
        SCK = ~SCK;         // Toggle LED
        mDelaymS(50);

        // Check IAP trigger pin (active low)
        if (EnableIAP == 0) {
            break;           // Exit loop, jump to bootloader
        }
    }

    // Jump to ISP bootloader
    EA = 0;
    JumpToBoot();
    while (1);               // Should never reach here
}
```

### 3. Software Trigger (Command-Based)

```c
// Jump to bootloader on receiving specific UART command
void CheckForUpdate(void) {
    if (RI) {                // UART0 data received
        UINT8 cmd = SBUF;
        RI = 0;
        if (cmd == 0x55) {   // Update command byte
            printf("Entering IAP mode...\n");
            mDelaymS(100);
            JumpToBoot();
        }
    }
}
```

### 4. IAP User Application (CH559)

```c
// CH559 IAP user application example
#include "..\Public\CH559.H"
#include "..\Public\DEBUG.H"

#define BOOT_ADDR  0xF000   // CH559 bootloader address

typedef void (*pTaskFn)(void);

sbit EnableIAP = P1^6;

void main() {
    CfgFsys();
    mDelaymS(5);
    mInitSTDIO();
    printf("CH559 User App v1.0\n");

    // P1.7 = status LED
    P1_MOD_OC &= ~(1 << 7);
    P1_DIR_PU |= (1 << 7);

    // P1.6 = IAP trigger (input with pull-up)
    P1_MOD_OC &= ~(1 << 6);
    P1_DIR_PU |= (1 << 6);

    while (1) {
        P1_7 = !P1_7;       // Blink LED
        mDelaymS(200);

        if (EnableIAP == 0) {
            printf("Jumping to bootloader...\n");
            EA = 0;
            ((pTaskFn)BOOT_ADDR)();
        }
    }
}
```

### 5. Flash Read/Write for Data Storage

```c
// CH554: Write data to Flash (must erase first)
// Flash is organized in 512-byte blocks

// Erase a Flash block (sets all bytes to 0xFF)
void Flash_EraseBlock(UINT16 addr) {
    ROM_ADDR = addr;
    ROM_CTRL = 0xA5;        // Trigger erase
    while (ROM_CTRL & 0x80); // Wait for completion
}

// Write a byte to Flash (must be erased first)
void Flash_WriteByte(UINT16 addr, UINT8 dat) {
    ROM_ADDR = addr;
    ROM_DATA = dat;
    ROM_CTRL = 0xA2;        // Trigger write
    while (ROM_CTRL & 0x80);
}

// Read a byte from Flash
UINT8 Flash_ReadByte(UINT16 addr) {
    ROM_ADDR = addr;
    ROM_CTRL = 0xA1;        // Trigger read
    return ROM_DATA;
}
```

### 6. Full IAP Application Example (CH554)

```c
#include "..\Public\CH554.H"
#include "..\Public\Debug.H"
#include "stdio.h"

#pragma NOAREGS

sbit EnableIAP = P1^6;
#define BOOT_ADDR 0x3800

typedef void (*pTaskFn)(void);

void main() {
    UINT16 count = 0;

    CfgFsys();
    mDelaymS(5);
    mInitSTDIO();

    // LED on P1.7 (SCK pin)
    P1_MOD_OC &= ~bSCK;
    P1_DIR_PU |= bSCK;

    // IAP trigger on P1.6 (input with pull-up)
    P1_MOD_OC &= ~(1 << 6);
    P1_DIR_PU |= (1 << 6);

    printf("Application started\n");
    printf("Pull P1.6 low to enter IAP mode\n");

    while (1) {
        SCK = ~SCK;         // Blink LED
        mDelaymS(100);

        if (EnableIAP == 0) {
            printf("IAP trigger detected\n");
            printf("Jumping to bootloader at 0x%04X\n", (UINT16)BOOT_ADDR);
            mDelaymS(50);

            EA = 0;          // MUST disable interrupts
            ((pTaskFn)BOOT_ADDR)();
        }
    }
}
```

## ISP Programming Workflow

1. **Hardware setup**: Connect USB or UART to chip
2. **Enter ISP mode**: Pull BOOT pin low during reset, or use IAP trigger
3. **Flash firmware**: Use WCH ISP tool to program .hex file
4. **Verify**: Read back Flash contents to confirm
5. **Reset**: Power cycle or reset to run new firmware

## Application Project Configuration

When creating an IAP user application, configure the Keil C51 project:

```
Options for Target -> Memory Model -> Small
Options for Target -> Code ROM Size -> Small: 2KB (or as needed)
Options for Target -> Off-chip Code memory:
  Start: 0x0000   Length: 0x3800   (CH554, leave room for bootloader)
```

## Common Errors

| Error | Cause | Fix |
|---|---|---|
| Jump crashes | Interrupts not disabled | Set `EA = 0` before jump |
| App won't start | Code overlaps bootloader | Set code size limit below bootloader address |
| Flash write fails | Block not erased | Erase before write (`ROM_CTRL = 0xA5`) |
| LED stays off | Bootloader took over | Verify app starts at 0x0000, not bootloader region |
| ISP won't connect | Wrong baud rate | Use 57600 or 115200 for ISP |
| App erased by ISP | Full chip erase | Use "program only" mode, not "erase + program" |
