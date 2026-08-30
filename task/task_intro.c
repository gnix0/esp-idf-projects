#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_log.h"

#include <stdbool.h>
#include <stdint.h>

#define LED_RED           GPIO_NUM_23
#define LED_YELL          GPIO_NUM_20
#define BLINK_RED_PERIOD  1000
#define BLINK_YELL_PERIOD 500
#define COUNTER_PERIOD    500

static const char TAG1[] = "TASK APP MAIN: ";
static const char TAG3[] = "TASK BLINK: ";
static const char TAG2[] = "TASK COUNTER: ";

typedef struct {
    uint32_t led_pin;
    uint32_t blink_period;
} Led_t;

typedef struct {
    uint16_t counter_val;
    uint16_t counter_period;
} Counter_t;

Led_t     led_red  = {LED_RED, BLINK_RED_PERIOD};
Led_t     led_yell = {LED_YELL, BLINK_YELL_PERIOD};
Counter_t counter  = {0, COUNTER_PERIOD};

TaskHandle_t xTaskBlinkRedHandle    = NULL;
TaskHandle_t xTaskBlinkYellowHandle = NULL;
TaskHandle_t xTaskCounterHandle     = NULL;

void vTaskBlink(void *pvParameters);
void vTaskCounter(void *pvParameters);

void app_main(void)
{
    ESP_LOGI(TAG1, "running %s", pcTaskGetName(NULL));

    xTaskCreate(vTaskCounter, "TASK_COUNTER", 2048, (void *)&counter,
                2, &xTaskCounterHandle);
    xTaskCreate(vTaskBlink, "TASK_BLINK_RED", 2048, (void *)&led_red,
                1, &xTaskBlinkRedHandle);
    xTaskCreate(vTaskBlink, "TASK_BLINK_YELL", 2048, (void *)&led_yell,
                1, &xTaskBlinkYellowHandle);
}

void vTaskBlink(void *pvParameters)
{
    bool status = false;
    uint32_t pin = ((Led_t *)pvParameters)->led_pin;
    uint32_t blink_period = ((Led_t *)pvParameters)->blink_period;

    ESP_LOGI(TAG2, "running %s", pcTaskGetName(NULL));
    gpio_reset_pin(pin);
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);

    for (;;) {
        gpio_set_level(pin, status^=1);
        ESP_LOGI(TAG2, "[%s] LED[%d] = %u", pcTaskGetName(NULL), pin, status);
        vTaskDelay(pdMS_TO_TICKS(blink_period));
    }
}

void vTaskCounter(void *pvParameters)
{
    uint16_t period = ((Counter_t *)pvParameters)->counter_period;
    uint16_t val    = ((Counter_t *)pvParameters)->counter_val;

    ESP_LOGI(TAG3, "running %s", pcTaskGetName(NULL));

    for (;;) {
        ESP_LOGI(TAG3, "[%s] Counter = %u", pcTaskGetName(NULL), val);

        /* if (counter == 10) { */
        /*     vTaskSuspend(xTaskBlinkRedHandle); */
        /*     gpio_set_level(LED_RED, 0); */
        /*     ESP_LOGI(TAG3, "task Blink suspended"); */
        /* } else if (counter == 15) { */
        /*     vTaskResume(xTaskBlinkRedHandle); */
        /*     ESP_LOGI(TAG3, "task Blink running"); */
        /*     vTaskDelete(NULL);  // deletes itself, same as passing vTaskCounterHandle instead */
        /* } */

        vTaskDelay(pdMS_TO_TICKS(period));
        val++;
    }
}
