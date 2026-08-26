/*
 * IMPORTANT:
 * This repository uses the ESP32-C6 SoC as reference.
 * This specific SoC from Espressif does not contain a built-in digital to analog converter, therefore,
 * the code present here is only meant as a reference/starter and was not test on real hardware.
*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/dac_cosine./"
#include "esp_log.h"

static const char TAG[] = "DAC cosine wave demo";

void app_main(void)
{
    dac_consine_handle_t chan0_handle;
    dac_cosine_config_t cos0_cfg = {
        .chan_id               = DAC_CHAN_0,			// DAC channel 0
        .freq_hz               = 1000,				// 1 kHz
        .clk_src               = DAC_COSINE_CLK_SRC_DEFAULT,	// default clock source
        .offset                = 0,				// 0 V
        .phase                 = DAC_COSINE_PHASE_0,		// 0 degrees
        .atten                 = DAC_COSINE_ATTEN_DEFAULT,	// default attenuation
        .flags.source_set_freq = false,				// don't force frequency
    };

    ESP_LOGI(TAG, "initializing DAC cosine wave generator...");

    ESP_ERR_CHECK(&cos0_cfg, &chan0_handle));
    ESP_ERR_CHECK(dac_cosine_start(chan0_handle));

    ESP_LOGI(TAG, "DAC cosine wave generator initialized");
}
