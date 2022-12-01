
set(sources 
    "main.c"
    "wifi/wifi_config.c"
    "led/led.c"
    "config/config.c"
)

set(private_incs 
"."
"wifi"
"led"
"config")


idf_component_register(SRCS ${sources}
PRIV_INCLUDE_DIRS ${private_incs})