/*  Weather Station Node
    The sensors read atmospheric pressure, temperature and relative humidity.
    All data is sent to a MQTT broker using WiFi.
*/
#include <stdio.h>
#include <string.h>

#include "wifi_handler.h"
#include "mqtt_handler.h"
#include "station_sensors.h"

void app_main(void)
{
    /*Initialize NVS */

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* Initialize WiFi*/

    wifi_init_sta();

    /* Initialize MQTT */

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.hostname = CONFIG_BROKER_URL,
        .broker.address.transport = MQTT_TRANSPORT_OVER_TCP,
        .broker.address.port = CONFIG_BROKER_PORT,
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

    /* Initialize I2C */

    i2c_master_init();

    /* Initialize GPIO */

    gpio_init();

    /* Initialize BMP280 */
 
    bmx280_t* bmx280 = bmx280_create(I2C_MASTER_NUM);

    if (!bmx280) { 
        ESP_LOGE("test", "Could not create bmx280 driver.");
        return;
    }

    ESP_ERROR_CHECK(bmx280_init(bmx280));
    bmx280_config_t bmx_cfg = BMX280_DEFAULT_CONFIG;
    ESP_ERROR_CHECK(bmx280_configure(bmx280, &bmx_cfg));

    /* Initialize DHT22 */

    setDHTgpio(GPIO_DHT_PIN);

    while (1)
    {   
        int minutes_passed = 0;
        struct sensor_data data = {0};
        bmx280_setMode(bmx280, BMX280_MODE_FORCE);
        do {
            vTaskDelay(pdMS_TO_TICKS(2000));
        } while(bmx280_isSampling(bmx280));

        float abs_pressure;
        bmx280_readoutFloat(bmx280,
                            &data.temperature_bmp280,
                            &abs_pressure,
                            &data.humidity);
        data.pressure = absolute_to_relative_pressure(abs_pressure,
                                                      LOCAL_ALTITUDE,
                                                      data.temperature_bmp280);

        /* DHT22 usually loses data. Read a few times just in case. */

        int iter = 0;
		do 
        {
            int ret = readDHT();
            errorHandler(ret);
            /* Must wait 3 seconds before retrying */
            vTaskDelay(pdMS_TO_TICKS(3000));
            iter++;
        } while ( (ret != DHT_OK) && (iter < DHT_TIMEOUT_RETRIES) );

        data.humidity = getHumidity();
        data.temperature_dht22 = getTemperature();

        /* Log to serial */

        ESP_LOGI("main", "temp_dht22 = %f, temp_bmp280 = %f pres = %f, hum = %f",
                 data.temperature_dht22, data.temperature_bmp280, data.pressure,
                 data.humidity);

        /* Publish with MQTT */

        char buf[MQTT_MSG_BUF_SIZE];

        snprintf(buf, MQTT_MSG_BUF_SIZE, "%.4f", data.temperature_bmp280);        
        esp_mqtt_client_publish(client, "home_station/temp_bmp280", buf, 0, 0, 0);

        snprintf(buf, MQTT_MSG_BUF_SIZE, "%.4f", data.temperature_dht22);        
        esp_mqtt_client_publish(client, "home_station/temp_dht22", buf, 0, 0, 0);

        snprintf(buf, MQTT_MSG_BUF_SIZE, "%.4f", data.pressure);        
        esp_mqtt_client_publish(client, "home_station/pressure", buf, 0, 0, 0);

        snprintf(buf, MQTT_MSG_BUF_SIZE, "%.4f", data.humidity);        
        esp_mqtt_client_publish(client, "home_station/humidity", buf, 0, 0, 0);

        for (int min_passed=0; min_passed < STATION_REFRESH_WAIT_MIN; min_passed++)
        {
            vTaskDelay(pdMS_TO_TICKS(1000 * 60));
        }

    }

}
