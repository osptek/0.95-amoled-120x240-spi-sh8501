<p align="left"><img alt="OSPTEK" src="./images/logo.png" width="200" /></p>

<h1 align="center">OSPTEK 0.95″ AMOLED 120×240（SH8501 · SPI）</h1>

<p align="center"><b>AMOLED 模组 · SPI · SH8501</b></p>

<p align="center"><a href="./README_EN.md">English</a> | 简体中文 · <a href="../../README.md">规格族索引</a></p>

<p align="center">
  <img alt="Size: 0.95 inch" src="https://img.shields.io/badge/Size-0.95%22-3498DB?style=flat-square" />
  <img alt="Resolution: 120x240" src="https://img.shields.io/badge/Resolution-120%C3%97240-8E44AD?style=flat-square" />
  <img alt="Interface: SPI" src="https://img.shields.io/badge/Interface-SPI-27AE60?style=flat-square" />
  <img alt="Driver: SH8501" src="https://img.shields.io/badge/Driver-SH8501-E7352C?style=flat-square" />
</p>

<p align="center"><img alt="OSPTEK 0.95 寸 120×240 AMOLED SPI 模组（SH8501）宣传图" src="./images/product.png" width="640" /></p>

## 目录

- [产品简介](#产品简介)
- [规格参数](#规格参数)
- [示例工程](#示例工程)
- [仓库结构](#仓库结构)
- [相关资料](#相关资料)
- [购买链接](#购买链接)
- [技术支持](#技术支持)

---

## 产品简介

OSPTEK **0.95 寸 120×240 AMOLED** 是一款 **SPI** 接口彩色显示模组，驱动芯片为 **SH8501**。小尺寸竖条分辨率适合穿戴、状态指示与紧凑 HMI 等场景。

规格标识（仓库名）：`0.95-amoled-120x240-spi-sh8501`

当前模组版本：**AM095Q120240XZ**。电气与外形细节以 [`docs/AM_095_Q120240_XZ_d0c3fdd1ce.pdf`](./docs/AM_095_Q120240_XZ_d0c3fdd1ce.pdf) 为准。

## 规格参数

| 项目 | 规格 |
| ---- | ---- |
| 尺寸 | 0.95 英寸 |
| 类型 | AMOLED（彩色） |
| 分辨率 | 120×240 |
| 接口 | SPI |
| 驱动 IC | SH8501 |

> 完整外形尺寸、FPC 定义、供电与时序以产品规格书 / 驱动手册为准。

## 示例工程

| 说明 | 路径 |
| ---- | ---- |
| ESP32-S3 · SH8501 SPI bring-up（LVGL9） | [`examples/esp32s3-0.95-amoled-120x240-spi-sh8501-bringup/`](./examples/esp32s3-0.95-amoled-120x240-spi-sh8501-bringup/) |

## 仓库结构

```text
0.95-amoled-120x240-spi-sh8501/                                # 仓库根（导航见 ../../README.md）
└── versions/
    └── AM095Q120240XZ/                                # 本料号完整资料
        ├── README.md
        ├── README_EN.md
        ├── images/
        ├── docs/
        └── examples/
```

## 相关资料

### 本产品资料

| 资料 | 链接 |
| ---- | ---- |
| 产品规格书（AM095Q120240XZ） | [`docs/AM_095_Q120240_XZ_d0c3fdd1ce.pdf`](./docs/AM_095_Q120240_XZ_d0c3fdd1ce.pdf) |
| 驱动 IC 数据手册（SH8501） | [`docs/SH_8501_A0_Data_Sheet_Preliminary_V0_0_UCS_210401_Truly_7722af3bde.pdf`](./docs/SH_8501_A0_Data_Sheet_Preliminary_V0_0_UCS_210401_Truly_7722af3bde.pdf) |
| 初始化序列（文本） | [`docs/0.95AM-500亮度-代码-ok.txt`](./docs/0.95AM-500%E4%BA%AE%E5%BA%A6-%E4%BB%A3%E7%A0%81-ok.txt) |
| 厂商 SPI4 init（2026-05-15） | [`docs/0.95_SH8501A_120x240_SPI4_20260515.txt`](./docs/0.95_SH8501A_120x240_SPI4_20260515.txt) |
| SPI4 点亮调试结论 | [`docs/SH8501A_SPI4_bringup_conclusion.md`](./docs/SH8501A_SPI4_bringup_conclusion.md) |
| 转接板原理图 | [`docs/AM095Q120240XZ转接板V1.0.pdf`](./docs/AM095Q120240XZ%E8%BD%AC%E6%8E%A5%E6%9D%BFV1.0.pdf) |

### 示例工程

- [ESP32-S3 SH8501 SPI bring-up（LVGL9）](./examples/esp32s3-0.95-amoled-120x240-spi-sh8501-bringup/)

## 购买链接

<p align="center">
  <a href="https://shop110742373.taobao.com/"><img alt="淘宝官方店铺" src="https://img.shields.io/badge/淘宝-官方店铺-FF6A00?style=for-the-badge" /></a>
  &nbsp;&nbsp;
  <a href="https://www.aliexpress.com/store/1105701619"><img alt="速卖通官方店铺" src="https://img.shields.io/badge/速卖通-官方店铺-FF6A00?style=for-the-badge" /></a>
</p>

**国内（淘宝）**

- 店铺：[鱼鹰光电工厂店](https://shop110742373.taobao.com/)

**海外（AliExpress）**

- 店铺：[OSPTEK Official Store](https://www.aliexpress.com/store/1105701619)

## 技术支持

- 技术支持 / 产品咨询：<luyu@osptek.com>
- QQ 技术交流群：**985881096**
- 公司官网：<https://osptek.com/>
- 有任何问题，都可以在本仓库 Issues 中提问

---

<p align="center"><sub>© 2026 OSPTEK 鱼鹰光电 · 本仓库资料采用 CC BY 4.0 许可</sub></p>
