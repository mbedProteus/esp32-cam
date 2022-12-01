#include "service_management.h"
#include <string.h>
#include <esp_log.h>

#define TAG "Service Management"

static struct service {
    init_function_t func;
    const char* service_name;
    int enabled;
    int started;
    int priority;
    struct service * next;
    struct service * prev;
};

static struct service *service_list = NULL;

int register_service(init_function_t func,
                     const char *service_name,
                     int priority,
                     int enabled)
{
    struct service * ptr_service = NULL;
    ptr_service = (struct service *)malloc(sizeof(struct service));

    ESP_LOGI(TAG, "registering %s service", service_name);

    ptr_service->func = func;
    ptr_service->priority = priority;
    ptr_service->service_name = strdup(service_name);
    ptr_service->next = NULL;
    ptr_service->prev = NULL;
    ptr_service->enabled = enabled;
    ptr_service->started = 0;

    if (service_list == NULL) {
        service_list = ptr_service;
        return 0;
    }

    for (struct service * it = service_list; it != NULL; it = it->next) {
        if (ptr_service->priority >= it->priority) {
            ptr_service->next = it->next;
            it->next = ptr_service;
            ptr_service->prev = it;
            break;
        }
    }

    if (ptr_service->prev == NULL && ptr_service != service_list) {
        struct service * it = service_list;

        if (ptr_service->priority >= service_list->priority)
        {
            while (it->next != NULL)
            {
                it = it->next;
            }
            it->next = ptr_service;
            ptr_service->prev = it;
        } else {
            ptr_service->next = service_list;
            service_list->prev = ptr_service;
            service_list = ptr_service;
        }
    }

    return 0;
}

int service_auto_start(void) {
    for (struct service *index = service_list; index != NULL; index = index->next)
    {
        if (index->enabled)
        {
            if (index->func() == 0)
            {
                ESP_LOGI(TAG, "Start %s successfully", index->service_name);
            }
            else
            {
                ESP_LOGE(TAG, "Fail to start service - %s", index->service_name);
                return -1;
            }
        }
    }

    return 0;
}

