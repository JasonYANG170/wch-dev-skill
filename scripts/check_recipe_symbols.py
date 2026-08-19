#!/usr/bin/env python3
"""Find recipe code symbols that are not backed by local evidence files.

This is a heuristic smoke check, not a compiler. It scans C/C++/Arduino code
blocks in recipes, extracts function-like identifiers, and checks whether each
identifier appears in the selected family/repo resources or examples.
"""

from __future__ import annotations

import argparse
import re
from pathlib import Path


CODE_FENCE = re.compile(r"^```([A-Za-z0-9_+#-]*)\s*$")
CALL = re.compile(r"\b([A-Za-z_][A-Za-z0-9_]*)\s*\(")
CODE_LANGS = {"c", "cpp", "c++", "arduino", "ino", "h", "hpp"}
TEXT_SUFFIXES = {
    ".c",
    ".cc",
    ".cpp",
    ".h",
    ".hpp",
    ".hh",
    ".ino",
    ".md",
    ".txt",
    ".cmake",
    ".yml",
    ".yaml",
    ".ld",
    ".s",
    ".S",
}
IGNORE_SYMBOLS = {
    "if",
    "for",
    "while",
    "switch",
    "return",
    "sizeof",
    "defined",
    "main",
    "printf",
    "sprintf",
    "snprintf",
    "memset",
    "memcpy",
    "memcmp",
    "strlen",
    "strcmp",
    "strcpy",
    "strerror",
    "malloc",
    "free",
    "delay",
    "millis",
    "micros",
    "pinMode",
    "digitalWrite",
    "digitalRead",
    "attachInterrupt",
    "detachInterrupt",
    "noInterrupts",
    "interrupts",
    "isr",
    "vTaskDelay",
    "vTaskDelete",
    "xTaskCreate",
    "xQueueReceive",
    "xQueueSend",
    "xSemaphoreGive",
    "xSemaphoreTake",
    "pdMS_TO_TICKS",
    "portYIELD_FROM_ISR",
    "configTime",
    "PSTR",
    "yield",
}
USER_SYMBOL_PATTERNS = [
    re.compile(r"^my_", re.I),
    re.compile(r"^handle_", re.I),
    re.compile(r"^on[A-Z]"),
    re.compile(r".*[Cc]allback$"),
    re.compile(r".*_task$"),
]


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="replace")


def recipe_scope(root: Path, recipe: Path) -> Path:
    rel = recipe.relative_to(root).parts
    if root.name == "esp-dev-skill" and len(rel) > 2 and rel[0] == "repos":
        return root / "repos" / rel[1]
    if root.name in {"wch-dev-skill", "chipintelli-dev-skill"} and len(rel) > 2 and rel[0] == "chips":
        return root / "chips" / rel[1]
    return root


def iter_code_blocks(text: str) -> list[str]:
    blocks: list[str] = []
    in_block = False
    keep = False
    current: list[str] = []
    for line in text.splitlines():
        match = CODE_FENCE.match(line)
        if match:
            if in_block:
                if keep and current:
                    blocks.append("\n".join(current))
                in_block = False
                keep = False
                current = []
                continue
            lang = match.group(1).lower()
            in_block = True
            keep = lang in CODE_LANGS
            current = []
            continue
        if in_block and keep:
            current.append(line)
    return blocks


def recipe_defined_symbols(block: str) -> set[str]:
    defined: set[str] = set()
    definition = re.compile(
        r"^\s*(?:static\s+|inline\s+|extern\s+|IRAM_ATTR\s+|void\s+|int\s+|bool\s+|"
        r"uint\d+_t\s+|int\d+_t\s+|esp_err_t\s+|BaseType_t\s+|[\w:<>&*]+\s+)+"
        r"([A-Za-z_][A-Za-z0-9_]*)\s*\([^;]*\)\s*\{",
        re.M,
    )
    for match in definition.finditer(block):
        defined.add(match.group(1))
    return defined


