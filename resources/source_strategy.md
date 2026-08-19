# WCH Source Strategy

The bundled examples are useful because WCH APIs are often chip-family-specific and not always documented in one place. Keep them available, but search them narrowly.

## Search Narrowly

- First choose `chips/<family>/`.
- Prefer headers, startup files, linker scripts, and the closest `EXAM` project.
- Exclude build outputs, object files, IDE metadata, binaries, and unrelated family folders from manual inspection.
- When looking for a symbol, search exact case first, then known prefix variants such as `GPIO_`, `RCC_`, `PFIC_`, `USB`, `BLE`, or family-specific register names.

## Avoid Cross-Family Leakage

Do not copy code across these boundaries unless a matched header/example proves it:

- CH57x/CH58x/CH59x BLE stack code versus CH32 BLE-enabled parts.
- CH32V RISC-V interrupt attributes versus CH32F ARM vector handlers.
- CH561/CH563 ARM7 SFR style versus CH32 register-library style.
- CH5xx 8051 keywords and memory model versus 32-bit families.

## Useful Proof Before Emitting Code

For each nontrivial peripheral example, confirm:

- Header containing the function or register.
- Example project showing init order.
- Required clock/reset call.
- Interrupt handler spelling and startup vector entry.
- Any size or alignment constant used by flash, USB, BLE, DMA, or IAP.
