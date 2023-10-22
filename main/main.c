/*  Weather Station Node
    The sensors read atmospheric pressure, temperature and relative humidity.
    All data is sent to a MQTT broker using WiFi.
*/
#include <stdio.h>
#include <string.h>

#include "wifi_handler.h"
#include "mqtt_handler.h"


void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifi_init_sta();

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.hostname = CONFIG_BROKER_URL,
        .broker.address.transport = MQTT_TRANSPORT_OVER_TCP,
        .broker.address.port = CONFIG_BROKER_PORT,
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

    for (int i = 10; i >= 0; i--) {
        printf("iteration %d...\n", i);
        esp_mqtt_client_publish(client, "/topic/home_station", "val val", 0, 0, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

}
