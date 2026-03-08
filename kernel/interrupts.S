.section .text
.global exception_vectors

.equ TF_X0, 0
.equ TF_X1, 8
.equ TF_X2, 16
.equ TF_X3, 24
.equ TF_X4, 32
.equ TF_X5, 40
.equ TF_X6, 48
.equ TF_X7, 56
.equ TF_X8, 64
.equ TF_X9, 72
.equ TF_X10, 80
.equ TF_X11, 88
.equ TF_X12, 96
.equ TF_X13, 104
.equ TF_X14, 112
.equ TF_X15, 120
.equ TF_X16, 128
.equ TF_X17, 136
.equ TF_X18, 144
.equ TF_X19, 152
.equ TF_X20, 160
.equ TF_X21, 168
.equ TF_X22, 176
.equ TF_X23, 184
.equ TF_X24, 192
.equ TF_X25, 200
.equ TF_X26, 208
.equ TF_X27, 216
.equ TF_X28, 224
.equ TF_X29, 232
.equ TF_X30, 240
.equ TF_SP, 248
.equ TF_ELR, 256
.equ TF_SPSR, 264
.equ TF_ESR, 272
.equ TF_FAR, 280
.equ TF_VECTOR, 288
.equ TF_RESERVED, 296
.equ TF_SIZE, 304

.macro vector_slot vector_id, target
  movz x18, #\vector_id
  b \target
.endm

.macro save_gprs
  sub sp, sp, #TF_SIZE
  stp x0, x1, [sp, #TF_X0]
  stp x2, x3, [sp, #TF_X2]
  stp x4, x5, [sp, #TF_X4]
  stp x6, x7, [sp, #TF_X6]
  stp x8, x9, [sp, #TF_X8]
  stp x10, x11, [sp, #TF_X10]
  stp x12, x13, [sp, #TF_X12]
  stp x14, x15, [sp, #TF_X14]
  stp x16, x17, [sp, #TF_X16]
  stp x18, x19, [sp, #TF_X18]
  stp x20, x21, [sp, #TF_X20]
  stp x22, x23, [sp, #TF_X22]
  stp x24, x25, [sp, #TF_X24]
  stp x26, x27, [sp, #TF_X26]
  stp x28, x29, [sp, #TF_X28]
  str x30, [sp, #TF_X30]
.endm

.macro restore_gprs
  ldp x0, x1, [sp, #TF_X0]
  ldp x2, x3, [sp, #TF_X2]
  ldp x4, x5, [sp, #TF_X4]
  ldp x6, x7, [sp, #TF_X6]
  ldp x8, x9, [sp, #TF_X8]
  ldp x10, x11, [sp, #TF_X10]
  ldp x12, x13, [sp, #TF_X12]
  ldp x14, x15, [sp, #TF_X14]
  ldp x16, x17, [sp, #TF_X16]
  ldp x18, x19, [sp, #TF_X18]
  ldp x20, x21, [sp, #TF_X20]
  ldp x22, x23, [sp, #TF_X22]
  ldp x24, x25, [sp, #TF_X24]
  ldp x26, x27, [sp, #TF_X26]
  ldp x28, x29, [sp, #TF_X28]
  ldr x30, [sp, #TF_X30]
  add sp, sp, #TF_SIZE
.endm

.balign 2048
exception_vectors:
  .balign 128
  vector_slot 0, sync_entry
  .balign 128
  vector_slot 1, irq_entry
  .balign 128
  vector_slot 2, fiq_entry
  .balign 128
  vector_slot 3, serror_entry

  .balign 128
  vector_slot 4, sync_entry
  .balign 128
  vector_slot 5, irq_entry
  .balign 128
  vector_slot 6, fiq_entry
  .balign 128
  vector_slot 7, serror_entry

  .balign 128
  vector_slot 8, sync_entry
  .balign 128
  vector_slot 9, irq_entry
  .balign 128
  vector_slot 10, fiq_entry
  .balign 128
  vector_slot 11, serror_entry

  .balign 128
  vector_slot 12, sync_entry
  .balign 128
  vector_slot 13, irq_entry
  .balign 128
  vector_slot 14, fiq_entry
  .balign 128
  vector_slot 15, serror_entry

common_entry:
  save_gprs

  add x0, sp, #TF_SIZE
  str x0, [sp, #TF_SP]

  mrs x0, elr_el1
  str x0, [sp, #TF_ELR]
  mrs x0, spsr_el1
  str x0, [sp, #TF_SPSR]
  mrs x0, esr_el1
  str x0, [sp, #TF_ESR]
  mrs x0, far_el1
  str x0, [sp, #TF_FAR]

  ldr x0, [sp, #TF_X18]
  str x0, [sp, #TF_VECTOR]
  str xzr, [sp, #TF_RESERVED]

  mov x0, sp
  bl exception_dispatch

  restore_gprs
  eret

sync_entry:
  b common_entry

irq_entry:
  b common_entry

fiq_entry:
  b common_entry

serror_entry:
  b common_entry
