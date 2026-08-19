# WCH Development Workflow

Use this reference when writing, porting, or reviewing WCH MCU firmware.

## Evidence Order

1. User project files: headers, startup files, linker scripts, project settings, and existing examples.
2. Exact chip-family source and headers under `chips/<family>/resources/`.
3. Closest WCH `EXAM` example for the same chip, peripheral, and toolchain.
4. Family recipe and API reference.
5. Generic embedded knowledge only for architecture-neutral reasoning.

If these sources disagree, follow the files that match the user's selected chip and SDK/EVT package. Mention the mismatch instead of silently blending examples.

## Code Generation Rules

- Start from a matching example structure whenever possible.
- Verify peripheral clock, GPIO alternate function, interrupt vector name, and linker/startup file against the selected family.
- Keep RISC-V, ARM Cortex-M, ARM7TDMI, and 8051 interrupt syntax separate.
- Do not assume flash page size, USB endpoint limits, BLE heap size, or bootloader offset across families.
- For BLE, copy the stack initialization order and memory configuration from the closest working example, then adjust only the required application layer.

## Minimum Answer Shape

When generating code, include:

- Target chip and family assumed.
- Source files or examples used as evidence.
- Required config changes (`config.h`, linker script, project include paths, or toolchain setting).
- Limits or unverified items that still need checking on hardware.
