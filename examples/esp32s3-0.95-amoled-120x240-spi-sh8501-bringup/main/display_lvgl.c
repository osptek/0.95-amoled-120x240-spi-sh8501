#include "display_lvgl.h"

#include "esp_check.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_sh8501_lk.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "driver/spi_master.h"
#include "draw/sw/lv_draw_sw_utils.h"

#define LCD_H_RES            (120)
#define LCD_V_RES            (240)

#define LCD_SPI_NUM          (SPI2_HOST)
#define LCD_PIXEL_CLK_HZ     (40 * 1000 * 1000)
#define LCD_DRAW_BUFF_HEIGHT (LCD_V_RES)

#define PIN_NUM_LCD_CS       (GPIO_NUM_39)
#define PIN_NUM_LCD_DC       (GPIO_NUM_38)
#define PIN_NUM_LCD_SCLK     (GPIO_NUM_21)
#define PIN_NUM_LCD_MOSI     (GPIO_NUM_47)
#define PIN_NUM_LCD_RST      (GPIO_NUM_45)

#define LCD_INITIAL_BRIGHTNESS_PERCENT  (50)

static const char *TAG = "display_lvgl";

/*
static const sh8501_lk_lcd_init_cmd_t lcd_init_cmds[] = {
    {0x11, (uint8_t[]){0}, 0, 60},
    {0x2A, (uint8_t[]){0x00, 0x00, 0x00, 0x77}, 4, 0},
    {0x2B, (uint8_t[]){0x00, 0x00, 0x00, 0xEF}, 4, 0},
    {0x44, (uint8_t[]){0x01, 0x27}, 2, 0},
    {0x35, (uint8_t[]){0x00}, 1, 0},
    {0x3A, (uint8_t[]){0x55}, 1, 0},
    {0x51, (uint8_t[]){0xFF}, 1, 60},
    {0x29, (uint8_t[]){0}, 0, 120},
    {0x39, (uint8_t[]){0}, 0, 0},
};
*/

static esp_lcd_panel_handle_t lcd_panel = NULL;
static lv_display_t *lvgl_disp = NULL;

static esp_err_t lcd_init(void)
{
    esp_err_t ret = ESP_OK;

    const spi_bus_config_t buscfg = SH8501_LK_PANEL_BUS_SPI_CONFIG(PIN_NUM_LCD_SCLK,
                                                                   PIN_NUM_LCD_MOSI,
                                                                   LCD_H_RES * LCD_DRAW_BUFF_HEIGHT * sizeof(uint16_t));
    ESP_RETURN_ON_ERROR(spi_bus_initialize(LCD_SPI_NUM, &buscfg, SPI_DMA_CH_AUTO), TAG, "SPI init failed");

    sh8501_lk_vendor_config_t vendor_config = {
        .spi_host = LCD_SPI_NUM,
        .cs_gpio_num = PIN_NUM_LCD_CS,
        .dc_gpio_num = PIN_NUM_LCD_DC,
        .pclk_hz = LCD_PIXEL_CLK_HZ,
        //.init_cmds = lcd_init_cmds,
        //.init_cmds_size = sizeof(lcd_init_cmds) / sizeof(sh8501_lk_lcd_init_cmd_t),
    };

    const esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_NUM_LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
        .vendor_config = &vendor_config,
    };

    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_sh8501_lk(&panel_config, &lcd_panel), err, TAG, "panel create failed");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_reset(lcd_panel), err, TAG, "panel reset failed");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_init(lcd_panel), err, TAG, "panel init failed");
    return ret;

err:
    if (lcd_panel) {
        esp_lcd_panel_del(lcd_panel);
        lcd_panel = NULL;
    }
    spi_bus_free(LCD_SPI_NUM);
    return ret;
}

static void lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    esp_lcd_panel_handle_t panel = (esp_lcd_panel_handle_t)lv_display_get_user_data(disp);
    const int32_t x1 = area->x1;
    const int32_t y1 = area->y1;
    const int32_t x2 = area->x2 + 1;
    const int32_t y2 = area->y2 + 1;

    const uint32_t px_count = (uint32_t)lv_area_get_width(area) * (uint32_t)lv_area_get_height(area);
    lv_draw_sw_rgb565_swap(px_map, px_count);

    esp_lcd_panel_draw_bitmap(panel, x1, y1, x2, y2, px_map);
    lv_display_flush_ready(disp);
}

static esp_err_t lvgl_init(void)
{
    const lvgl_port_cfg_t lvgl_cfg = {
        .task_priority = 4,
        .task_stack = 8192,
        .task_affinity = -1,
        .task_max_sleep_ms = 500,
        .timer_period_ms = 5,
    };
    ESP_RETURN_ON_ERROR(lvgl_port_init(&lvgl_cfg), TAG, "LVGL port init failed");

    const size_t buf_bytes = LCD_H_RES * LCD_V_RES * sizeof(lv_color16_t);
    void *buf1 = heap_caps_malloc(buf_bytes, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    void *buf2 = heap_caps_malloc(buf_bytes, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    ESP_RETURN_ON_FALSE(buf1 && buf2, ESP_ERR_NO_MEM, TAG, "LVGL buffer alloc failed");

    lvgl_port_lock(0);
    lvgl_disp = lv_display_create(LCD_H_RES, LCD_V_RES);
    lv_display_set_color_format(lvgl_disp, LV_COLOR_FORMAT_RGB565);
    lv_display_set_user_data(lvgl_disp, lcd_panel);
    lv_display_set_flush_cb(lvgl_disp, lvgl_flush_cb);
    lv_display_set_buffers(lvgl_disp, buf1, buf2, buf_bytes, LV_DISPLAY_RENDER_MODE_FULL);
    lvgl_port_unlock();

    return ESP_OK;
}

esp_err_t display_lvgl_init(void)
{
    ESP_RETURN_ON_ERROR(lcd_init(), TAG, "LCD init failed");
    ESP_RETURN_ON_ERROR(display_lvgl_set_brightness(LCD_INITIAL_BRIGHTNESS_PERCENT), TAG, "set brightness failed");
    ESP_RETURN_ON_ERROR(lvgl_init(), TAG, "LVGL init failed");
    return ESP_OK;
}

esp_lcd_panel_handle_t display_lvgl_get_panel(void)
{
    return lcd_panel;
}

esp_err_t display_lvgl_set_brightness(uint8_t percent)
{
    ESP_RETURN_ON_FALSE(lcd_panel, ESP_ERR_INVALID_STATE, TAG, "panel not initialized");
    return esp_lcd_panel_sh8501_lk_set_brightness(lcd_panel, percent);
}

lv_display_t *display_lvgl_get_disp(void)
{
    return lvgl_disp;
}
