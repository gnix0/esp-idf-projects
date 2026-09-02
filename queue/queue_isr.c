#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"
#include "esp_log.h"

#define LED1_PIN    GPIO_NUM_3
#define LED2_PIN    GPIO_NUM_4
#define LED3_PIN    GPIO_NUM_22
#define LED4_PIN    GPIO_NUM_23
#define BUTTON1_PIN GPIO_NUM_6
#define BUTTON2_PIN GPIO_NUM_7

#define ENABLED     1
#define DISABLED    0

static const char TAG[] = "Queues with ISR";

QueueHandle_t gpio_evt_queue = NULL;

void buttonTask(void *pvParameters);
void ledTask(void *pvParameters);

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;

    BaseType_t xHigherPriorityTaskWoken = pdTRUE;

    xQueueSendFromISR(gpio_evt_queue, &gpio_num, &xHigherPriorityTaskWoken);
}

void app_main(void)
{
    gpio_config_t io_conf = {};

    // LEDs configuration
    io_conf.pin_bit_mask    = (1ULL<<LED1_PIN) | (1ULL<<LED2_PIN) 
                            | (1ULL<<LED3_PIN) | (1ULL<<LED4_PIN);
    io_conf.mode            = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en      = DISABLED;
    io_conf.pull_down_en    = DISABLED;
    io_conf.intr_type       = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);

    // Buttons configuration
    io_conf.pin_bit_mask    = (1ULL<<BUTTON1_PIN) | (1ULL<<BUTTON2_PIN);
    io_conf.mode            = GPIO_MODE_INPUT;
    io_conf.pull_up_en      = ENABLED;
    io_conf.pull_down_en    = DISABLED;
    io_conf.intr_type       = GPIO_INTR_NEGEDGE;
    gpio_config(&io_conf);

    // Install GPIO ISR Service and add handler for the buttons
    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);
    gpio_isr_handler_add(BUTTON1_PIN, gpio_isr_handler, (void *)BUTTON1_PIN);
    gpio_isr_handler_add(BUTTON2_PIN, gpio_isr_handler, (void *)BUTTON2_PIN);

    // Create queue and tasks
    gpio_evt_queue = xQueueCreate(2, sizeof(uint32_t));
    xTaskCreate(buttonTask, "Buttons Task", 2048, NULL, 2, NULL);
    xTaskCreate(ledTask, "LEDs Task", 2048, NULL, 1, NULL);

    for (;;) {
        gpio_set_level(LED3_PIN, ENABLED);
        vTaskDelay(pdMS_TO_TICKS(1000));
        gpio_set_level(LED3_PIN, DISABLED);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void ledTask(void *pvParameters)
{
    uint8_t led = DISABLED;

    for (;;) {
        led^=1;
        gpio_set_level(LED4_PIN, led);
        vTaskDelay(pdMS_TO_TICKS(750));
    }
}

void buttonTask(void *pvParameters)
{
    uint32_t gpio_num               = 0;
    uint8_t led1                    = 0;
    uint8_t led2                    = 0;
    TickType_t last_button_press    = 0;

    for (;;) {
        xQueueReceive(gpio_evt_queue, &gpio_num, portMAX_DELAY);
        ESP_LOGI(TAG, "GPIO[%li] intr \n", gpio_num);

        TickType_t curr_time = xTaskGetTickCount();
        if (curr_time - last_button_press >= pdMS_TO_TICKS(250)) {
            // Invert first two LEDs states based on ISR
            last_button_press = curr_time;

            if (gpio_num == BUTTON1_PIN)
                gpio_set_level(LED1_PIN, led1^=1);
            else if (gpio_num == BUTTON2_PIN)
                gpio_set_level(LED2_PIN, led2^=1);
        }
    }
}