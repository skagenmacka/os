#pragma once

#include <stdint.h>

void timer_init(void);
void timer_handle_irq(void);
uint64_t timer_get_ticks(void);
