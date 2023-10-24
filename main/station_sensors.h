#ifndef STATION_SENSORS_H
#define STATION_SENSORS_H

#include "driver/i2c.h"
#include "driver/gpio.h"
#include "bmx280.h"
#include "DHT22/main/DHT22.h"

#define I2C_MASTER_SCL_IO           9       /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           8       /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0       /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          100000  /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0       /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0       /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       5000

#define GPIO_DHT_PIN                18      /* GPIO Pin for DHT22 */
#define GPIO_INPUT_PIN_SEL          (1ULL << GPIO_DHT_PIN)

esp_err_t i2c_master_init(void);
void gpio_init(void);

#endif  // STATIONS_SENSORS_H
