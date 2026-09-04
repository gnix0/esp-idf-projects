#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "driver/gpio.h"
#include "esp_log.h"

#define LED1_PIN    GPIO_NUM_3
#define LED2_PIN    GPIO_NUM_4
#define BUTTON_PIN  GPIO_NUM_7

#define ENABLED         1
#define DISABLED        0
#define BUTTON_PRESSED  0
#define BUTTON_RELEASED 1

#define TIMER_START 0

static const char TIMER1_TAG[] = "Timer 1 Callback";
static const char TIMER2_TAG[] = "Timer 2 Callback";

TaskHandle_t xTask1Handle   = NULL;
TimerHandle_t xTimer1Handle = NULL:
TimerHandle_t xTimer2Handle = NULL;

void task1(void *pvParameters);

// Timer callback functions
void timer1_callback_(TimerHandle_t xTimer);
void timer2_callback_(TimerHandle_t xTimer);

void app_main(void)
{
    gpio_config_t io_conf = {};

    io_conf.pin_bit_mask    = (1ULL<<LED1_PIN) | (1ULL<<LED2_PIN);
    io_conf.mode            = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en      = DISABLED;
    io_conf.pull_down_en    = DISABLED;
    io_conf.intr_type       = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);

    io_conf.pin_bit_mask    = (1ULL<<BUTTON_PIN);
    io_conf.mode            = GPIO_MODE_INPUT;
    io_conf.pull_up_en      = ENABLED;
    io_conf.pull_down_en    = DISABLED;
    io_conf.intr_type       = GPIO_INTR_NEGEDGE;
    gpio_config(&io_conf);

    // Auto Reload Timer
    xTimer1Handle = xTimerCreate("Timer1", pdMS_TO_TICKS(1000), pdTRUE, (void *)0, timer1_callback_);
    // One-shot Timer
    xTimer2Handle = xTimerCreate("Timer2", pdMS_TO_TICKS(5000), pdTRUE, (void *)0, timer2_callback_);

    xTaskCreate(task1, "Task 1", configMINIMAL_STACK_SIZE+1024, NULL, 1, &xTask1Handle);

    xTimerStart(xTimer1Handle, TIMER_START);

    vTaskDelete(NULL);
}

void task1(void *pvParameters)
{
    uint8_t debounce_time = 0;

    for (;;) {
        if (gpio_get_level(BUTTON_PIN) == BUTTON_PRESSED) {
            debounce_time++;

            if (debounce_time >= 10) {
                gpio_set_level(LED2_PIN, ENABLED);

                debounce_time = 0;

                xTimerStart(xTimer2Handle, 0);
                ESP_LOGI(TIMER2_TAG, "Timer 2 Start");
            }
        } else {
            debounce_time = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void timer1_callback_(TimerHandle_t xTimer)
{
    static uint8_t led_state = DISABLED;

    gpio_set_level(LED1_PIN, led_state^=1);
    ESP_LOGI(TIMER1_TAG, "Timer 1 Callback");
}

void timer2_callback_(TimerHandle_t xTimer)
{
    gpio_set_level(LED2_PIN, DISABLED);
    ESP_LOGI(TIMER2_TAG, "Timer 2 Callback");
}