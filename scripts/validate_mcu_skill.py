#!/usr/bin/env python3
"""Validate MCU skill structure and recipe quality signals."""

from __future__ import annotations

import argparse
import re
from pathlib import Path


ALLOWED_FRONTMATTER = {"name", "description", "license", "metadata", "allowed-tools"}
REQUIRED_COMMON = [
    "SKILL.md",
    "agents/openai.yaml",
    "resources/development_workflow.md",
    "resources/source_strategy.md",
    "resources/recipe_quality.md",
]
REQUIRED_BY_SKILL = {
    "wch-dev-skill": ["resources/chip_matrix.md", "resources/scenario_routing.md"],
    "esp-dev-skill": ["resources/repo_index.md", "resources/recipe_index.md"],
    "chipintelli-dev-skill": [
        "resources/chip_matrix.md",
        "resources/scenario_routing.md",
        "resources/sdk_index.md",
    ],
}
DANGEROUS_PATTERNS = [
    "Always use the latest SDK",
    "Default to latest version",
    "no matching doc = does not exist",
]
META_PATTERNS = {
    "applies": re.compile(r"(Applies to|Applicable|Chips:|适用|适用芯片)", re.I),
    "version": re.compile(r"(SDK|EVT|ESP-IDF|Arduino|Version|版本)", re.I),
    "evidence": re.compile(r"(Evidence|Example|参考|证据|示例)", re.I),
    "validation": re.compile(r"(Validation|验证|compiled|source-matched|example-derived|draft)", re.I),
}


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="replace")


def frontmatter(text: str) -> tuple[dict[str, str], list[str]]:
    if not text.startswith("---"):
        return {}, ["SKILL.md missing YAML frontmatter"]
    end = text.find("\n---", 3)
    if end < 0:
        return {}, ["SKILL.md frontmatter is not closed"]
    keys: dict[str, str] = {}
    errors: list[str] = []
    for line in text[3:end].splitlines():
        if not line or line.startswith(" ") or line.startswith("-"):
            continue
        if ":" not in line:
            errors.append(f"Malformed frontmatter line: {line}")
            continue
        key = line.split(":", 1)[0].strip()
        keys[key] = line
        if key not in ALLOWED_FRONTMATTER:
            errors.append(f"Unsupported top-level frontmatter key: {key}")
    for required in ("name", "description"):
        if required not in keys:
            errors.append(f"Missing required frontmatter key: {required}")
    return keys, errors


def find_recipe_files(root: Path) -> list[Path]:
    return sorted(root.glob("**/recipes/*.md"))


def check_inline_md_paths(root: Path) -> list[str]:
    errors: list[str] = []
    pattern = re.compile(r"`([^`]+\.md)`")
    for md in [root / "SKILL.md", *root.glob("resources/*.md")]:
        if md.name in {"recipe_index.md", "repo_index.md", "sdk_index.md", "glossary.md"}:
            continue
        if not md.exists():
            continue
        text = read_text(md)
        for match in pattern.finditer(text):
            raw = match.group(1)
            if raw.startswith(("http://", "https://")) or "<" in raw or "*" in raw:
                continue
            if not raw.startswith(("resources/", "chips/", "repos/", "agents/", "scripts/")):
                continue
            target = (md.parent / raw).resolve() if not raw.startswith("resources/") and not raw.startswith("chips/") and not raw.startswith("repos/") else (root / raw).resolve()
            if not target.exists():
                errors.append(f"{md.relative_to(root)} references missing path `{raw}`")
    return errors


def recipe_metadata_warnings(root: Path) -> list[str]:
    warnings: list[str] = []
    for recipe in find_recipe_files(root):
        head = "\n".join(read_text(recipe).splitlines()[:40])
        missing = [name for name, pattern in META_PATTERNS.items() if not pattern.search(head)]
        if missing:
            warnings.append(f"{recipe.relative_to(root)} missing recipe metadata: {', '.join(missing)}")
    return warnings


def dangerous_text_warnings(root: Path) -> list[str]:
    warnings: list[str] = []
    for md in root.glob("**/*.md"):
        if any(part in {"resources", "repos", "chips"} for part in md.relative_to(root).parts) and md.stat().st_size > 500_000:
            continue
        text = read_text(md)
        for phrase in DANGEROUS_PATTERNS:
            if phrase in text:
                warnings.append(f"{md.relative_to(root)} contains dangerous absolute rule: {phrase}")
    return warnings


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("root", nargs="?", default=".", help="Skill root directory")
    parser.add_argument("--strict", action="store_true", help="Treat warnings as failures")
    parser.add_argument("--max-warnings", type=int, default=50, help="Maximum warnings to print")
    args = parser.parse_args()

    root = Path(args.root).resolve()
    errors: list[str] = []
    warnings: list[str] = []

    skill_path = root / "SKILL.md"
    if not skill_path.exists():
        errors.append("Missing SKILL.md")
        skill_name = root.name
    else:
        keys, fm_errors = frontmatter(read_text(skill_path))
        errors.extend(fm_errors)
        name_line = keys.get("name", f"name: {root.name}")
        skill_name = name_line.split(":", 1)[1].strip()

    required = REQUIRED_COMMON + REQUIRED_BY_SKILL.get(skill_name, [])
    for rel in required:
        if not (root / rel).exists():
            errors.append(f"Missing required file: {rel}")

    errors.extend(check_inline_md_paths(root))
    warnings.extend(dangerous_text_warnings(root))
    warnings.extend(recipe_metadata_warnings(root))

    for item in errors:
        print(f"ERROR: {item}")
    shown_warnings = warnings[: max(args.max_warnings, 0)]
    for item in shown_warnings:
        print(f"WARN: {item}")
    if len(warnings) > len(shown_warnings):
        print(f"WARN: ... {len(warnings) - len(shown_warnings)} more warnings not shown")

    if errors or (args.strict and warnings):
        print(f"FAILED: {len(errors)} errors, {len(warnings)} warnings")
        return 1
    print(f"OK: {skill_name} ({len(warnings)} warnings)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
