# Recipe: USB Power Delivery (USB-PD)

## Overview

Implement USB Power Delivery (USB-PD) Sink (SNK) and Source (SRC) roles on CH32M030 using the built-in USBPD peripheral. The USB-PD protocol enables power negotiation up to 100W over USB-C connectors.

**Availability**: CH32M030 only (not available on CH32F10x or CH32F20x).

## Key Concepts

- USB-PD uses the CC (Configuration Channel) pins for communication, not the D+/D- lines
- CH32M030 has two USBPD instances: USBPD0 and USBPD1
- SNK role: Device that receives power (like a phone charging)
- SRC role: Device that provides power (like a charger)
- PD negotiation involves PDO (Power Data Objects) exchanged between SNK and SRC

## USBPD Instances

| Instance | Description | Notes |
|----------|-------------|-------|
| USBPD0 | USB-PD port 0 | Available on CH32M030C8T and CH32M030G8R |
| USBPD1 | USB-PD port 1 | Available on all CH32M030 variants |

**Note**: CH32M030G8R only has USBPD1. CH32M030C8T has both USBPD0 and USBPD1.

## Configuration (PD_Process.h)

Select the USBPD instance in `PD_Process.h`:

```c
#define PD_SEL  PD0   // or PD1
```

## Example: USB-PD Sink (SNK)

The SNK example requests power from a USB-PD Source (charger):

```c
#include "debug.h"
#include "PD_Process.h"

volatile UINT8 Tim_Ms_Cnt = 0x00;

// Timer1 for PD timing (1ms period)
void TIM1_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_TIM1, ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0x00;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    NVIC_SetPriority(TIM1_UP_IRQn, 3);
    NVIC_EnableIRQ(TIM1_UP_IRQn);
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
}

void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM1_UP_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) {
        Tim_Ms_Cnt++;
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
}

int main(void)
{
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(921600);
    printf("PD SNK TEST\r\n");

#if(PD_SEL == PD1)
    Delay_Ms(3);
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SDI_Disable, ENABLE);
#endif

    PD_Init();
    TIM1_Init(999, 48 - 1);  // 1ms timing

    while(1)
    {
        // Calculate timing delta
        TIM_ITConfig(TIM1, TIM_IT_Update, DISABLE);
        Tmr_Ms_Dlt = Tim_Ms_Cnt - Tmr_Ms_Cnt_Last;
        Tmr_Ms_Cnt_Last = Tim_Ms_Cnt;
        TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

        // PD detection and processing
        PD_Ctl.Det_Timer += Tmr_Ms_Dlt;
        if(PD_Ctl.Det_Timer > 4) {
            PD_Ctl.Det_Timer = 0;
            PD_Det_Proc();    // Detection process
        }
        PD_Main_Proc();       // Main PD state machine
    }
}
```

## USB-PD Key Functions (PD_Process.c)

The PD_Process module provides:

```c
void PD_Init(void);            // Initialize USBPD peripheral
void PD_Det_Proc(void);        // PD detection and connection process
void PD_Main_Proc(void);       // Main PD state machine (call in main loop)
void PDO_Request(uint8_t index); // Request specific PDO voltage
```

## PD SNK Notes

- The CH32M030C8T and CH32M030G8R do NOT have an integrated 5.1K pull-down on the CC pin. External 5.1K pull-down resistors are required on CC1 and CC2.
- `CC_PD` register bit differentiates SNK (write 1) from SRC (write 0) mode.
- Modify `PDO_Request(PDO_INDEX_1)` to change the requested voltage level.
- Always monitor Vbus voltage to detect cable removal.

## PD SRC Notes

The Source example provides power to connected Sink devices:

```c
// Similar structure to SNK, but configured for source mode
// PD_Init() configures the peripheral for SRC role
// The source advertises its PDO capabilities
// Responds to SNK requests with appropriate voltage
```

## Pitfalls

- **External pull-down**: CH32M030 does NOT have internal CC pull-down. You MUST add external 5.1K resistors on CC pins for SNK mode.
- **Voltage safety**: When changing PDO request voltage, be aware of the actual voltage being requested. Incorrect voltage can damage connected devices.
- **Power sequencing**: Ensure the board is not powered before connecting. The PD negotiation happens at connection time.
- **CC pin selection**: USBPD0 and USBPD1 use different CC pins. Select the correct instance in PD_Process.h.
- **Timer precision**: The PD protocol requires precise timing. Use TIM1 with 1ms period as shown.
- **GPIO remap for USBPD1**: If using USBPD1, you may need to disable SDI remap: `GPIO_PinRemapConfig(GPIO_Remap_SDI_Disable, ENABLE)`.
- **PD protocol version**: The examples implement PD 2.0/3.0 basic negotiation. For full PD 3.0+ features (PPS, AVS), additional implementation is needed.

## Related Examples

- `CH32M030EVT/EVT/EXAM/USBPD/USBPD_SNK` - USB-PD Sink (power consumer)
- `CH32M030EVT/EVT/EXAM/USBPD/USBPD_SRC` - USB-PD Source (power provider)
