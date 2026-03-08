#pragma once

void uart_init(void);
void uart_putc(char c);

void get_line(char *buf);

void put_string(const char *s);
void kprintf(const char *fmt, ...);
