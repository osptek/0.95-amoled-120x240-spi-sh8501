# SH8501A 0.95" AMOLED SPI4 点亮调试结论

> 项目：ESP32-S3 + AM095Q120240XZ（120×240，SH8501A，SPI4 转接板）  
> 日期：2026-07-07  
> 厂商 init 参考：`docs/0.95_SH8501A_120x240_SPI4_20260515.txt`

---

## 1. 硬件与引脚

| 信号 | GPIO |
|------|------|
| CS   | 39   |
| DC   | 38   |
| SCLK | 21   |
| MOSI | 47   |
| RST  | 45   |
| TE   | 40（本工程未强制依赖） |

- 分辨率：**120 × 240**
- 色深：**RGB565**（`0x3A = 0x55`）
- SPI 主机：`SPI2_HOST`，时钟 **10 MHz**
- 坐标范围（厂商）：列 `0x0000 ~ 0x0077`（119），行 `0x0000 ~ 0x00EF`（239）

---

## 2. 问题现象

### 2.1 能亮的场景

| 测试方式 | 结果 |
|----------|------|
| BIST 自检（`0xC0/0xBA/0xC1` 解锁序列） | ✅ 能亮 |
| 裸 SPI 8-bit `WriteComm` / `WriteData` 刷纯色 | ✅ 能亮（白/蓝/红） |

### 2.2 不亮的场景

| 测试方式 | 结果 |
|----------|------|
| 完整 init + `esp_lcd_panel_draw_bitmap()` | ❌ 不亮 |
| 改 `lcd_cmd_bits = 8` 后仍走 esp_lcd `tx_color` | ❌ 不亮 |
| 关 TE、删 init 末尾 `0x2C` 等微调 | ❌ 不亮 |

### 2.3 示波器观察

- CS、DC、SCLK 在刷图时**波形正常**（全屏约 12 ms）
- RST 有 **10 ms 低脉冲**，复位有效

**初步判断**：硬件连线、供电、复位、init 时序基本正常；问题出在**刷图阶段的软件协议**，而非硬件或 init 不完整。

---

## 3. 调试过程中排除的方向

以下方向均尝试过，**不是根因**：

1. **init 序列不完整** — 已按厂商文档完整发送，BIST 也能亮
2. **RST 未复位** — 示波器确认有效
3. **SPI 没发出去** — 示波器确认有完整传输
4. **仅改 `lcd_cmd_bits`** — 单独改为 8-bit 不足以让 esp_lcd 送图路径工作
5. **TE 信号** — 开关 TE 对刷图不亮无改善
6. **init 末尾多一个 `0x2C`** — 删除后仍不亮

---

## 4. 根因分析

### 4.1 核心矛盾：init 能走 esp_lcd，刷图不能

ESP-IDF `esp_lcd` 的 SPI panel IO 面向通用 LCD / QSPI 设计，默认用 **32-bit 命令帧**（opcode + cmd 等）发命令，刷图走 `esp_lcd_panel_io_tx_color()`。

厂商 SPI4 参考代码则是：

```c
WriteComm(cmd);   // DC=0，每次只发 1 字节命令
WriteData(data);  // DC=1，发参量或像素
```

| 阶段 | esp_lcd 默认路径 | 厂商 SPI4 要求 |
|------|------------------|----------------|
| init | 32-bit 命令帧（`lcd_cmd_bits=32`） | 8-bit `WriteComm` |
| 刷图 | `tx_color()` 封装送像素 | 8-bit 裸 SPI + 双 `0x2C` + 像素流 |

init 阶段两种帧格式屏都能接受（或容忍），但**刷图像素阶段必须严格按厂商协议**，否则控制器收不到有效 RAMWR 数据，表现为「SPI 在发、屏不亮」。

### 4.2 厂商 `DM_block_write` 协议要点

厂商文档中的送图函数：

```c
void DM_block_write(unsigned int Xstart, unsigned int Xend,
                    unsigned int Ystart, unsigned int Yend)
{
    WriteComm(0x2A);  // CASET + 4 字节坐标
    WriteComm(0x2B);  // RASET + 4 字节坐标
    WriteComm(0x2C);  // RAMWR — 第一次
    WriteComm(0x2C);  // RAMWR — 第二次（必须连续发两次）
    // 随后发送像素数据
}
```

