#ifndef _TICK_H
#define _TICK_H

#include <stdint.h>

static volatile uint16_t tick;

void init_sensor();

void tick_start();
void tick_stop();

#endif /* _TICK_H */
