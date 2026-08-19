---
name: wch-dev-skill
description: >-
  Use for WCH / Nanjing Qinheng MCU firmware work, including CH32, CH57, CH58,
  CH59, CH56, CH561/CH563, CH32H, CH32X, and CH5xx devices. Routes development
  to local chip-family recipes, headers, API references, and EVT examples for
  RISC-V, ARM Cortex-M, ARM7TDMI, and 8051 WCH projects.
license: MIT
metadata:
  author: Community
  version: "2.1.0"
  compatibility: Build requires MounRiver Studio, Keil MDK/C51, or the toolchain used by the selected WCH family.
  tags:
    - embedded
    - WCH
    - RISC-V
    - ARM
    - 8051
    - BLE
    - USB-PD
    - Ethernet
    - firmware
---

# wch-dev-skill

Use this skill when developing, porting, debugging, or reviewing WCH MCU firmware. The entrypoint is intentionally short: route first, then load only the family and scenario references needed for the exact chip.

## Route First

1. Identify the exact chip marking, board, SDK/EVT package, toolchain, and requested peripheral or protocol.
2. Read `resources/chip_matrix.md` to choose `chips/<family>/`.
3. Read `resources/scenario_routing.md` if the user asks for a feature or peripheral and the correct recipe is not obvious.
4. For implementation, read the selected family recipe under `chips/<family>/recipes/` and the closest `chips/<family>/resources/EXAM/` example.
5. For API details, read the selected family's `chips/<family>/resources/peripheral_api.md`, `pitfalls.md`, `memory_layout.md`, or BLE/config references as needed.

## Evidence Order

Before emitting code or build steps, verify names in this order:

1. User's existing project files.
2. Exact family headers/source and startup/linker files.
3. Closest EVT/example project under `chips/<family>/resources/EXAM/`.
4. Family recipe and API reference.
5. Generic embedded knowledge only for architecture-neutral reasoning.

If sources conflict, follow the files matching the user's selected chip and SDK/EVT package. State the mismatch instead of blending examples.

## Coding Rules

- Do not guess APIs, register names, interrupt handlers, linker symbols, flash page sizes, USB endpoints, or BLE configuration values.
- Keep RISC-V, ARM Cortex-M, ARM7TDMI, and 8051 conventions separate.
- Enable peripheral clocks and configure GPIO/pin mux before peripheral initialization.
- Copy the closest working example structure when creating new firmware.
- For BLE, keep the stack initialization order and memory configuration aligned with the selected example. `BLE_MEMHEAP_SIZE` is project-dependent; CH57x templates commonly use 6KB, and smaller heaps require proof.
- For IAP/OTA, verify bootloader offset, application linker script, vector relocation, and erase/write granularity.

## Key References

- `resources/chip_matrix.md` - chip-to-family routing.
- `resources/scenario_routing.md` - feature-to-recipe routing.
- `resources/development_workflow.md` - shared implementation workflow.
- `resources/source_strategy.md` - how to search the large example/source tree without cross-family leakage.
- `resources/recipe_quality.md` - required metadata and evidence standard for recipes.
- `scripts/validate_mcu_skill.py` - structural and recipe-quality smoke check.
- `scripts/check_recipe_symbols.py` - optional symbol-evidence scan for a selected family or recipe.
- `chips/<family>/recipes/*.md` - scenario guides.
- `chips/<family>/resources/` - API references, pitfalls, memory layout, and example indexes.

## Stop Conditions

Ask for the exact part number when the family cannot be inferred. If an API or feature cannot be found in the selected project, headers, examples, or references, say that it is unverified for that selected chip instead of inventing a portable-looking substitute.
