#ifndef __WIFI_CONFIG__
#define __WIFI_CONFIG__

#include "sdkconfig.h"


#define DEFAULT_SCAN_LIST_SIZE 10

#ifndef CONFIG_RETRY_CONNECT
#define RETRY_CONNECT 5
#else
#define RETRY_CONNECT CONFIG_RETRY_CONNECT
#endif

#ifndef CONFIG_STA_SSID
#define STA_SSID ""
#else
#define STA_SSID CONFIG_STA_SSID
#endif

#ifndef CONFIG_STA_PASSPHRASE
#define STA_PASSPHRASE ""
#else
#define STA_PASSPHRASE CONFIG_STA_PASSPHRASE
#endif

typedef enum {
    WIFI_STATE_STARTED,
    WIFI_STATE_DISCONNECTED,
    WIFI_STATE_CONNECTED,
    WIFI_STATE_RETRYING
} wifi_state_t;


void sta_init(void *args);
wifi_state_t wifi_config_get_current_state(void);
void wifi_config_retry(void);

#endif // __WIFI_CONFIG__