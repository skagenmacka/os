#pragma once

#include <stdint.h>

void timer_init(void);
void irq_handle(void);
uint64_t timer_get_ticks(void);
