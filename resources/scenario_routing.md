# WCH Scenario Routing

Use this quick map after selecting the chip family in `resources/chip_matrix.md`.

## Common Firmware Tasks

| Scenario | Where to Look |
|---|---|
| New project | `chips/<family>/recipes/new_project.md` and closest `chips/<family>/resources/EXAM/` template |
| GPIO, UART, SPI, ADC, timer/PWM, flash | `chips/<family>/recipes/` first, then `chips/<family>/resources/peripheral_api.md` |
| I2C, DMA, watchdog, RTC, EXTI | Verify the recipe exists for the selected family; otherwise search the exact family examples and headers |
| IAP or OTA | `chips/<family>/recipes/iap_ota.md`, linker script, startup file, and flash layout reference |
| BLE peripheral/central/HID/Mesh/OTA | `chips/ch57x/recipes/` or `chips/ch58x-ch59x/recipes/`; copy the closest BLE example |
| USB device/host | Family USB recipe plus endpoint descriptors in `resources/EXAM/` |
| USB-PD | `chips/ch32x-usbpd/recipes/usbpd_config.md` or the matching CH32H/CH32F/CH32L example |
| Ethernet | `chips/ch56x-ethernet/`, `chips/ch561-ch563/`, `chips/ch32h-highperf/`, or matching CH32V/CH32F recipe |
| 8051 USB/TouchKey/DataFlash | `chips/ch5xx-8051/recipes/` and CH5xx EVT examples |

## Answer Discipline

For code answers, name the selected family, example path, required header, clock/reset call, interrupt/vector convention, and any memory or flash alignment requirement.