与 esp_lcd 默认 `draw_bitmap` 的差异：

| 项目 | esp_lcd 默认 | 厂商要求 |
|------|-------------|----------|
| 命令格式 | 32-bit 帧 | 8-bit 单字节 |
| RAMWR | 通常一次 `0x2C` | **连续两次 `0x2C`** |
| 末坐标 | exclusive（`x_end` 不含） | **inclusive**（`0x77` = 第 119 列） |
| 像素发送 | `io_tx_color()` | DC=1 裸 SPI 连续写 |

### 4.3 全屏 DMA 单次长度限制

全屏 RGB565 数据量：

```
120 × 240 × 2 = 57,600 字节 ≈ 56.25 KB
```

ESP32-S3 SPI DMA 单次传输上限约 **32 KB**。一次性发送会报错：

```
txdata transfer > hardware max
```

必须**分块发送**（本工程使用 4092 字节/块），并用 `SPI_TRANS_CS_KEEP_ACTIVE` 保持 CS 低，使多块在屏看来是一次连续传输。

### 4.4 同一 CS 不能挂两个 SPI 设备

集成 vendor SPI 时，若顺序错误：

```
错误：spi_bus_add_device(vendor) → esp_lcd_panel_io_del()
正确：esp_lcd_panel_io_del()      → spi_bus_add_device(vendor)
```

同一 CS 线上两个 SPI 设备共存会导致集成版刷图失败（即使裸 SPI 单独测试能亮）。

---

## 5. 从不亮到亮：最关键的一步

> **init 完成后，将刷图路径从 esp_lcd 默认 IO 切换到厂商 8-bit 裸 SPI 送图。**

对应 API：

```c
esp_lcd_panel_sh8501_enable_vendor_spi_draw(panel, &lcd_io,
    SPI2_HOST, PIN_DC, PIN_CS, 10*1000*1000);
```

该函数内部完成三件事：

1. **删除** `esp_lcd` 的 `panel_io`（释放同 CS 上的 esp_lcd SPI 设备）
2. **注册** 独立 8-bit SPI 设备（`spi_bus_add_device`）
3. **重定向** `esp_lcd_panel_draw_bitmap()` → `vendor_spi_dm_block_write()`

此后每次刷图按厂商协议执行：

```
DC=0  WriteComm(0x2A) → WriteData(列坐标 4B)
DC=0  WriteComm(0x2B) → WriteData(行坐标 4B)
DC=0  WriteComm(0x2C)
DC=0  WriteComm(0x2C)          ← 双 RAMWR
DC=1  分块 WriteData(像素流)
```

**一句话**：不是 init 写错了，而是刷图协议错了；把送图改成与厂商裸 SPI demo 一致，屏就亮了。

---

## 6. 最终软件架构

```
┌─────────────────────────────────────────────────────────┐
│  应用层 (main.c / LVGL)                                  │
│    esp_lcd_panel_draw_bitmap()  或  LVGL flush_cb       │
└────────────────────────┬────────────────────────────────┘
                         │
┌────────────────────────▼────────────────────────────────┐
│  esp_lcd_sh8501 驱动 (components/esp_lcd_sh8501/)        │
│                                                          │
│  init 阶段：esp_lcd panel_io (32-bit 命令帧)             │
│  刷图阶段：vendor_spi (8-bit WriteComm/WriteData)        │
│            └─ vendor_spi_dm_block_write()                │
│               ├─ 双 0x2C                                 │
│               ├─ inclusive 坐标                          │
│               └─ 4092B 分块 DMA                          │
└─────────────────────────────────────────────────────────┘
```

### 6.1 推荐初始化顺序

```c
spi_bus_initialize(...)
esp_lcd_new_panel_io_spi(...)      // lcd_cmd_bits = 32
esp_lcd_new_panel_sh8501(...)
esp_lcd_panel_reset(...)
esp_lcd_panel_init(...)
esp_lcd_panel_sh8501_enable_vendor_spi_draw(...)  // ← 关键
// 此后 lcd_io 已被释放，不可再用于 esp_lcd IO 操作
```

### 6.2 关键源文件

