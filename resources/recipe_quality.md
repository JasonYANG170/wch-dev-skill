# WCH Recipe Quality Standard

Use this standard when creating or revising WCH recipes. The goal is to make generated firmware compile against the selected family instead of looking generally plausible.

## Required Metadata

Every recipe should state near the top:

- Applicable chips and excluded chips.
- Tested or source-matched SDK/EVT package when known.
- Toolchain and project type.
- Required headers, startup file, linker script, and example path.
- Validation level: `compiled`, `source-matched`, `example-derived`, or `draft`.

## Required Implementation Evidence

For each nontrivial code block, cite or name:

- Header/source file containing each API or register.
- Closest example project.
- Required peripheral clock/reset call.
- GPIO/pin mux setup.
- Interrupt vector spelling and handler attribute, if interrupts are used.
- Flash/USB/BLE/DMA alignment or buffer limits, if relevant.

## Review Checklist

- No mixed headers across WCH families.
- No RISC-V interrupt attributes in ARM/8051 examples.
- No assumed flash page size without family evidence.
- No BLE heap or MTU claims without matching stack/config evidence.
- New files, linker offsets, and project include paths are listed when needed.

## Optional Symbol Scan

Run `scripts/check_recipe_symbols.py --scope-glob "chips/<family>/recipes/*.md"` when revising a family, or `--recipe-glob "chips/<family>/recipes/<recipe>.md"` for one file. Treat findings as review leads; examples may define helper functions that are intentionally local to a recipe.
