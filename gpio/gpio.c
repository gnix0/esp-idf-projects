#include "driver/gpio.h"
#include "freertos/idf_additions.h"
#include "hal/gpio_types.h"
#include "portmacro.h"

#define LED_PIN_1 5
#define LED_PIN_2 19

void app_main(void)
{
    gpio_reset_pin(LED_PIN_1);
    gpio_set_direction(LED_PIN_1, GPIO_MODE_DEF_OUTPUT);

    gpio_reset_pin(LED_PIN_2);
    gpio_set_direction(LED_PIN_2, GPIO_MODE_DEF_OUTPUT);

    for (;;) {
        gpio_set_level(LED_PIN_1, 1);
        gpio_set_level(LED_PIN_2, 0);
        vTaskDelay(500/portTICK_PERIOD_MS);

        gpio_set_level(LED_PIN_1, 0);
        gpio_set_level(LED_PIN_2, 1);
        vTaskDelay(500/portTICK_PERIOD_MS);
    }
}
