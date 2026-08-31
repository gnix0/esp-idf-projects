#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"

#define LED_PIN      GPIO_NUM_5
#define BTN_PIN      GPIO_NUM_3
#define PWM_MAX_DUTY ((1U << 13) - 1)

static const char TAG_LEDC[]    = "LEDC Task:";
static const char TAG_MONITOR[] = "Monitoring Task:";

uint32_t duty = 0;

typedef struct {
    ledc_mode_t      speed_mode;
    ledc_channel_t   channel;
    uint32_t         target_duty;
    uint32_t         scale;
    uint32_t         cycle_num;
    ledc_fade_mode_t fade_mode;
} Fade_t;

Fade_t fade_params = {
    LEDC_LOW_SPEED_MODE,
    LEDC_CHANNEL_0,
    PWM_MAX_DUTY,
    PWM_MAX_DUTY / 8,
    500,
    LEDC_FADE_NO_WAIT,
};

TaskHandle_t xTaskLedcHandle    = NULL;
TaskHandle_t xTaskMonitorHandle = NULL;

void vTaskLedc(void *pvParameters);
void vTaskMonitor(void *pvParameters);

void app_main(void)
{
    gpio_config_t btn_config = {
        .pin_bit_mask = (1UL << BTN_PIN),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = true,
        .pull_down_en = false,
        .intr_type    = GPIO_INTR_DISABLE,
    };

    ESP_ERROR_CHECK(gpio_config(&btn_config));

    xTaskCreate(vTaskLedc, "TASK_LED_FADE", 2048, NULL,
                1, &xTaskLedcHandle);
    xTaskCreate(vTaskMonitor, "TASK_MONITOR", 2048, NULL,
                1, &xTaskMonitorHandle);
}

void vTaskLedc(void *pvParameters)
{
    esp_err_t err = 0;
    ESP_LOGI(TAG_LEDC, "assembly LEDC configs for LED[%d]", LED_PIN);

    ledc_timer_config_t timer_config = {
        .duty_resolution = LEDC_TIMER_13_BIT,
        .freq_hz         = 2000,
        .speed_mode      = LEDC_LOW_SPEED_MODE,
        .timer_num       = LEDC_TIMER_0,
        .clk_cfg         = LEDC_AUTO_CLK,
    };
    err = ledc_timer_config(&timer_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_LEDC, "timer configuration failed for LED[%d]", LED_PIN);
        vTaskDelete(NULL);
        abort();
    }

    ledc_channel_config_t channel_config = {
        .channel    = LEDC_CHANNEL_0,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_sel  = LEDC_TIMER_0,
        .intr_type  = LEDC_INTR_DISABLE,
        .gpio_num   = LED_PIN,
        .duty       = 0,
        .hpoint     = 0,
    };
    err = ledc_channel_config(&channel_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_LEDC, "channel configuration failed for LED[%d]", LED_PIN);
        vTaskDelete(NULL);
        abort();
    }

    ESP_ERROR_CHECK(ledc_fade_func_install(0));

    for (;;) {
        bool btn_status = gpio_get_level(BTN_PIN);

        duty = ledc_get_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

        if (btn_status == 0) {
            err = ledc_set_fade_step_and_start(fade_params.speed_mode, fade_params.channel,
                                               fade_params.target_duty, fade_params.scale,
                                               fade_params.cycle_num, fade_params.fade_mode);
            if (err != ESP_OK) {
                ESP_LOGE(TAG_LEDC, "fading failed for LED[%d]", LED_PIN);
                vTaskDelete(NULL);
                abort();
            }
        } else if (btn_status == 1) {
            err = ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0,
                                               0, 2000,
                                               LEDC_FADE_NO_WAIT);
            if (err != ESP_OK) {
                ESP_LOGE(TAG_LEDC, "reset fading failed for LED[%d]", LED_PIN);
                vTaskDelete(NULL);
                abort();
            }
        }
    }
}

void vTaskMonitor(void *pvParameters)
{
    esp_err_t err = 0;
    ESP_LOGI(TAG_MONITOR, "initializing monitor task");

    for (;;) {
        ESP_LOGI(TAG_LEDC, "button = %d", gpio_get_level(BTN_PIN));
        if (duty >= PWM_MAX_DUTY) {
            vTaskSuspend(xTaskLedcHandle);

            err = ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
            if (err != ESP_OK) {
                ESP_LOGE(TAG_MONITOR, "monitor task failed to set duty cycle");
                abort();
            }

            err = ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            if (err != ESP_OK) {
                ESP_LOGE(TAG_MONITOR, "monitor task failed to update duty cicle");
                abort();
            }

            vTaskDelay(pdMS_TO_TICKS(2000));

            vTaskResume(xTaskLedcHandle);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
