
set(sources 
    "main.c"
    "wifi/wifi_config.c"
    "led/led.c"
)

set(private_incs 
"."
"wifi"
"led")


idf_component_register(SRCS ${sources}
PRIV_INCLUDE_DIRS ${private_incs})