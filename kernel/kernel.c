#include <stdint.h>

#define UART0_BASE 0x09000000

static void uart_putc(char c) {
  volatile uint32_t *uartdr = (uint32_t *)UART0_BASE;
  *uartdr = (uint32_t)c;
}

static void uart_puts(const char *s) {
  while (*s) {
    uart_putc(*s++);
  }
}

void kernel_main(void) {
  uart_puts("Hello from kernel!\r\n");

  while (1) {
    __asm__ volatile("wfi");
  }
}
