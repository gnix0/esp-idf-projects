#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_log.h"

#include <stdbool.h>
#include <stdint.h>

static const char TAG1[] = "TASK APP MAIN: ";
static const char TAG3[] = "TASK BLINK: ";
static const char TAG2[] = "TASK COUNTER: ";

#define LED_PIN 23

TaskHandle_t xTaskBlinkHandle   = NULL;
TaskHandle_t xTaskCounterHandle = NULL;

void vTaskBlink(void *pvParameters);
void vTaskCounter(void *pvParameters);

void app_main(void)
{
    ESP_LOGI(TAG1, "running task app main");

    xTaskCreate(vTaskCounter, "TASK_COUNTER", 2048, NULL,
                2, &xTaskCounterHandle);
    xTaskCreate(vTaskBlink, "TASK_BLINK", 2048, NULL,
                1, &xTaskBlinkHandle);
}

void vTaskBlink(void *pvParameters)
{
    bool status = false;

    ESP_LOGI(TAG2, "running task Blink");
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    for (;;) {
        gpio_set_level(LED_PIN, status^=1);
        ESP_LOGI(TAG2, "LED[%d] = %d", LED_PIN, status);
        vTaskDelay(pdMS_TO_TICKS(1500));
    }
}

void vTaskCounter(void *pvParameters)
{
    uint16_t counter = 0;
    ESP_LOGI(TAG3, "running task Counter");

    for (;;) {
        ESP_LOGI(TAG3, "Counter = %d", counter);

        if (counter == 10) {
            vTaskSuspend(xTaskBlinkHandle);
            gpio_set_level(LED_PIN, 0);
            ESP_LOGI(TAG3, "task Blink suspended");
        } else if (counter == 15) {
            vTaskResume(xTaskBlinkHandle);
            ESP_LOGI(TAG3, "task Blink running");
            vTaskDelete(NULL);  // deletes itself, same as passing vTaskCounterHandle instead
        }

        vTaskDelay(pdMS_TO_TICKS(1500));
        counter++;
    }
}
