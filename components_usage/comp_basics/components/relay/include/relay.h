#ifndef RELAY_H
#define RELAY_H

#include <stdbool.h>

typedef struct {
    bool pin;
    bool state;
} Relay;

void relay_init(Relay *relay, bool pin);
void relay_turn_on(Relay *relay);
void relay_turn_off(Relay *relay);
bool relay_get_state(Relay *relay);

#endif
