#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include "hal/gpio_types.h"
#include "portmacro.h"

#define LED_PIN_1  5
#define LED_PIN_2  19
#define BUTTON_PIN_1 23
#define BUTTON_PIN_2 22

static const char TAG[] = "INTERRUPT_TEST";

static QueueHandle_t gpio_evt_queue = NULL;

static void gpio_isr_handler(void *arg);
static void button_task(void *pvparameters);

void app_main(void)
{
    gpio_reset_pin(LED_PIN_1);
    gpio_set_direction(LED_PIN_1, GPIO_MODE_OUTPUT);
    gpio_reset_pin(LED_PIN_2);
    gpio_set_direction(LED_PIN_2, GPIO_MODE_OUTPUT);

    gpio_set_level(LED_PIN_1, 0);
    gpio_set_level(LED_PIN_2, 0);

    gpio_reset_pin(BUTTON_PIN_1);
    gpio_set_direction(BUTTON_PIN_1, GPIO_MODE_INPUT);
    gpio_pullup_en(BUTTON_PIN_1);
    gpio_set_intr_type(BUTTON_PIN_1, GPIO_INTR_NEGEDGE); // Set to falling edge

    gpio_reset_pin(BUTTON_PIN_2);
    gpio_set_direction(BUTTON_PIN_2, GPIO_MODE_INPUT);
    gpio_pullup_en(BUTTON_PIN_2);
    gpio_set_intr_type(BUTTON_PIN_2, GPIO_INTR_NEGEDGE); // Set to falling edge

    gpio_evt_queue = xQueueCreate(1, sizeof(uint32_t));
    xTaskCreate(button_task, "BUTTON TASK", 2048, NULL, 2, NULL);

    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);
    gpio_isr_handler_add(BUTTON_PIN_1, gpio_isr_handler, (void *)BUTTON_PIN_1);
    gpio_isr_handler_add(BUTTON_PIN_2, gpio_isr_handler, (void *)BUTTON_PIN_2);
}

static void button_task(void *pvparameters)
{
    uint32_t gpio_num;
    TickType_t last_button_press = 0;
    bool led1 = 0;
    bool led2 = 0;

    for(;;) {
        xQueueReceive(gpio_evt_queue, &gpio_num, portMAX_DELAY);
        ESP_LOGI(TAG, "GPIO[%li] intr\n", gpio_num);

        TickType_t current_time = xTaskGetTickCount();

        if (current_time - last_button_press > pdMS_TO_TICKS(250)) {
            last_button_press = current_time;

            if (gpio_num == BUTTON_PIN_1)
                gpio_set_level(LED_PIN_1, led1^=1);
            else if (gpio_num == BUTTON_PIN_2)
                gpio_set_level(LED_PIN_2, led2^=1);
        }
    }
}

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}
