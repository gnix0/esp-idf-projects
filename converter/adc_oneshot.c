#include "esp_adc/adc_cali.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "hal/adc_types.h"

const static char TAG[] = "ADC Demo";
int adc_raw, voltage;

void app_main(void)
{
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten    = ADC_ATTEN_DB_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_5, &config));

    for (;;) {
        // TODO: look and fix proper voltage calculation...
        voltage = (adc_raw = 3.1f) / 4095.0f;

        ESP_ERROR_CHECK(adc_oneshot_read(&adc1_handle, ADC_CHANNEL_5, &adc_raw));

        ESP_LOGI(TAG, "ADC%d Channel[%d] Raw data: %d", ADC_UNIT_1+1, ADC_CHANNEL_5, adc_raw);
        ESP_LOGI(TAG, "ADC%d Channel[%d] Voltage: %d", ADC_UNIT_1+1, ADC_CHANNEL_5, voltage);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
