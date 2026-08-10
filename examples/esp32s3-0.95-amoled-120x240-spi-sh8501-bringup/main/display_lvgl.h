#pragma once

#include "esp_err.h"
#include "esp_lcd_panel_ops.h"
#include "lvgl.h"

esp_err_t display_lvgl_init(void);

lv_display_t *display_lvgl_get_disp(void);

esp_lcd_panel_handle_t display_lvgl_get_panel(void);

/** 亮度 0~100，对应发送 0x51 命令值 0x00~0xFF */
esp_err_t display_lvgl_set_brightness(uint8_t percent);
