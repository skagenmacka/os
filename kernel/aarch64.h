#pragma once

static inline unsigned long cpuid() {
  unsigned long x;
  // read system register, return the current CPU/core running
  asm volatile("mrs %0, mpidr_el1" : "=r"(x));
  return x & 0xff;
}