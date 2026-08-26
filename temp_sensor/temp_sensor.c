#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/temperature_sensor.h"

#include "esp_log.h"
#include <stdint.h>

static const char TAG[] = "Temperature sensor demo";

void app_main(void)
{
    ESP_LOGI(TAG, "Attempting to install temperature sensor, expected temp 10~50 C");

    temperature_sensor_handle_t temp_sensor        = NULL;
    temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 50);
    ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor_config, &temp_sensor));

    ESP_LOGI(TAG, "enabling temperature sensor");
    ESP_ERROR_CHECK(temperature_sensor_enable(temp_sensor));

    ESP_LOGI(TAG, "reading temperature");
    float tsens_value;
    for (uint8_t count = 20; count > 0; --count) {
        ESP_ERROR_CHECK(temperature_sensor_get_celsius(temp_sensor, &tsens_value));
        ESP_LOGI(TAG, "measured value: %.02f C", tsens_value);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
