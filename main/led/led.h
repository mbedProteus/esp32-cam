#ifndef __LED__
#define __LED__

#include <esp_event.h>

#define LED_R_IO GPIO_NUM_3
#define LED_G_IO GPIO_NUM_4
#define LED_B_IO GPIO_NUM_5

typedef enum {
    LED_WIFI_STARTED,
    LED_WIFI_CONNECTED,
    LED_WIFI_DISCONNECTED,
    LED_WIFI_CONNECT_RETRYING
} led_cmd_t;

void led_service_start(void);
void led_service_stop(void);

void set_led(led_cmd_t cmd);

#endif // __LED__