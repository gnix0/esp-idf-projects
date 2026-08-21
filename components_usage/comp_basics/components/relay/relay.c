#include "relay.h"
#include "driver/gpio.h"
#include "hal/gpio_types.h"

#include <stdbool.h>

void relay_init(Relay *relay, bool pin)
{
    relay->pin = pin;
    relay->state = 0;
    gpio_set_direction(relay->pin, GPIO_MODE_OUTPUT);
    gpio_set_level(relay->pin, relay->state);
}

void relay_turn_on(Relay *relay)
{
    relay->state = 1;
    gpio_set_level(relay->pin, relay->state);
}

void relay_turn_off(Relay *relay)
{
    relay->state = 0;
    gpio_set_level(relay->pin, relay->state);
}

bool relay_get_state(Relay *relay)
{
    return relay->state;
}
