# 0.95" 120×240 AMOLED SPI module (SH8501) — documentation & samples

**简体中文：** [`README.md`](README.md)

---

> This repository provides **sample projects** for this module, together with datasheets, specifications, and interface / bring-up documentation for selection reference and integration.

## Product overview

| Item | Description |
|:--|:--|
| Module | 0.95-inch **AMOLED** panel, **120×240** resolution |
| Interface | **SPI** |
| Driver IC | **SH8501** |
| Spec ID | **`0.95-amoled-120x240-spi-sh8501`** is the common product designation in documentation |

---

## Repository layout

### Top-level

| Path | Contents |
|:--|:--|
| `docs/` | Datasheets, specifications, interface and initialization documentation |
| `examples/` | **Sample projects** |

### `examples/` layout

| Location | Description (internal package folder) |
|:--|:--|
| `examples/` root | **ESP-IDF代码** (LVGL9 + SH8501 SPI) |

### Sample project paths

| Description | Path |
|:--|:--|
| SH8501 SPI + LVGL9 | `examples/s3-idf_sh8501-spi_lvgl-v9/` |
