
#ifndef __EVENT__
#define __EVENT__
#include <esp_event.h>

typedef void (*callback_func_t)(char* value);
typedef void* event_context;


int event_init(void);
char* get_event_value(const char* key);
void set_event(const char* key, const char* value);
void show_event(void);
event_context register_event(const char *key, callback_func_t cb);
void unregiter_event(event_context ctx);
#endif  //__EVENT__