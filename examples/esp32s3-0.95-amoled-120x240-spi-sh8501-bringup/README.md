# ESP32-S3 · 0.95″ AMOLED SH8501 SPI bring-up（LVGL9）

点亮 / LVGL 示例：`AM095Q120240XZ`（120×240 · SH8501A · SPI4）。

## 说明

- 驱动组件：`components/esp_lcd_sh8501_lk`（8-bit 裸 SPI + 双 `0x2C` RAMWR + 分块 DMA）
- UI：LVGL 9 + `esp_lvgl_port`（自定义同步 flush；`idf_component.yml` 拉取依赖）
- 默认引脚（转接板）：CS=39，DC=38，SCLK=21，MOSI=47，RST=45
- SPI 时钟默认 **40 MHz**；亮度默认 50%（`0x51`）

调试结论见仓库 [`docs/SH8501A_SPI4_bringup_conclusion.md`](../../docs/SH8501A_SPI4_bringup_conclusion.md)。  
厂商 init 参考：[`docs/0.95_SH8501A_120x240_SPI4_20260515.txt`](../../docs/0.95_SH8501A_120x240_SPI4_20260515.txt)。

## 编译烧录

```bash
idf.py set-target esp32s3
idf.py build flash monitor
```

依赖由 Component Manager 按 `main/idf_component.yml` 自动拉取（勿提交 `managed_components/`）。
