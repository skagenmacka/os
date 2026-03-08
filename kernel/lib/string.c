#include "string.h"

void *memset(void *dest, int value, unsigned long count) {
  unsigned char *ptr = (unsigned char *)dest;
  unsigned long i;

  for (i = 0; i < count; i++) {
    ptr[i] = (unsigned char)value;
  }

  return dest;
}

int strcmp(const char *lhs, const char *rhs) {
  while (*lhs && (*lhs == *rhs)) {
    lhs++;
    rhs++;
  }

  return (unsigned char)*lhs - (unsigned char)*rhs;
}

char *strcpy(char *dest, const char *src) {
  char *out = dest;

  while (*src) {
    *dest++ = *src++;
  }

  *dest = '\0';
  return out;
}
