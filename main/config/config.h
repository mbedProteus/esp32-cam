#ifndef __CONFIG__
#define __CONFIG__

#include <nvs.h>

typedef enum {
    CONFIG_TYPE_NUM,
    CONFIG_TYPE_STRING
} config_type_t;

int init_config(void);

int set_config(const char *key,
               config_type_t type,
               void *value,
               size_t size);
int get_config(const char *key,
               config_type_t type,
               void *out,
               size_t *out_size);
int commit_config(void);
#endif // __CONFIG__