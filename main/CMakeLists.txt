
set(sources 
    "main.c"
    "wifi/wifi_config.c"
)

set(private_incs 
"."
"wifi")


idf_component_register(SRCS ${sources}
PRIV_INCLUDE_DIRS ${private_incs})