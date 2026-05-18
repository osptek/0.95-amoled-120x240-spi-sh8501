#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lvgl_port.h"
#include "lv_demos.h"

#include "esp_lcd_sh8501.h"

/* LCD size */
#define EXAMPLE_LCD_H_RES   (120)
#define EXAMPLE_LCD_V_RES   (240)

#define LV_COLOR_FORMAT (LV_COLOR_FORMAT_RGB565)
#define LCD_COLOR_DEPTH (16)

/* LCD settings */
#define EXAMPLE_LCD_SPI_NUM         (SPI2_HOST)
#define EXAMPLE_LCD_PIXEL_CLK_HZ    (40 * 1000 * 1000)
#define EXAMPLE_LCD_DRAW_BUFF_HEIGHT (EXAMPLE_LCD_V_RES)

/* LCD pins */
#define EXAMPLE_PIN_NUM_LCD_CS (GPIO_NUM_39)
#define EXAMPLE_PIN_NUM_LCD_DC (GPIO_NUM_38)
#define EXAMPLE_PIN_NUM_LCD_SCLK (GPIO_NUM_21)
#define EXAMPLE_PIN_NUM_LCD_MOSI (GPIO_NUM_47)
#define EXAMPLE_PIN_NUM_LCD_RST (GPIO_NUM_45)
#define EXAMPLE_PIN_NUM_LCD_TE (GPIO_NUM_40)

// #define EXAMPLE_LCD_X_GAP              0
// #define EXAMPLE_LCD_Y_GAP              0

static const char *TAG = "test";

/* LCD IO and panel */
static esp_lcd_panel_io_handle_t lcd_io = NULL;
static esp_lcd_panel_handle_t lcd_panel = NULL;

/* LVGL display */
static lv_display_t *lvgl_disp = NULL;

extern void example_lvgl_demo_ui(lv_disp_t *disp);

static esp_err_t app_lcd_init(void)
{
    esp_err_t ret = ESP_OK;

    /* LCD initialization */
    ESP_LOGD(TAG, "Initialize SPI bus");
   const spi_bus_config_t buscfg = SH8501_PANEL_BUS_SPI_CONFIG(EXAMPLE_PIN_NUM_LCD_SCLK,
                                                                    EXAMPLE_PIN_NUM_LCD_MOSI,
                                                                    EXAMPLE_LCD_H_RES * EXAMPLE_LCD_DRAW_BUFF_HEIGHT * sizeof(uint16_t));
    ESP_RETURN_ON_ERROR(spi_bus_initialize(EXAMPLE_LCD_SPI_NUM, &buscfg, SPI_DMA_CH_AUTO), TAG, "SPI init failed");

    ESP_LOGD(TAG, "Install panel IO");
    const esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = EXAMPLE_PIN_NUM_LCD_DC,
        .cs_gpio_num = EXAMPLE_PIN_NUM_LCD_CS,
        .pclk_hz = EXAMPLE_LCD_PIXEL_CLK_HZ,
        .lcd_cmd_bits = 32,
        .lcd_param_bits = 8,
        .spi_mode = 0,
        .trans_queue_depth = 10,
        .on_color_trans_done = NULL,
        .user_ctx = NULL, 
    };

    // 将 LCD 连接到 SPI 总线
    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)EXAMPLE_LCD_SPI_NUM, &io_config, &lcd_io), err, TAG, "New panel IO failed");
    
    sh8501_vendor_config_t vendor_config = {
        .flags = {
            .use_qspi_interface = 0,
        },
    };

    ESP_LOGD(TAG, "Install LCD driver");
    const esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = EXAMPLE_PIN_NUM_LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = LCD_COLOR_DEPTH,
        .vendor_config = &vendor_config,
    };
    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_sh8501(lcd_io, &panel_config, &lcd_panel), err, TAG, "New panel failed");
    
    esp_lcd_panel_reset(lcd_panel);
    esp_lcd_panel_init(lcd_panel);
    // esp_lcd_panel_sh8501_set_brightness(lcd_panel, 70);
    // ESP_ERROR_CHECK(esp_lcd_panel_set_gap(lcd_panel, EXAMPLE_LCD_X_GAP, EXAMPLE_LCD_Y_GAP));
    // ESP_ERROR_CHECK(esp_lcd_panel_mirror(lcd_panel, false, false));
    // ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(lcd_panel, false));
    // ESP_ERROR_CHECK(esp_lcd_panel_invert_color(lcd_panel, 1));//反转颜色 
    // esp_lcd_panel_disp_on_off(lcd_panel, true);

    return ret;

err:
    if (lcd_panel) {
        esp_lcd_panel_del(lcd_panel);
    }
    if (lcd_io) {
        esp_lcd_panel_io_del(lcd_io);
    }
    spi_bus_free(EXAMPLE_LCD_SPI_NUM);
    return ret;
}

static esp_err_t app_lvgl_init(void)
{
    /* Initialize LVGL */
    const lvgl_port_cfg_t lvgl_cfg = {
        .task_priority = 4,         /* LVGL task priority */
        .task_stack = 4096*2,         /* LVGL 任务堆栈大小*/
        .task_affinity = -1,        /* LVGL task pinned to core (-1 is no affinity) */
        .task_max_sleep_ms = 500,   /* Maximum sleep in LVGL task */
        .timer_period_ms = 5        /* LVGL timer tick period in ms */
    };
    ESP_RETURN_ON_ERROR(lvgl_port_init(&lvgl_cfg), TAG, "LVGL port initialization failed");

    /* Add LCD screen */
    ESP_LOGD(TAG, "Add LCD screen");
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = lcd_io,
        .panel_handle = lcd_panel,
        .buffer_size = EXAMPLE_LCD_H_RES * EXAMPLE_LCD_DRAW_BUFF_HEIGHT,
        .double_buffer = true,
        .hres = EXAMPLE_LCD_H_RES,
        .vres = EXAMPLE_LCD_V_RES,
        .monochrome = false,//单色
        .color_format = LV_COLOR_FORMAT,
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        },
        .flags = {
            .buff_dma = false,
            .swap_bytes = true,
            .buff_spiram = true,
            .sw_rotate = false,// true: 软件; false: 硬件
            .full_refresh = true,
            .direct_mode = false,
        }
    };
    lvgl_disp = lvgl_port_add_disp(&disp_cfg);

    //lv_disp_set_rotation(lvgl_disp, LV_DISP_ROTATION_90);//旋转90度

    return ESP_OK;
}

static void app_main_display(void)
{
    /* Task lock */
    lvgl_port_lock(0);

    // lv_demo_music();
    lv_demo_widgets();
    // example_lvgl_demo_ui(lvgl_disp);

    /* Task unlock */
    lvgl_port_unlock();
}

void app_main(void)
{
    /* LCD HW initialization */
    ESP_ERROR_CHECK(app_lcd_init());

    /* LVGL initialization */
    ESP_ERROR_CHECK(app_lvgl_init());

    /* Show LVGL objects */
    app_main_display();
}
