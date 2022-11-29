#include "wifi_config.h"
#include <esp_wifi.h>
#include <esp_system.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_event.h>
#include <lwip/err.h>
#include <lwip/sys.h>
#include "led.h"
#include "event.h"


#define TAG "wifi"

static int number_retry = 0;
static wifi_state_t _current_state = WIFI_STATE_DISCONNECTED;

static void wifi_event_handler(void *event_handler_arg,
                               esp_event_base_t event_base,
                               int32_t event_id,
                               void *event_data)
{
    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
        case WIFI_EVENT_STA_START:
            ESP_LOGD(TAG, "Wifi started");
            set_led(LED_WIFI_STARTED);
            esp_wifi_connect();
            esp
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            if (number_retry < RETRY_CONNECT)
            {
                set_led(LED_WIFI_CONNECT_RETRYING);
                vTaskDelay(2000 / portTICK_PERIOD_MS);
                esp_wifi_connect();
                number_retry++;
                ESP_LOGI(TAG, "retrying to connect");
            }
            else
            {
                set_led(LED_WIFI_DISCONNECTED);
                ESP_LOGI(TAG, "connect to the AP fail");
            }
            break;
        case WIFI_EVENT_STA_CONNECTED:
            set_led(LED_WIFI_CONNECTED);
            ESP_LOGI(TAG, "connected to %s", STA_SSID);
            break;
        default:
            ESP_LOGI(TAG, "This event haven't handled - %ld", event_id);
            break;
        }
    }
    else if (event_base == IP_EVENT)
    {
        if (event_id == IP_EVENT_STA_GOT_IP)
        {
            ip_event_got_ip_t *ip_event = (ip_event_got_ip_t *)event_data;
            ESP_LOGI(TAG, "got ip" IPSTR, IP2STR(&ip_event->ip_info.ip));
            number_retry = 0;
        }
    }
}

int sta_init(void)
{
    ESP_ERROR_CHECK(esp_netif_init());

    // esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t init_config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&init_config);

    esp_event_handler_instance_t any_id;
    esp_event_handler_instance_t got_ip;

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        &wifi_event_handler,
                                        NULL,
                                        &any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                        IP_EVENT_STA_GOT_IP,
                                        &wifi_event_handler,
                                        NULL,
                                        &got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = STA_SSID,
            .password = STA_PASSPHRASE,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH},
    };

    ESP_LOGI(TAG, "Start connect to %s network", STA_SSID);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    return 0;
}

wifi_state_t wifi_config_get_current_state(void) {
    return _current_state;
}

void wifi_config_retry(void) {
    if (_current_state == WIFI_STATE_DISCONNECTED)
        esp_event_post(WIFI_EVENT, WIFI_EVENT_STA_START, NULL, 0, portMAX_DELAY);
}