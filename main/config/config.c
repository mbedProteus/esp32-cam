#include "config.h"
#include <nvs_flash.h>
#include <nvs.h>
#include <string.h>
#include <esp_log.h>

#define TAG "system_config"

#define CONFIG_PARTITION_NAME "config"

int init_config(void)
{
    nvs_handle_t handler;
    // esp_err_t ret = nvs_open_from_partition("nvs", "wifi-ssid", NVS_READWRITE, &handler);
    esp_err_t ret = nvs_open("config", NVS_READWRITE, &handler);

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error: %s", esp_err_to_name(ret));
        return -1;
    }
    char wifi_ssid[40] = {0};
    size_t wifi_ssid_size = sizeof(wifi_ssid);
    nvs_get_str(handler, "wifi-ssid", &wifi_ssid, &wifi_ssid_size);
    if (strlen(wifi_ssid) != 0 && strcmp(wifi_ssid, "") != 0)
    {
        ESP_LOGI(TAG, "SSID: %s", wifi_ssid);
    }
    else
    {
        nvs_set_str(handler, "wifi-ssid", CONFIG_STA_SSID);
    }

    nvs_commit(handler);
    nvs_close(handler);
    return 0;
}
