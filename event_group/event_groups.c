#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"

#include "driver/gpio.h"
#include "esp_log.h"

#define LED1_PIN    GPIO_NUM_3
#define LED2_PIN    GPIO_NUM_4
#define BUTTON_PIN  GPIO_NUM_7

#define ENABLED     1
#define DISABLED    0

#define TIMER_START 0

#define TASK_1_BIT  ( 1 << 0 ) // 1
#define TASK_2_BIT  ( 1 << 1 ) // 10

TaskHandle_t xTask1Handle   = NULL;
TaskHandle_t xTask2Handle   = NULL;
TaskHandle_t xTask3Handle   = NULL;

TimerHandle_t xTimer1Handle = NULL;
TimerHandle_t xTimer2Handle = NULL;

EventGroupHandle_t xEvents  = NULL;

void vTask1(void *pvParameters);
void vTask2(void *pvParameters);
void vTask3(void *pvParameters);

void timer_callback_(TimerHandle_t pxTimer);

static const char TAG[] = "Event Groups Demo";

void app_main(void)
{
    gpio_config_t io_conf = {};

    io_conf.pin_bit_mask    = (1ULL<<LED1_PIN) | (1ULL<<LED2_PIN);
    io_conf.mode            = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en      = DISABLED;
    io_conf.pull_down_en    = DISABLED;
    io_conf.intr_type       = GPIO_INTR_DISABLE;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    io_conf.pin_bit_mask    = (1ULL<<BUTTON_PIN);
    io_conf.mode            = GPIO_MODE_INPUT;
    io_conf.pull_up_en      = ENABLED;
    io_conf.pull_down_en    = DISABLED;
    io_conf.intr_type       = GPIO_INTR_NEGEDGE;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    xEvents = xEventGroupCreate();

    xTimer1Handle = xTimerCreate("Timer 1", pdMS_TO_TICKS(1000), pdTRUE, (void *)0, timer_callback_);

    xTaskCreate(vTask1, "Task 1", configMINIMAL_STACK_SIZE+1024, NULL, 1, &xTask1Handle);
    xTaskCreate(vTask2, "Task 2", configMINIMAL_STACK_SIZE+1024, NULL, 1, &xTask2Handle);
    xTaskCreate(vTask3, "Task 3", configMINIMAL_STACK_SIZE+1024, NULL, 1, &xTask3Handle);

    xTimerStart(xTimer1Handle, TIMER_START);

    vTaskDelete(NULL);
}

void vTask1(void *pvParameters)
{
    // Keep waiting until event bit of task 1 is set
    for (;;) {
        xEventGroupWaitBits(xEvents, TASK_1_BIT, pdTRUE, pdTRUE, portMAX_DELAY);
        ESP_LOGI(TAG, "Task 1 was unlocked.");
    }
}

void vTask2(void *pvParameters)
{
    uint8_t led_status = DISABLED;

    // Keep waiting until event bit of task 2 is set
    for (;;) {
        xEventGroupWaitBits(xEvents, TASK_2_BIT, pdTRUE, pdTRUE, portMAX_DELAY);
        ESP_LOGI(TAG, "Task 2 was unlocked.");

        gpio_set_level(LED2_PIN, led_status^=1);
    }
}

void vTask3(void *pvParameters)
{
    xEventGroupWaitBits(xEvents, TASK_1_BIT | TASK_2_BIT, pdTRUE, pdTRUE, portMAX_DELAY);
    ESP_LOGI(TAG, "Task 3 was unlocked.");
}

void timer_callback_(TimerHandle_t pxTimer)
{
    static uint8_t count = 0;

    count++;
    gpio_set_level(LED1_PIN, count%2);

    if (count == 5)
        xEventGroupSetBits(xEvents, TASK_1_BIT);
    else if (count == 10)
        xEventGroupSetBits(xEvents, TASK_2_BIT);
    else if (count == 15) {
        count = 0;
        xEventGroupSetBits(xEvents, TASK_1_BIT | TASK_2_BIT);
    }    
}