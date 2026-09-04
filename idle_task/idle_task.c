#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_log.h"

#include <rom/ets_sys.h>

#define LED1_PIN        GPIO_NUM_3
#define LED2_PIN        GPIO_NUM_4
#define BLINK1_PERIOD   1000
#define BLINK2_PERIOD   500

#define ENABLED     1
#define DISABLED    0

static const char TAG1[] = "Task App Main";
static const char TAG2[] = "Task Blink";
static const char TAG3[] = "Task Counter";

typedef struct {
    uint32_t led_pin;
    uint32_t blink_period;
} Led_t;

Led_t led1 = {LED1_PIN, BLINK1_PERIOD};
Led_t led2 = {LED2_PIN, BLINK2_PERIOD};

void vTaskBlink(void *pvParameters);
void vTaskCounter(void *pvParameters);

TaskHandle_t xTaskBlink1Handle  = NULL;
TaskHandle_t xTaskBlink2Handle  = NULL;
TaskHandle_t xTaskCounterHandle = NULL;

static uint32_t ulIdleCycleCount        = 0UL;
static uint32_t lastIdleCycleCountPrint = 0;

void app_main(void)
{
    ESP_LOGI(TAG1, "Application starting...");

    xTaskCreate(
        vTaskBlink,
        "Task Blink 1",
        2048,
        (void *)&led1,
        1,
        &xTaskBlink1Handle
    );

    xTaskCreate(
        vTaskBlink,
        "Task Blink 2",
        2048,
        (void *)&led2,
        1,
        &xTaskBlink2Handle
    );

    xTaskCreate(
        vTaskCounter,
        "Task Counter",
        2048,
        (void *)1000,
        2,
        &xTaskCounterHandle
    );

    uint32_t counter = 0;
    for (;;) {
        ESP_LOGI(TAG1, "Counter: %d", counter++);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vTaskBlink(void *pvParameters)
{
    uint32_t led_pin        = ((Led_t *)pvParameters)->led_pin;
    uint32_t blink_period   = ((Led_t *)pvParameters)->blink_period;
    uint8_t status          = DISABLED;

    ESP_LOGI(TAG2, "Starting Blink Task...");

    gpio_set_direction(led_pin, GPIO_MODE_OUTPUT);

    for (;;) {
        gpio_set_level(led_pin, status^=1);
        ets_delay_us(1000000);
        ESP_LOGI(TAG2, "[%s] - LED[%d]: %d", pcTaskGetName(NULL), led_pin, status);
        vTaskDelay(pdMS_TO_TICKS(blink_period));
    }
}

void vTaskCounter(void *pvParameters)
{
    uint32_t counter = (uint32_t)pvParameters;

    for (;;) {
        ESP_LOGI(TAG3, "Counter: %d", counter++);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vApplicationIdleHook(void)
{
    ulIdleCycleCount++;

    if (ulIdleCycleCount - lastIdleCycleCountPrint >= pdMS_TO_TICKS(1000)) {
        ESP_LOGI(__func__, "Idle cycle count: %lu", ulIdleCycleCount);
        lastIdleCycleCountPrint = ulIdleCycleCount;
    }
}