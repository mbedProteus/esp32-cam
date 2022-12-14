#include <stdio.h>
#include "wifi_config.h"
#include "freertos/FreeRTOS.h"
#include <freertos/task.h>
#include <nvs_flash.h>
#include <esp_log.h>
#include "led.h"
#include "service_management.h"
#include "event.h"
#include <esp_wifi.h>

#define TAG "main"

void init_nvs() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);
}

void mount_storage () {
    init_nvs();
}

void init_service() {
    register_service(&sta_init, "STA", 2, 1);
    register_service(&event_init, "Event", 1, 1);
    register_service(&led_init, "LED", 3, 0);
}

void app_main(void) {
    mount_storage();
    init_service();

    service_auto_start();
}
