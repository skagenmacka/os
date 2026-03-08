#include <stdint.h>

#include "aarch64.h"
#include "fs/fs.h"
#include "fs/vfs.h"
#include "interrupts.h"
#include "io.h"
#include "kalloc.h"
#include "timer.h"

#include "lib/cmd.h"
#include "lib/string.h"

#define NCPU 8

#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_PATCH 1

static void cmd_ls(const char *path) {
  unsigned int i = 0;
  char name[32];
  enum vnode_type type;

  while (vfs_readdir(path, i, name, sizeof(name), &type) == 0) {
    put_string(name);
    if (type == VNODE_DIR) {
      uart_putc('/');
    }
    uart_putc('\r');
    uart_putc('\n');
    i++;
  }
}

static void handle_command(char *cmd) {
  char *argv[4];
  int argc = parse_command(cmd, argv, 4);

  if (argc == 0) {
    return;
  }

  if (strcmp(argv[0], "echo") == 0) {
    put_string("");
    uart_putc('\r');
    uart_putc('\n');
  } else if (strcmp(argv[0], "mkdir") == 0) {
    if (argc != 2) {
      put_string("usage: mkdir <directory_name>\r\n");
      return;
    }

    int s = vfs_create(argv[1], VNODE_DIR);
    kprintf("Created path %s with status %d\r\n", argv[1], s);
  } else if (strcmp(argv[0], "ls") == 0) {
    cmd_ls("/");
  } else if (strcmp(argv[0], "ticks") == 0) {
    put_string("ticks: ");
    put_hex64(timer_get_ticks());
    put_string("\r\n");
  } else if (strcmp(argv[0], "test") == 0) {
    void *p1 = kalloc();
    void *p2 = kalloc();

    put_string("p1: ");
    put_hex64((uint64_t)p1);
    put_string("\r\n");

    put_string("p2: ");
    put_hex64((uint64_t)p1);
    put_string("\r\n");

    kfree(p1);
    kfree(p2);
  } else {
    kprintf("os: command not found: '%s'\r\n", argv[0]);
  }
}

void kernel_main(void) {
  unsigned long id = cpuid();
  if (id == 0) {
    // loading output
    uart_init();
    kprintf("booting OS version %d.%d.%d...\r\n", VERSION_MAJOR, VERSION_MINOR,
            VERSION_PATCH);

    vfs_init();

    irq_disable();

    timer_init();

    irq_enable();
  }

  char line[128];

  while (1) {
    put_string("os> ");

    get_line(line);
    uart_putc('\r');

    handle_command(line);
  }
}
