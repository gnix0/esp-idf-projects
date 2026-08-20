#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdbool.h>
#include <sys/time.h>

#define LED_PIN_1  5
#define LED_PIN_2  19
#define BUTTON_PIN 23

static unsigned long long time_ms(void);
static bool button_was_pressed(void);
static void toggle_leds(bool *led1_state, bool *led2_state);
static void make_leds_out_of_phase(bool *led1_state, bool *led2_state);

void app_main(void)
{
    gpio_reset_pin(LED_PIN_1);
    gpio_set_direction(LED_PIN_1, GPIO_MODE_OUTPUT);
    gpio_reset_pin(LED_PIN_2);
    gpio_set_direction(LED_PIN_2, GPIO_MODE_OUTPUT);

    gpio_reset_pin(BUTTON_PIN);
    gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_PIN, GPIO_PULLUP_ONLY);

    bool led1_state = false;
    bool led2_state = false;

    gpio_set_level(LED_PIN_1, 0);
    gpio_set_level(LED_PIN_2, 0);

    static bool waiting_for_second_press = false;
    static unsigned long long first_press_time = 0;

    for (;;) {
        if (button_was_pressed()) {
            unsigned long long now = time_ms();

            if (!waiting_for_second_press) {
                first_press_time = now;
                waiting_for_second_press = true;
            } else {
                unsigned long long elapsed = now - first_press_time;

                if (elapsed < 500) {
                    make_leds_out_of_phase(&led1_state, &led2_state);

                    waiting_for_second_press = false;
                } else {
                    toggle_leds(&led1_state, &led2_state);

                    first_press_time = now;
                    waiting_for_second_press = true;
                }
            }
        }

        if (waiting_for_second_press) {
            unsigned long long now = time_ms();
            unsigned long long elapsed = now - first_press_time;
            if (elapsed >= 500) {
                toggle_leds(&led1_state, &led2_state);

                waiting_for_second_press = false;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static unsigned long long time_ms(void)
{
    struct timeval tv;

    gettimeofday(&tv,NULL);
    return (((long long)tv.tv_sec)*1000)+(tv.tv_usec/1000);
}

static bool button_was_pressed(void)
{
    static int last_button_state = 1;

    int current_button_state = gpio_get_level(BUTTON_PIN);

    bool pressed = false;

    if (last_button_state == 1 && current_button_state == 0) {
        pressed = true;
    }

    last_button_state = current_button_state;
    return pressed;
}

static void toggle_leds(bool *led1_state, bool *led2_state)
{
    *led1_state = !(*led1_state);
    *led2_state = !(*led2_state);

    gpio_set_level(LED_PIN_1, *led1_state);
    gpio_set_level(LED_PIN_2, *led2_state);
}

static void make_leds_out_of_phase(bool *led1_state, bool *led2_state)
{
    if (*led1_state == *led2_state) {
        *led1_state = true;
        *led2_state = false;
    } else {
        *led1_state = false;
        *led2_state = false;
    }

    gpio_set_level(LED_PIN_1, *led1_state);
    gpio_set_level(LED_PIN_2, *led2_state);
}