def is_user_symbol(symbol: str) -> bool:
    return any(pattern.match(symbol) for pattern in USER_SYMBOL_PATTERNS)


def is_function_definition(block: str, match: re.Match[str]) -> bool:
    line_start = block.rfind("\n", 0, match.start(1)) + 1
    prefix = block[line_start : match.start(1)].strip()
    if not prefix or prefix.endswith(("=", ".", "->", "::")):
        return False
    tail = block[match.end() : match.end() + 300]
    before_semicolon = tail.split(";", 1)[0]
    return "{" in before_semicolon


def extract_symbols(recipe: Path) -> set[str]:
    symbols: set[str] = set()
    blocks = iter_code_blocks(read_text(recipe))
    defined_in_recipe: set[str] = set()
    for block in blocks:
        defined_in_recipe.update(recipe_defined_symbols(block))
    for block in blocks:
        block = re.sub(r"//.*", "", block)
        block = re.sub(r"/\*.*?\*/", "", block, flags=re.S)
        for match in CALL.finditer(block):
            symbol = match.group(1)
            prefix = block[max(0, match.start(1) - 2) : match.start(1)]
            if prefix.endswith((".", ">", ":")):
                continue
            if symbol in IGNORE_SYMBOLS:
                continue
            if is_function_definition(block, match):
                continue
            if symbol in defined_in_recipe or is_user_symbol(symbol):
                continue
            if symbol.startswith("__") and symbol.endswith("__"):
                continue
            symbols.add(symbol)
    return symbols


def evidence_files(scope: Path, recipe: Path, include_examples: bool) -> list[Path]:
    files: list[Path] = []
    for path in scope.rglob("*"):
        if not path.is_file() or path == recipe:
            continue
        parts = path.relative_to(scope).parts
        if "recipes" in parts:
            continue
        if not include_examples and any(part.upper() in {"EXAM", "EXAMPLES", "PROJECTS"} for part in parts):
            continue
        if path.suffix in TEXT_SUFFIXES:
            files.append(path)
    return files


def build_evidence_index(files: list[Path]) -> str:
    chunks: list[str] = []
    for path in files:
        try:
            chunks.append(read_text(path))
        except OSError:
            continue
    return "\n".join(chunks)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("root", nargs="?", default=".", help="Skill root directory")
    parser.add_argument("--strict", action="store_true", help="Treat findings as failures")
    parser.add_argument("--include-examples", action="store_true", help="Also scan large example/project trees")
    parser.add_argument("--recipe-glob", default="**/recipes/*.md", help="Recipe glob relative to the skill root")
    parser.add_argument("--scope-glob", help="Only check recipes whose relative path matches this glob")
    parser.add_argument("--max-findings", type=int, default=100, help="Maximum findings to print")
    args = parser.parse_args()

    root = Path(args.root).resolve()
    cache: dict[Path, str] = {}
    findings: list[str] = []

    for recipe in sorted(root.glob(args.recipe_glob)):
        rel_recipe = recipe.relative_to(root)
        if args.scope_glob and not rel_recipe.match(args.scope_glob):
            continue
        symbols = extract_symbols(recipe)
        if not symbols:
            continue
        scope = recipe_scope(root, recipe)
        if scope not in cache:
            cache[scope] = build_evidence_index(evidence_files(scope, recipe, args.include_examples))
        evidence = cache[scope]
        for symbol in sorted(symbols):
            if symbol not in evidence:
                findings.append(f"{rel_recipe}: symbol `{symbol}` not found in {scope.relative_to(root)} evidence")

    shown = findings[: max(args.max_findings, 0)]
    for finding in shown:
        print(f"WARN: {finding}")
    if len(findings) > len(shown):
        print(f"WARN: ... {len(findings) - len(shown)} more findings not shown")

    if findings and args.strict:
        print(f"FAILED: {len(findings)} symbol findings")
        return 1
    print(f"OK: {root.name} ({len(findings)} symbol findings)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
