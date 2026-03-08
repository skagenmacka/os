#include "kalloc.h"

#include <stdint.h>

#define PAGE_SIZE 4096UL     // 4 KiB page
#define PHYSTOP 0x48000000UL // physical memory end

struct run {
  struct run *next;
};

static struct run *freelist;
extern char __kernel_end[];

static uint64_t align_up(uint64_t value, uint64_t align) {
  return (value + align - 1) & ~(align - 1);
}

void kinit(void) {
  uint64_t start = align_up((uint64_t)__kernel_end, PAGE_SIZE);

  for (uint64_t p = start; p + PAGE_SIZE <= PHYSTOP; p += PAGE_SIZE) {
    kfree((void *)p);
  }
}

void *kalloc(void) {
  struct run *r = freelist;

  if (r == 0) {
    return 0;
  }

  freelist = r->next;
  return (void *)r;
}

void kfree(void *p) {
  struct run *r = (struct run *)p;

  if (((uint64_t)p % PAGE_SIZE) != 0) {
    while (1) {
    }
  }

  r->next = freelist;
  freelist = r;
}
