#include "config.h"
#include <nvs_flash.h>
#include <string.h>
#include <esp_log.h>

#define TAG "system_config"

#define NVS_NAMESPACE_CONFIG "config"

static nvs_handle_t _nvs_handler;

int init_config(void)
{
    esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, "nvs_config");
    if (it != NULL) {
        esp_partition_t *part = esp_partition_get(it);
        if (part != NULL)
            ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_flash_init_partition_ptr(part));
    } else {
        ESP_LOGE(TAG, "Partition is not found");
    }
    return 0;
}

int set_config(const char *key,
               config_type_t type,
               void *value,
               size_t size)
{
    // if (nvs_get)

    return 0;
}
