#include "cmd.h"

static int is_space(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

int parse_command(char *line, char **argv, int max_args) {
  int argc = 0;
  char *p = line;

  while (*p && argc < max_args) {
    while (*p && is_space(*p)) {
      p++;
    }

    if (!*p) {
      break;
    }

    argv[argc++] = p;

    while (*p && !is_space(*p)) {
      p++;
    }

    if (*p) {
      *p = '\0';
      p++;
    }
  }

  return argc;
}