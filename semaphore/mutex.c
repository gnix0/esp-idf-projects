#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "driver/gpio.h"
#include "esp_log.h"

#include <stdio.h>

#define PRIORITY_MIN    1
#define PRIORITY_MAX    4
#define TASK_STACK_SIZE 2048

static const char TAG[] = "Mutex Demo";

void task1(void *pvParameters);
void task2(void *pvParameters);

SemaphoreHandle_t xMutexSemaphore = NULL;

void send_serial_data(const char *data);

void app_main(void)
{
    xMutexSemaphore = xSemaphoreCreateMutex();

    xTaskCreate(&task1, "Task 1", TASK_STACK_SIZE, NULL, PRIORITY_MIN, NULL);
    xTaskCreate(&task2, "Task 2", TASK_STACK_SIZE, NULL, PRIORITY_MAX, NULL);

    vTaskDelete(NULL);
}

void task1(void *pvParameters)
{
    for (;;) {
        xSemaphoreTake(xMutexSemaphore, portMAX_DELAY);

        ESP_LOGI(TAG, "Task 1 took the mutex.");

        send_serial_data("Task 1: [1] Sending information through serial interface.\n");
        send_serial_data("Task 1: [2] Sending information through serial interface.\n");
        send_serial_data("Waiting 5 seconds...");

        for (volatile uint32_t i = 0; i < 20000; i++)
            esp_rom_delay_us(100);

        send_serial_data("Task 1: [3] Sending information through serial interface.\n");
        send_serial_data("Task 1: [4] Sending information through serial interface.\n");

        xSemaphoreGive(xMutexSemaphore);
        ESP_LOGI(TAG, "Task 1 released the mutex.");

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void task2(void *pvParameters)
{
    for (;;) {

        if (xSemaphoreTake(xMutexSemaphore, portMAX_DELAY))
            ESP_LOGI(TAG, "Task 2 can't take the mutex now.");
        else
            ESP_LOGI(TAG, "Task 2 took the mutex.");

        send_serial_data("Task 2: [1] Sending information through serial interface.\n");
        send_serial_data("Task 2: [2] Sending information through serial interface.\n");
        send_serial_data("Task 2: [3] Sending information through serial interface.\n");
        send_serial_data("Task 2: [4] Sending information through serial interface.\n");

        ESP_LOGI(TAG, "Task 2 released the mutex.");
        xSemaphoreGive(xMutexSemaphore);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void send_serial_data(const char *data)
{
    printf(data);
}