.section .text
.global _start

_start:
  mrs x0, cpacr_el1
  orr x0, x0, #(3 << 20)
  msr cpacr_el1, x0
  isb

  ldr x0, =stack_top
  mov sp, x0
  bl kernel_main

1:
  wfi
  b 1b

.section .bss
.align 12
stack:
  .skip 4096
stack_top:
