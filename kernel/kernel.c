#include <stdint.h>

#include "io.h"

#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_PATCH 1

void kernel_main(void) {
  uart_init();
  kprintf("OS version %d.%d.%d\r\n", VERSION_MAJOR, VERSION_MINOR,
          VERSION_PATCH);
  kprintf("Hejsan\r\n");

  char line[128];

  while (1) {
    put_string("> ");

    get_line(line);

    put_string("Du skrev: ");
    put_string(line);
    uart_putc('\n');
  }
}
