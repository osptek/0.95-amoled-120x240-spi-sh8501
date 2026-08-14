#include "esp_err.h"
#include "esp_lvgl_port.h"
#include "lv_demos.h"

#include "display_lvgl.h"

void app_main(void) {
    ESP_ERROR_CHECK(display_lvgl_init());

    lvgl_port_lock(0);
    //lv_demo_widgets();
    lv_demo_stress();
    lvgl_port_unlock();
}
