#include "led.h"

#include <esp_system.h>
#include <inttypes.h>
#include <driver/gpio.h>
// #include <driver/ledc.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>

#define TAG "led"

#define MAX_DUTY 8192 // = 2 ** 13

#define RED_CHANNEL LEDC_CHANNEL_0
#define GREEN_CHANNEL LEDC_CHANNEL_1
#define BLUE_CHANNEL LEDC_CHANNEL_2


// static QueueHandle_t led_queue = NULL;

// static TaskHandle_t led_service_task;
ESP_EVENT_DEFINE_BASE(LED_EVENTS);

static esp_event_handler_instance_t _led_event_instance;
// static volatile bool _is_started = false;

typedef struct {
    uint32_t r_duty;
    uint32_t g_duty;
    uint32_t b_duty;
} rgb_duty_t;

static void led_event_handler(void *handler_args,
                         esp_event_base_t event_base,
                         int32_t event_id,
                         void * event_data) {

    ESP_LOGI(TAG, "Receive %"PRId32" cmd", event_id);
    
    if (event_base == LED_EVENTS)
    {
        switch (event_id)
        {
        case LED_WIFI_STARTED:
            /* Yellow */
            gpio_set_level(LED_R_IO, 1);
            gpio_set_level(LED_B_IO, 0);
            gpio_set_level(LED_G_IO, 1);
            break;
        case LED_WIFI_CONNECTED:
            /* Green */
            gpio_set_level(LED_R_IO, 0);
            gpio_set_level(LED_B_IO, 0);
            gpio_set_level(LED_G_IO, 1);
            break;
        case LED_WIFI_DISCONNECTED:
            gpio_set_level(LED_R_IO, 1);
            gpio_set_level(LED_B_IO, 0);
            gpio_set_level(LED_G_IO, 0);
        break;
        case LED_WIFI_CONNECT_RETRYING:
            /* Red */
            gpio_set_level(LED_R_IO, 1);
            gpio_set_level(LED_B_IO, 1);
            gpio_set_level(LED_G_IO, 0);
            break;
        default:
            gpio_set_level(LED_R_IO, 0);
            gpio_set_level(LED_B_IO, 0);
            gpio_set_level(LED_G_IO, 0);
            break;
        }
    }
}

static void led_init(void)
{
    gpio_config_t io_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pin_bit_mask = ((1UL << LED_R_IO) | (1UL << LED_B_IO) | (1UL << LED_G_IO))
    };


    // ledc_timer_config_t ledc_timer = {
    //     .clk_cfg = LEDC_AUTO_CLK,
    //     .duty_resolution = LEDC_TIMER_14_BIT,
    //     .freq_hz = 5000,
    //     .speed_mode = LEDC_LOW_SPEED_MODE,
    //     .timer_num = LEDC_TIMER_0
    // };
    // ledc_timer_config(&ledc_timer);

    // ledc_channel_config_t channel_config = {
    //     .channel = LEDC_CHANNEL_0,
    //     .duty = 0,
    //     .hpoint = 0,
    // }

    gpio_config(&io_config);
    esp_event_handler_instance_register(LED_EVENTS,
                                        ESP_EVENT_ANY_ID,
                                        &led_event_handler,
                                        NULL,
                                        &_led_event_instance);

}

void led_service_start(void) {
    led_init();
}

void led_service_stop(void) {
    esp_event_handler_instance_unregister(LED_EVENTS, ESP_EVENT_ANY_ID, _led_event_instance);
}

void set_led(led_cmd_t cmd) {
    ESP_LOGI(TAG, "Start send %d cmd", cmd);
    esp_event_post(LED_EVENTS, (int32_t)cmd, NULL, 0, portMAX_DELAY);
}