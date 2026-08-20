#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"
#include "esp_log.h"

#define LED_PIN_1  5
#define LED_PIN_2  19
#define BUTTON_PIN 23

static const *TAG = "INTERRUPT_TEST";

void app_main(void)
{
    gpio_reset_pin(LED_PIN_1);
}
