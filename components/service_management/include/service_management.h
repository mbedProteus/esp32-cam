#ifndef __SERVICE_MANAGEMENT__
#define __SERVICE_MANAGEMENT__

typedef int (*init_function_t)(void);

int register_service(init_function_t func,
                     const char *service_name,
                     int priority);


int service_start(void);

#endif // __SERVICE_MANAGEMENT__