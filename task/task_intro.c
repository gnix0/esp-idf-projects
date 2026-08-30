#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_log.h"

#include <stdbool.h>
#include <stdint.h>

static const char TAG[] = "Test task: ";

#define LED_PIN 23

TaskHandle_t xTaskBlinkHandle = NULL;

void vTaskBlink(void *pvParameters);

void app_main(void)
{
    ESP_LOGI(TAG, "initializing application");

    xTaskCreate(vTaskBlink, "TASK_BLINK", 2048, NULL,
                1, &xTaskBlinkHandle);

    uint16_t counter = 0;
    for (;;) {
        ESP_LOGI(TAG, "TASK MAIN - Counter: %d", counter);
        vTaskDelay(pdMS_TO_TICKS(3000));
        counter++;
    }
}

void vTaskBlink(void *pvParameters)
{
    bool status = false;

    ESP_LOGI(TAG, "initializing task Blink");
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    for (;;) {
        gpio_set_level(LED_PIN, status^=1);
        ESP_LOGI(TAG, "TASK BLINK LED[%d]: %d", LED_PIN, status);
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}
