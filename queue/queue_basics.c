#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"
#include "esp_log.h"

#define LED                 GPIO_NUM_3
#define DEFAULT_LED_STATUS  0

QueueHandle_t xQueue;

TaskHandle_t xTask1Handle;
TaskHandle_t xTask2Handle;

void xTask1(void *pvParameters);
void xTask2(void *pvParameters);

static const char TAG[] = "Queue Basics";

void app_main(void)
{
    uint8_t status = DEFAULT_LED_STATUS;

    xQueue = xQueueCreate(5, sizeof(int));
    xTaskCreate(xTask1, "Task 1", configMINIMAL_STACK_SIZE+1024, NULL,
                1, &xTask1Handle);
    xTaskCreate(xTask2, "Task 2", configMINIMAL_STACK_SIZE+1024, NULL,
                1, &xTask2Handle);

    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    for (;;) {
        gpio_set_level(LED, status^=1);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void xTask1(void *pvParameters)
{
    uint8_t count = 0;
    
    for (;;) {
        if (count < 10) {
            xQueueSend(xQueue, &count, portMAX_DELAY);
            count++;
        } else {
            count = 0;
            vTaskDelay(pdMS_TO_TICKS(5000));
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void xTask2(void *pvParameters)
{
    int8_t received_count = 0;

    for (;;) {
        if (xQueueReceive(xQueue, &received_count, pdMS_TO_TICKS(1000)) == pdTRUE)
            ESP_LOGI(TAG, "Received: %d", received_count);
        else
            ESP_LOGI(TAG, "Timeout!");
    }
}