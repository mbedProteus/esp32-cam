#include <stdio.h>
#include "wifi_config.h"
#include "freertos/FreeRTOS.h"
#include <freertos/task.h>
#include <nvs_flash.h>
#include <esp_log.h>
#include "led.h"

#define TAG "main"

void app_main(void) {
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    led_service_start();
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ret = nvs_flash_erase();
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);
    vTaskDelay(1000/portTICK_PERIOD_MS);
    set_led(LED_WIFI_DISCONNECTED);
    sta_init(NULL);

    int count = 0;
    int max_try = 60;
    wifi_state_t state = WIFI_STATE_DISCONNECTED;

    while (1)
    {
        vTaskDelay(1000/portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "Loop!!!!!!!!");
        if (++count == max_try) {
            state = wifi_config_get_current_state();
            switch (state)
            {
            case WIFI_STATE_CONNECTED:
                count = 0;
                break;
            case WIFI_STATE_RETRYING:
            case WIFI_STATE_STARTED:
                break;
            case WIFI_STATE_DISCONNECTED:
                max_try = max_try >= 120 ? 120 : max_try + 10;
                count = 0;
                break;
            default:
                break;
            }
        }
    }
}
