#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_log.h"

#define LED1_PIN    GPIO_NUM_3
#define LED2_PIN    GPIO_NUM_4
#define BUTTON_PIN  GPIO_NUM_7

#define ENABLED     1
#define DISABLED    0
#define RECEIVED    1

static const char TAG[] = "Task Notifications Demo";

TaskHandle_t xTask1Handle = NULL;

void vTask1(void *pvParameters);

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    // Variable to check if the task's notification will unlock a task of higher priority
    BaseType_t xHigherPriorityTaskWoken = pdTRUE;

    if (BUTTON_PIN == (uint32_t) arg)
        xTaskNotifyFromISR(xTask1Handle, 1, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
}

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

    ESP_ERROR_CHECK(gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1));
    ESP_ERROR_CHECK(gpio_isr_handler_add(BUTTON_PIN, gpio_isr_handler, (void *)BUTTON_PIN));

    xTaskCreate(vTask1, "Task 1", configMINIMAL_STACK_SIZE+1024, NULL, 2, &xTask1Handle);
    
    vTaskDelete(NULL);
}

void vTask1(void *pvParameters)
{
    uint32_t ulNotifiedValue = 0;

    for (;;) {
        ulNotifiedValue = ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // Wait for notification

        if (ulNotifiedValue == RECEIVED) {
            ESP_LOGI(TAG, "Button pressed!");
            gpio_set_level(LED1_PIN, ENABLED);
            vTaskDelay(pdMS_TO_TICKS(5000));
            gpio_set_level(LED1_PIN, DISABLED);
        }
    }
}