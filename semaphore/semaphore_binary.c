#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "driver/gpio.h"
#include "esp_log.h"

#define LED1_PIN    3
#define LED2_PIN    4
#define LED3_PIN    22
#define LED4_PIN    23
#define BUTTON1_PIN 7
#define BUTTON2_PIN 6

#define ENABLED     1
#define DISABLED    0

static const char TAG[] = "Binary Semaphore Demo";

SemaphoreHandle_t xBinarySemaphore = NULL;

void led_task(void *pvParameters);

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdTRUE;

    if (BUTTON1_PIN == (uint32_t)arg)
        xSemaphoreGiveFromISR(xBinarySemaphore, &xHigherPriorityTaskWoken);
}

void app_main(void)
{
    gpio_config_t io_conf = {};

    io_conf.pin_bit_mask    = (1ULL<<LED1_PIN);
    io_conf.mode            = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en      = DISABLED;
    io_conf.pull_down_en    = DISABLED;
    io_conf.intr_type       = GPIO_INTR_DISABLE;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    io_conf.pin_bit_mask    = (1ULL<<BUTTON1_PIN);
    io_conf.mode            = GPIO_MODE_INPUT;
    io_conf.pull_up_en      = ENABLED;
    io_conf.pull_down_en    = DISABLED;
    io_conf.intr_type       = GPIO_INTR_NEGEDGE;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    ESP_ERROR_CHECK(gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1));
    ESP_ERROR_CHECK(gpio_isr_handler_add(BUTTON1_PIN, gpio_isr_handler, (void *)BUTTON1_PIN));

    xBinarySemaphore = xSemaphoreCreateBinary();

    xTaskCreate(&led_task, "LED Task", configMINIMAL_STACK_SIZE+1024, NULL, 5, NULL);
    vTaskDelete(NULL);  // Same as vTaskDelete(&app_main);
}

void led_task(void *pvParameters)
{
    uint8_t led_status = DISABLED;

    for (;;) {
        if (xSemaphoreTake(xBinarySemaphore, portMAX_DELAY) == pdTRUE) {
            ESP_ERROR_CHECK(gpio_set_level(LED1_PIN, led_status^=1));
            ESP_LOGI(TAG, "ISR Received on BUTTON[%d]. Inverting LED[%d] state to: %d",
                                                    BUTTON1_PIN, LED1_PIN, led_status);
        }
    }
}