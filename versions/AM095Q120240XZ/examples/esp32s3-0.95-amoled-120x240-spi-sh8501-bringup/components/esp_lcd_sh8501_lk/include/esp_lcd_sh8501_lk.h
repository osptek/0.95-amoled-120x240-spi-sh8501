#pragma once

#include <stdint.h>

#include "driver/spi_master.h"
#include "esp_lcd_panel_vendor.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SH8501A SPI4 (LK) panel initialization command entry.
 */
typedef struct {
    int cmd;
    const void *data;
    size_t data_bytes;
    unsigned int delay_ms;
} sh8501_lk_lcd_init_cmd_t;

/**
 * @brief Vendor configuration for SH8501A SPI4 (LK) panel.
 *
 * Pass via `esp_lcd_panel_dev_config_t::vendor_config`.
 * SPI bus must be initialized before `esp_lcd_new_panel_sh8501_lk()`.
 */
typedef struct {
    spi_host_device_t spi_host;
    int cs_gpio_num;
    int dc_gpio_num;
    uint32_t pclk_hz;
    const sh8501_lk_lcd_init_cmd_t *init_cmds;
    uint16_t init_cmds_size;
} sh8501_lk_vendor_config_t;

/**
 * @brief Create SH8501A SPI4 (LK) LCD panel.
 *
 * Registers an 8-bit SPI device and implements `esp_lcd_panel_t`.
 * All commands and pixel data use WriteComm/WriteData (DC GPIO).
 */
esp_err_t esp_lcd_new_panel_sh8501_lk(const esp_lcd_panel_dev_config_t *panel_dev_config,
                                      esp_lcd_panel_handle_t *ret_panel);

/**
 * @brief Set brightness via command 0x51 (0-100%).
 */
esp_err_t esp_lcd_panel_sh8501_lk_set_brightness(esp_lcd_panel_handle_t panel, uint8_t brightness_percent);

#define SH8501_LK_PANEL_BUS_SPI_CONFIG(sclk, mosi, max_trans_sz) \
    {                                                            \
        .sclk_io_num = sclk,                                     \
        .mosi_io_num = mosi,                                     \
        .miso_io_num = -1,                                       \
        .quadhd_io_num = -1,                                     \
        .quadwp_io_num = -1,                                     \
        .max_transfer_sz = max_trans_sz,                         \
    }

#ifdef __cplusplus
}
#endif
