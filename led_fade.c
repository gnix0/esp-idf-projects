#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/ledc.h"

#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"

#include <stdbool.h>
#include <stdint.h>

#define LED1_PIN        GPIO_NUM_23
#define LEDC_LS_TIMER   LEDC_TIMER_0
#define LEDC_LS_MODE    LEDC_LOW_SPEED_MODE
#define LEDC_LS_CHANNEL LEDC_CHANNEL_0

#define ADC_CHAN  ADC_CHANNEL_5
#define ADC_UNIT  ADC_UNIT_1
#define ADC_ATTEN ADC_ATTEN_DB_12

#define PWM_MAX_DUTY ((1U << 13) - 1)

#define MAX_VALUE 3000

static const char TAG[] = "LEDC_DIM_ADC_PWM";

static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten,
                                 adc_cali_handle_t *out_handle);

int adc_raw, voltage;

void app_main(void)
{
    esp_err_t err;

    ledc_timer_config_t timer_config = {
        .duty_resolution = LEDC_TIMER_13_BIT,
        .freq_hz         = 4000,
        .speed_mode      = LEDC_LS_MODE,
        .timer_num       = LEDC_LS_TIMER,
        .clk_cfg         = LEDC_AUTO_CLK,
    };
    err = ledc_timer_config(&timer_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "timer configuration failed for GPIO pin %d", LED1_PIN);
        abort();
    }

    ledc_channel_config_t channel_config = {
        .channel    = LEDC_LS_CHANNEL,
        .speed_mode = LEDC_LS_MODE,
        .timer_sel  = LEDC_LS_TIMER,
        .intr_type  = LEDC_INTR_DISABLE,
        .gpio_num   = LED1_PIN,
        .duty       = 0,
        .hpoint     = 0,
    };
    err = ledc_channel_config(&channel_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "channel configuration failed for GPIO pin %d", LED1_PIN);
        abort();
    }

    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT,
    };
    err = adc_oneshot_new_unit(&init_config, &adc_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to initialize ADC unit");
        abort();
    }

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten    = ADC_ATTEN,
    };
    err = adc_oneshot_config_channel(adc_handle, ADC_CHAN, &config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to configure ADC channel %d", ADC_CHAN);
        abort();
    }

    adc_cali_handle_t adc_cali_chan_handle = NULL;
    bool do_calibration_chan = adc_calibration_init(ADC_UNIT, ADC_CHAN, ADC_ATTEN,
                                                    &adc_cali_chan_handle);

    for (;;) {
        err = adc_oneshot_read(adc_handle, ADC_CHAN, &adc_raw);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "failure reading ADC channel %d", ADC_CHAN);
            abort();
        }

        if (do_calibration_chan) {
            err = adc_cali_raw_to_voltage(adc_cali_chan_handle, adc_raw, &voltage);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "failure processing ADC channel %d", ADC_CHAN);
                abort();
            }
        }

        uint32_t duty = ((uint32_t)voltage * PWM_MAX_DUTY) / MAX_VALUE;

        if (duty > PWM_MAX_DUTY)
            duty = PWM_MAX_DUTY;

        err = ledc_set_duty(LEDC_LS_MODE, LEDC_LS_CHANNEL, duty);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "failure setting LEDC duty");
            abort();
        }

        err = ledc_update_duty(LEDC_LS_MODE, LEDC_LS_CHANNEL);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "failure updating LEDC duty");
            abort();
        }

        ESP_LOGI(TAG, "Voltage: %d mV | Duty: %lu", voltage, (unsigned long)duty);

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten,
                                 adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle     = NULL;
    esp_err_t         err        = ESP_FAIL;
    bool              calibrated = false;

    if (!calibrated) {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Curve Fitting");
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id  = unit,
            .chan     = channel,
            .atten    = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        err = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
        if (err == ESP_OK)
            calibrated = true;
    }

    *out_handle = handle;
    if (err == ESP_OK)
        ESP_LOGI(TAG, "Calibration success");
    else if (err == ESP_ERR_NOT_SUPPORTED || !calibrated)
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    else
        ESP_LOGE(TAG, "Invalid argument or insufficient memory");

    return calibrated;
}
