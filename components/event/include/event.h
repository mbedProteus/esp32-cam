
#ifndef __EVENT__
#define __EVENT__
#include <esp_event.h>

typedef void (*callback_func_t)(void* arg);

int event_init(void);
char* get_event_value(const char* key);
void set_event(const char* key, const char* value);
void show_event(void);

#endif  //__EVENT__