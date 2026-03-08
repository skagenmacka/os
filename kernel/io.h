#pragma once

#include <stdint.h>

void uart_init(void);
void uart_putc(char c);

void get_line(char *buf);

void put_string(const char *s);
void put_hex64(uint64_t val);
void kprintf(const char *fmt, ...);
