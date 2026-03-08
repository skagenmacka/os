#include <stdint.h>

#define UART0_BASE 0x09000000
#define UART_DR ((volatile unsigned int *)(UART0_BASE + 0x00))
#define UART_FR ((volatile unsigned int *)(UART0_BASE + 0x18))

static void uart_putc(char c) {
  volatile uint32_t *uartdr = (uint32_t *)UART0_BASE;
  *uartdr = (uint32_t)c;
}

static void uart_puts(const char *s) {
  while (*s) {
    uart_putc(*s++);
  }
}

static char uart_getc() {
  while (*UART_FR & (1 << 4)) {
  }
  return (char)(*UART_DR);
}

static void get_line(char *buffer) {
  int i = 0;

  while (1) {
    char c = uart_getc();

    if (c == '\r' || c == '\n') {
      buffer[i] = 0;
      uart_putc('\n');
      return;
    }

    buffer[i++] = c;
    uart_putc(c);
  }
}

void kernel_main(void) {
  uart_puts("Hello from kernel!\r\n");

  char line[128];

  while (1) {
    uart_puts("> ");

    get_line(line);

    uart_puts("Du skrev: ");
    uart_puts(line);
    uart_puts("\n");
  }
}
