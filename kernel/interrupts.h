#pragma once

#include <stdint.h>

enum exception_vector {
  VECTOR_CURRENT_SP0_SYNC = 0,
  VECTOR_CURRENT_SP0_IRQ = 1,
  VECTOR_CURRENT_SP0_FIQ = 2,
  VECTOR_CURRENT_SP0_SERROR = 3,
  VECTOR_CURRENT_SPX_SYNC = 4,
  VECTOR_CURRENT_SPX_IRQ = 5,
  VECTOR_CURRENT_SPX_FIQ = 6,
  VECTOR_CURRENT_SPX_SERROR = 7,
  VECTOR_LOWER_A64_SYNC = 8,
  VECTOR_LOWER_A64_IRQ = 9,
  VECTOR_LOWER_A64_FIQ = 10,
  VECTOR_LOWER_A64_SERROR = 11,
  VECTOR_LOWER_A32_SYNC = 12,
  VECTOR_LOWER_A32_IRQ = 13,
  VECTOR_LOWER_A32_FIQ = 14,
  VECTOR_LOWER_A32_SERROR = 15,
};

struct trap_frame {
  uint64_t x[31];
  uint64_t sp;
  uint64_t elr_el1;
  uint64_t spsr_el1;
  uint64_t esr_el1;
  uint64_t far_el1;
  uint64_t vector;
  uint64_t reserved;
};

void exception_dispatch(struct trap_frame *frame);

static inline void irq_enable(void) {
  __asm__ volatile("msr daifclr, #2" ::: "memory");
}

static inline void irq_disable(void) {
  __asm__ volatile("msr daifset, #2" ::: "memory");
}

static inline unsigned long irq_save(void) {
  unsigned long flags;
  __asm__ volatile("mrs %0, daif" : "=r"(flags));
  return flags;
}

static inline void irq_restore(unsigned long flags) {
  __asm__ volatile("msr daif, %0" ::"r"(flags) : "memory");
}
