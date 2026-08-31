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

TaskHandle_t xTaskLedcHandle    = NULL;
TaskHandle_t xTaskMonitorHandle = NULL;

void vTaskLedc(void *pvParameters);
void vTaskMonitor(void *pvParameters);

void app_main(void)
{
    gpio_config_t btn_config = {
        .pin_bit_mask = (1ULL << BTN_PIN),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = 0,
        .pull_down_en = 1,
        .intr_type    = GPIO_INTR_DISABLE,
    };

    ESP_ERROR_CHECK(ledc_fade_func_install(0));
}

void vTaskLedc(void *pvParameters)
{
    esp_err_t err;
    ESP_LOGI(TAG_LEDC, "assembly LEDC configs for LED[%d]", LED_PIN);

    ledc_timer_config_t timer_config = {
        .duty_resolution = LEDC_TIMER_13_BIT,
        .freq_hz         = 4000,
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
}

void vTaskMonitor(void *pvParameters)
{

}
