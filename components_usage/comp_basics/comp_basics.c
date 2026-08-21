#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"
#include "hal/gpio_types.h"
#include "esp_log.h"

#include "relay.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define RELAY_PIN_1  5
#define RELAY_PIN_2  19
#define BUTTON_PIN_1 22
#define BUTTON_PIN_2 23

static const char TAG[] = "COMPONENTS_TEST";

static void gpio_isr_handler(void *arg);
static void button_task(void *pvparameters);

static QueueHandle_t gpio_evt_queue = NULL;

Relay relay1, relay2;

void app_main(void)
{
    relay_init(&relay1, RELAY_PIN_1);
    relay_init(&relay2, RELAY_PIN_2);

    gpio_config_t button_config = {};
    button_config.pin_bit_mask  = (1ULL<<BUTTON_PIN_1)|(1ULL<<BUTTON_PIN_2);
    button_config.mode          = GPIO_MODE_INPUT;
    button_config.pull_up_en    = GPIO_PULLUP_ENABLE;
    button_config.pull_down_en  = GPIO_PULLDOWN_DISABLE;
    button_config.intr_type     = GPIO_INTR_NEGEDGE;
    gpio_config(&button_config);

    gpio_evt_queue = xQueueCreate(1, sizeof(uint64_t));
    xTaskCreate(button_task, "BUTTON TASK", 2048, NULL, 2, NULL);

    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);
    gpio_isr_handler_add(BUTTON_PIN_1, gpio_isr_handler, (void *)BUTTON_PIN_1);
    gpio_isr_handler_add(BUTTON_PIN_2, gpio_isr_handler, (void *)BUTTON_PIN_2);
}

static void button_task(void *pvparameters)
{
    uint32_t gpio_num;
    TickType_t last_button_press = 0;

    for(;;) {
        xQueueReceive(gpio_evt_queue, &gpio_num, portMAX_DELAY);
        ESP_LOGI(TAG, "GPIO[%li] intr\n", gpio_num);

        TickType_t current_time = xTaskGetTickCount();

        if (current_time - last_button_press > pdMS_TO_TICKS(250)) {
            last_button_press = current_time;

            if (gpio_num == BUTTON_PIN_1) {
                if (relay_get_state(&relay1) == 1 && relay_get_state(&relay2) == 1) {
                    ESP_LOGI(TAG, "Both relays are on...\n", gpio_num);
                    continue;
                }

                relay_turn_on(&relay1);
                relay_turn_on(&relay2);
            }
            else if (gpio_num == BUTTON_PIN_2) {
                if (relay_get_state(&relay1) == 0 && relay_get_state(&relay2) == 0) {
                    ESP_LOGI(TAG, "Both relays are off...\n", gpio_num);
                    continue;
                }

                relay_turn_off(&relay1);
                relay_turn_off(&relay2);
            }
        }
    }
}

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}
