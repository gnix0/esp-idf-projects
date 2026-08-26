/*
 * IMPORTANT:
 * This repository uses the ESP32-C6 SoC as reference.
 * This specific SoC from Espressif does not contain a built-in digital to analog converter, therefore,
 * the code present here is only meant as a reference/starter and was not test on real hardware.
*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/dac_oneshot.h"
#include "esp_log.h"

#include <stdint.h>

static const char TAG[] = "DAC oneshot demo";

void app_main(void)
{
    uint8_t val = 0;

    dac_oneshot_handle_t chan0_handle;			// DAC channel 0 handle
    dac_oneshot_config_t chan0_cfg = {			// DAC channel 0 configuration
        .chan_id = DAC_CHAN_0,				// DAC channel 0
    };
    ESP_ERROR_CHECK(dac_oneshot_new_channel(&chan0_cfg, &chan0_handle));

    ESP_LOGI(TAG, "DAC oneshot example starting");

    for (;;) {
        for (val = 0; val < 255; ++val) {
            ESP_ERROR_CHECK(dac_oneshot_output_voltage(chan0_handle_val));
            vTaskDelay(pdMS_TO_TICKS1);
        }

        for (val = 0; val < 255; --val) {
            ESP_ERROR_CHECK(dac_oneshot_output_voltage(chan0_handle_val));
            vTaskDelay(pdMS_TO_TICKS1);
        }
    }
}
