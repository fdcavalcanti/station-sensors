# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "main.c"
                   "wifi_handler.c"                    
                   "mqtt_handler.c"
                   "station_sensors.c"
                   "DHT22/main/DHT22.c"
               )

set(INCLUDE_DIRS "DHT22/main/")
register_component()
