<p align="left"><img alt="OSPTEK" src="./images/logo.png" width="200" /></p>

<h1 align="center">OSPTEK 0.95″ AMOLED 120×240 (SH8501 · SPI)</h1>

<p align="center"><b>AMOLED module · SPI · SH8501</b></p>

<p align="center"><a href="./README.md">简体中文</a> | English</p>

<p align="center">
  <img alt="Size: 0.95 inch" src="https://img.shields.io/badge/Size-0.95%22-3498DB?style=flat-square" />
  <img alt="Resolution: 120x240" src="https://img.shields.io/badge/Resolution-120%C3%97240-8E44AD?style=flat-square" />
  <img alt="Interface: SPI" src="https://img.shields.io/badge/Interface-SPI-27AE60?style=flat-square" />
  <img alt="Driver: SH8501" src="https://img.shields.io/badge/Driver-SH8501-E7352C?style=flat-square" />
</p>

<p align="center"><img alt="OSPTEK 0.95&quot; 120×240 AMOLED SPI module (SH8501) product image" src="./images/product.png" width="640" /></p>

## Contents

- [Overview](#overview)
- [Specifications](#specifications)
- [Sample projects](#sample-projects)
- [Repository layout](#repository-layout)
- [Resources](#resources)
- [Buy](#buy)
- [Support](#support)

---

## Overview

OSPTEK **0.95″ 120×240 AMOLED** is an **SPI** color display module driven by **SH8501**. The compact portrait resolution suits wearables, status indicators, and tight HMI layouts.

Spec ID (repository name): `0.95-amoled-120x240-spi-sh8501`

Current module version: **AM095Q120240XZ**. Electrical and mechanical details follow [`docs/AM_095_Q120240_XZ_d0c3fdd1ce.pdf`](./docs/AM_095_Q120240_XZ_d0c3fdd1ce.pdf).

## Specifications

| Item | Spec |
| ---- | ---- |
| Size | 0.95 inch |
| Type | AMOLED (color) |
| Resolution | 120×240 |
| Interface | SPI |
| Driver IC | SH8501 |

> Full outline, FPC definition, power, and timing follow the product datasheet / driver IC datasheet.

## Sample projects

| Description | Path |
| ---- | ---- |
| ESP32-S3 · SH8501 SPI + LVGL9 | [`examples/s3-idf_sh8501-spi_lvgl-v9/`](./examples/s3-idf_sh8501-spi_lvgl-v9/) |

## Repository layout

```text
0.95-amoled-120x240-spi-sh8501/
├── README.md
├── README_EN.md
├── MODULE_VERSION.md
├── LICENSE
├── images/          # README assets
├── docs/            # datasheets, init files
└── examples/        # sample projects
```

## Resources

### Product files

| Resource | Link |
| ---- | ---- |
| Product datasheet (AM095Q120240XZ) | [`docs/AM_095_Q120240_XZ_d0c3fdd1ce.pdf`](./docs/AM_095_Q120240_XZ_d0c3fdd1ce.pdf) |
| Driver IC datasheet (SH8501) | [`docs/SH_8501_A0_Data_Sheet_Preliminary_V0_0_UCS_210401_Truly_7722af3bde.pdf`](./docs/SH_8501_A0_Data_Sheet_Preliminary_V0_0_UCS_210401_Truly_7722af3bde.pdf) |
| Init sequence (text) | [`docs/0.95AM-500亮度-代码-ok.txt`](./docs/0.95AM-500%E4%BA%AE%E5%BA%A6-%E4%BB%A3%E7%A0%81-ok.txt) |
| Adapter schematic | [`docs/AM095Q120240XZ转接板V1.0.pdf`](./docs/AM095Q120240XZ%E8%BD%AC%E6%8E%A5%E6%9D%BFV1.0.pdf) |

### Samples

- [ESP32-S3 SH8501 SPI + LVGL9](./examples/s3-idf_sh8501-spi_lvgl-v9/)

## Buy

<p align="center">
  <a href="https://www.aliexpress.com/store/1105701619"><img alt="AliExpress store" src="https://img.shields.io/badge/AliExpress-Official_Store-FF6A00?style=for-the-badge" /></a>
  &nbsp;&nbsp;
  <a href="https://shop110742373.taobao.com/"><img alt="Taobao store" src="https://img.shields.io/badge/Taobao-Official_Store-FF6A00?style=for-the-badge" /></a>
</p>

**Overseas (AliExpress)**

- Store: [OSPTEK Official Store](https://www.aliexpress.com/store/1105701619)

**China (Taobao)**

- Store: [鱼鹰光电工厂店](https://shop110742373.taobao.com/)

## Support

- Technical support / product inquiry: <luyu@osptek.com>
- QQ group (China): **985881096**
- Website: <https://osptek.com/>
- For questions or problems, you can also open a GitHub [Issue](https://github.com/osptek/0.95-amoled-120x240-spi-sh8501/issues)

---

<p align="center"><sub>© 2026 OSPTEK · Materials in this repository are licensed under CC BY 4.0</sub></p>
