
#ifndef __EVENT__
#define __EVENT__
#include <esp_event.h>

ESP_EVENT_DEFINE_BASE(SYS_EVENT);

typedef struct
{
    char *key;
    char *value;
} event_t;

int event_init(void);
char *get_event_value(const char *key);
void set_event(const char* key, const char* value);
void show_event(void);

#endif //__EVENT__