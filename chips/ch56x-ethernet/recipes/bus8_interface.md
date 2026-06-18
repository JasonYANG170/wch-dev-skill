# Recipe: 8-Bit Bus Interface (BUS8)

## Overview

Use the BUS8 (External Bus) peripheral to interface with external devices via an
8-bit parallel bus on CH56x chips. The BUS8 controller maps external device registers
into the CPU memory space, allowing direct read/write access via pointer dereference.
This is commonly used to interface with USB host controllers (e.g., CH372), external
peripherals, or FPGA registers.

## Memory Mapping

The BUS8 controller maps external device registers into the memory range starting
at `0x80000000`:

| Address       | Description          |
|---------------|----------------------|
| `0x80000000`  | External data port   |
| `0x80000001`  | External command port (with address line A0) |

The address lines (A0, A1, etc.) select the register within the external device.

## API Function

```c
// Initialize BUS8
// addr_lines: number of address lines (ADDR_0 to ADDR_7)
// data_width: data bus width (WIDTH_8 or WIDTH_16)
// hold_time:  data hold time (HOLD_0 to HOLD_3)
// setup_time: address setup time (SETUP_0 to SETUP_3)
void BUS8_Init(UINT8 addr_lines, UINT8 data_width, UINT8 hold_time, UINT8 setup_time);
```

## Parameters

### Address Lines

| Constant  | Lines | Addressable Registers |
|-----------|-------|-----------------------|
| `ADDR_0`  | 0     | 1                     |
| `ADDR_1`  | 1     | 2                     |
| `ADDR_2`  | 2     | 4                     |
| `ADDR_3`  | 3     | 8                     |
| `ADDR_4`  | 4     | 16                    |
| `ADDR_5`  | 5     | 32                    |
| `ADDR_6`  | 6     | 64                    |
| `ADDR_7`  | 7     | 128                   |

### Data Width

| Constant   | Description |
|------------|-------------|
| `WIDTH_8`  | 8-bit bus   |
| `WIDTH_16` | 16-bit bus  |

### Timing

| Constant   | Description              |
|------------|--------------------------|
| `HOLD_0`   | Hold time: 0 cycles      |
| `HOLD_1`   | Hold time: 1 cycle       |
| `HOLD_2`   | Hold time: 2 cycles      |
| `HOLD_3`   | Hold time: 3 cycles      |
| `SETUP_0`  | Setup time: 0 cycles     |
| `SETUP_1`  | Setup time: 1 cycle      |
| `SETUP_2`  | Setup time: 2 cycles     |
| `SETUP_3`  | Setup time: 3 cycles     |

## Complete Example: Read External Device

```c
#include "CH56x_common.h"

#define FREQ_SYS  30000000

// Define memory-mapped access to external bus
// With 1 address line (ADDR_1), A0 selects between two registers
#define R8_xbus_cmd  (*((PUINT8V)0x80000001))  // Command register (A0=1)
#define R8_xbus_dat  (*((PUINT8V)0x80000000))  // Data register (A0=0)

void DebugInit(UINT32 baudrate)
{
    UINT32 x, t = FREQ_SYS;
    x = 10 * t * 2 / 16 / baudrate;
    x = (x + 5) / 10;
    R8_UART3_DIV = 1;
    R16_UART3_DL = x;
    R8_UART3_FCR = RB_FCR_FIFO_TRIG | RB_FCR_TX_FIFO_CLR |
                   RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN;
    R8_UART3_LCR = RB_LCR_WORD_SZ;
    R8_UART3_IER = RB_IER_TXD_EN;
    R32_PB_SMT |= (1 << 4) | (1 << 3);
    R32_PB_DIR |= (1 << 4);
}

int main(void)
{
    SystemInit(FREQ_SYS);
    Delay_Init(FREQ_SYS);
    DebugInit(115200);

    printf("BUS8 Example\r\n");

    // Initialize BUS8:
    // - 6 address lines (ADDR_6)
    // - 16-bit data width
    // - 3 cycle hold time
    // - 3 cycle setup time
    BUS8_Init(ADDR_6, WIDTH_16, HOLD_3, SETUP_3);

    // Write command to external device
    R8_xbus_cmd = 0x01;

    // Read data from external device
    UINT8 ver = R8_xbus_dat;
    printf("Device version: 0x%02X\r\n", ver);

    while(1);
}
```

## Complete Example: CH372 USB Host Controller

The BUS8 interface is commonly used with the CH372 USB host controller:

```c
#include "CH56x_common.h"

#define R8_CH372_CMD   (*((PUINT8V)0x80000001))
#define R8_CH372_DAT   (*((PUINT8V)0x80000000))

// CH372 commands
#define CH372_GET_IC_VER    0x01
#define CH372_SET_USB_MODE   0x15
#define CH372_GET_STATUS     0x22

void CH372_WriteCmd(UINT8 cmd)
{
    R8_CH372_CMD = cmd;
    mDelayuS(2);  // Command setup time
}

void CH372_WriteData(UINT8 dat)
{
    R8_CH372_DAT = dat;
    mDelayuS(1);
}

UINT8 CH372_ReadData(void)
{
    return R8_CH372_DAT;
}

UINT8 CH372_GetVersion(void)
{
    CH372_WriteCmd(CH372_GET_IC_VER);
    return CH372_ReadData();
}

int main(void)
{
    SystemInit(CLK_SOURCE_PLL_60MHz);
    Delay_Init(GetSysClock());

    BUS8_Init(ADDR_1, WIDTH_8, HOLD_2, SETUP_2);

    UINT8 ver = CH372_GetVersion();
    printf("CH372 version: 0x%02X\r\n", ver);

    while(1);
}
```

## GPIO Pin Assignment

The BUS8 controller uses dedicated pins for data bus (D0-D7), address lines (A0-A7),
and control signals (CS, RD, WR). These pins are configured automatically by
`BUS8_Init()`. Check the chip datasheet for the specific pin mapping.

## Notes

- The external bus occupies the memory region starting at `0x80000000`.
- Access width (8-bit or 16-bit) must match the `data_width` parameter.
- Timing parameters (hold, setup) may need adjustment based on the external device's
  speed requirements. Start with maximum values and optimize if needed.
- The BUS8 peripheral clock must be enabled if peripheral clock gating is used.
- The `PUINT8V` type is used for volatile pointer access to ensure the compiler
  does not optimize away the bus access.

## Example Project

`CH569EVT/EVT/EXAM/BUS8/` - External bus interface demo with CH372.
