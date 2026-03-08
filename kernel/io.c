#include <stdarg.h>
#include <stdint.h>

#define UART0_BASE 0x09000000
#define UART_DR ((volatile unsigned int *)(UART0_BASE + 0x00))
#define UART_FR ((volatile unsigned int *)(UART0_BASE + 0x18))
#define UART_IBRD ((volatile unsigned int *)(UART0_BASE + 0x24))
#define UART_FBRD ((volatile unsigned int *)(UART0_BASE + 0x28))
#define UART_LCRH ((volatile unsigned int *)(UART0_BASE + 0x2C))
#define UART_CR ((volatile unsigned int *)(UART0_BASE + 0x30))
#define UART_IMSC ((volatile unsigned int *)(UART0_BASE + 0x38))
#define UART_MIS ((volatile unsigned int *)(UART0_BASE + 0x40))
#define UART_ICR ((volatile unsigned int *)(UART0_BASE + 0x44))

#define LINE_BUFFER_SIZE 128
#define UART_RX_BUFFER_SIZE 256
#define UART_FR_RXFE (1U << 4)
#define UART_FR_TXFF (1U << 5)
#define UART_INT_RX (1U << 4)
#define UART_INT_RT (1U << 6)

static volatile unsigned int uart_rx_head;
static volatile unsigned int uart_rx_tail;
static char uart_rx_buffer[UART_RX_BUFFER_SIZE];

static char uart_getc(void);
static int uart_rx_pop(char *c);
static void discard_escape_sequence(void);

void uart_init(void) {
  *UART_CR = 0x0;
  *UART_ICR = 0x7FF;

  *UART_IBRD = 13;
  *UART_FBRD = 1;
  *UART_LCRH = (0x3 << 5);
  *UART_IMSC = 0x0;
  *UART_CR = (1 << 0) | (1 << 8) | (1 << 9);

  uart_rx_head = 0;
  uart_rx_tail = 0;
}

void uart_irq_init(void) {
  *UART_ICR = 0x7FF;
  *UART_IMSC = UART_INT_RX | UART_INT_RT;
}

void uart_putc(char c) {
  while (*UART_FR & UART_FR_TXFF) {
  }

  *UART_DR = (uint32_t)c;
}

static char uart_getc(void) {
  char c;

  while (!uart_rx_pop(&c)) {
  }

  return c;
}

static int uart_rx_pop(char *c) {
  if (uart_rx_head == uart_rx_tail) {
    return 0;
  }

  *c = uart_rx_buffer[uart_rx_tail];
  uart_rx_tail = (uart_rx_tail + 1U) % UART_RX_BUFFER_SIZE;
  return 1;
}

void uart_handle_irq(void) {
  unsigned int mis = *UART_MIS;

  if ((mis & (UART_INT_RX | UART_INT_RT)) == 0) {
    return;
  }

  while ((*UART_FR & UART_FR_RXFE) == 0) {
    unsigned int next_head = (uart_rx_head + 1U) % UART_RX_BUFFER_SIZE;
    char c = (char)(*UART_DR);

    if (next_head == uart_rx_tail) {
      break;
    }

    uart_rx_buffer[uart_rx_head] = c;
    uart_rx_head = next_head;
  }

  *UART_ICR = UART_INT_RX | UART_INT_RT;
}

static void discard_escape_sequence(void) {
  char c = uart_getc();

  if (c != '[' && c != 'O') {
    return;
  }

  do {
    c = uart_getc();
  } while ((c >= '0' && c <= '9') || c == ';');
}

void get_line(char *buf) {
  int i = 0;

  while (1) {
    char c = uart_getc();

    if (c == 27) {
      discard_escape_sequence();
      continue;
    }

    if (c == '\b' || c == 127) {
      if (i > 0) {
        i--;
        uart_putc('\b');
        uart_putc(' ');
        uart_putc('\b');
      }
      continue;
    }

    if (c == '\r' || c == '\n') {
      buf[i] = 0;
      uart_putc('\n');
      return;
    }

    if (c < ' ' || c > '~') {
      continue;
    }

    if (i >= LINE_BUFFER_SIZE - 1) {
      continue;
    }

    buf[i++] = c;
    uart_putc(c);
  }
}

void put_string(const char *s) {
  while (*s) {
    uart_putc(*s++);
  }
}

void put_hex64(uint64_t val) {
  static const char digits[] = "0123456789abcdef";

  put_string("0x");

  for (int shift = 60; shift >= 0; shift -= 4) {
    uart_putc(digits[(val >> shift) & 0xf]);
  }
}

void put_int(int val) {
  char buf[16];
  int i = 0;

  if (val == 0) {
    uart_putc('0');
    return;
  }

  if (val < 0) {
    uart_putc('-');
    val = -val;
  }

  while (val > 0) {
    buf[i++] = '0' + (val % 10);
    val /= 10;

    if (i == 16) {
      break;
    }
  }

  while (i--) {
    uart_putc(buf[i]);
  }
}

void kprintf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  while (*fmt) {
    if (*fmt == '%') {
      fmt++;

      switch (*fmt) {
      case 'd': {
        int d = va_arg(args, int);
        put_int(d);
        break;
      }
      case 's': {
        char *s = va_arg(args, char *);
        put_string(s);
        break;
      }
      case '%':
        uart_putc('%');
        break;

      default:
        uart_putc('?');
      }
    } else {
      uart_putc(*fmt);
    }
    fmt++;
  }

  va_end(args);
}
