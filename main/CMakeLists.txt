# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "main.c"
                   "wifi_handler.c"                    
                   "mqtt_handler.c"
               )

register_component()
