#include "led.h"

#include <esp_system.h>
#include <inttypes.h>
#include <driver/gpio.h>
// #include <driver/ledc.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>
#include <event.h>
#include <string.h>

#define TAG "led"

#define MAX_DUTY 8192 // = 2 ** 13

#define RED_CHANNEL LEDC_CHANNEL_0
#define GREEN_CHANNEL LEDC_CHANNEL_1
#define BLUE_CHANNEL LEDC_CHANNEL_2

typedef struct {
    uint32_t r_duty;
    uint32_t g_duty;
    uint32_t b_duty;
} rgb_duty_t;

void wifi_led_status_callback(char *value)
{
    ESP_LOGI(TAG, "Value: %s", value);
    if (strcmp(value, "disconnected") == 0)
    {
        gpio_set_level(LED_R_IO, 1);
        gpio_set_level(LED_B_IO, 0);
        gpio_set_level(LED_G_IO, 0);
    }
    else if (strcmp(value, "connected") == 0)
    {
        /* Green */
        gpio_set_level(LED_R_IO, 0);
        gpio_set_level(LED_B_IO, 0);
        gpio_set_level(LED_G_IO, 1);
    }
    else if (strcmp(value, "retry") == 0)
    {
        /* Red */
        gpio_set_level(LED_R_IO, 1);
        gpio_set_level(LED_B_IO, 1);
        gpio_set_level(LED_G_IO, 0);
    }
    else if (strcmp(value, "started") == 0)
    {
        /* Yellow */
        gpio_set_level(LED_R_IO, 1);
        gpio_set_level(LED_B_IO, 0);
        gpio_set_level(LED_G_IO, 1);
    }
}

int led_init(void)
{
    gpio_config_t io_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pin_bit_mask = ((1UL << LED_R_IO) | (1UL << LED_B_IO) | (1UL << LED_G_IO))
    };

    gpio_config(&io_config);

    event_context evctx = register_event("wifi-status", &wifi_led_status_callback);
    if (evctx != NULL) {
        ESP_LOGI(TAG, "Registered event with id : %"PRIu32, (uint32_t)evctx);
    }

    return 0;
}