| 文件 | 内容 |
|------|------|
| `components/esp_lcd_sh8501/esp_lcd_sh8501.c` | vendor SPI 实现、`dm_block_write`、分块 DMA |
| `components/esp_lcd_sh8501/include/esp_lcd_sh8501.h` | `enable_vendor_spi_draw` API |
| `main/display_lvgl.c` | LCD + LVGL 初始化、自定义 flush |
| `docs/0.95_SH8501A_120x240_SPI4_20260515.txt` | 厂商 init 与 `DM_block_write` 参考 |

---

## 7. LVGL 接入说明

### 7.1 为何不用 `lvgl_port_add_disp()`

`enable_vendor_spi_draw()` 会 **删除 `panel_io` 句柄**。`lvgl_port_add_disp()` 依赖该 IO 做异步 DMA flush callback，句柄失效后回调无法完成，LVGL 会卡死。

### 7.2 当前 LVGL 方案

```
lvgl_port_init()                    // 仅任务 + 定时器
lv_display_create()                 // 手动创建 display
lv_display_set_flush_cb()           // 自定义同步 flush
  └─ lv_draw_sw_rgb565_swap()       // 修正字节序
  └─ esp_lcd_panel_draw_bitmap()   // 走 vendor SPI
  └─ lv_display_flush_ready()       // 同步完成立刻 ready
lv_display_set_buffers(FULL, DMA)   // 全屏双缓冲
```

### 7.3 性能粗估

- 全屏 57,600 字节 @ 10 MHz SPI ≈ **46 ms** 纯传输
- 加上命令开销，每帧约 **50 ms** → 约 **20 FPS**（全屏刷新模式）

后续可改为 **partial 刷新模式** 或 **提高 SPI 时钟** 以改善帧率（需实测信号完整性）。

---

## 8. 颜色字节序（次要问题）

裸 SPI 测试时：

| 固件发送 | 屏上显示 |
|----------|----------|
| 白 | 白 |
| 红 | 蓝 |
| 绿 | 红 |

说明 RGB565 **高低字节顺序与屏期望相反**，属于显示层问题，与「完全不亮」无关。

处理方式：

- LVGL flush 中调用 `lv_draw_sw_rgb565_swap()`
- 或在驱动层对像素做 swap

若颜色仍不对，可尝试关闭 swap 对比验证。

---

## 9. 结论汇总

| # | 结论 |
|---|------|
| 1 | 硬件正常；BIST 和裸 SPI 测试均已验证 |
| 2 | init 序列正确；问题不在 init 完整性 |
| 3 | **根因是刷图协议**：esp_lcd 默认 32-bit 帧 + `tx_color` 与 SH8501A SPI4 不兼容 |
| 4 | **关键修复**：`enable_vendor_spi_draw()`，8-bit 裸 SPI + 双 `0x2C` + 分块 DMA |
| 5 | 全屏数据须分块（≤4092 B/块），否则 DMA 超限 |
| 6 | 同 CS 须先删 esp_lcd IO 再注册 vendor 设备 |
| 7 | RGB565 需 swap 字节序；LVGL 用自定义同步 flush，不用 `lvgl_port_add_disp` |

---

## 10. 后续建议

1. **向厂商索要 ESP32 SPI4 参考工程**，作最终对照（当前结论已与厂商 C 参考逻辑一致）。
2. **尝试提高 SPI 时钟**（20/40 MHz），在示波器确认波形前提下提升帧率。
3. **改为 partial 刷新**（`LV_DISPLAY_RENDER_MODE_PARTIAL`），减少每帧传输量。
4. 若需恢复 `lvgl_port_add_disp` 异步刷新，须保留 panel_io 或扩展 esp_lvgl_port 支持 vendor SPI 同步路径。
5. 亮度调节可走 `0x51` 命令（init 中已设 `0xFF`），可封装为运行时 API。

---

## 附录：厂商 init 序列（摘要）

```
0x11        Sleep Out         delay 60ms
0x2A        列 0x0000~0x0077
0x2B        行 0x0000~0x00EF
0x44        TE 相关
0x35        TE on
0x3A = 0x55 RGB565
0x51 = 0xFF 亮度最大          delay 60ms
0x29        Display On        delay 120ms
0x39        扩展命令
```

BIST 测试（仅诊断用，正常刷图不需要）：

```
0xC0 0x5A 0x5A
0xBA 0x81
0xC1 0x5A 0x5A
```
