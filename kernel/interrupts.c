#include "interrupts.h"
#include "io.h"
#include "timer.h"

#define ESR_EC_SHIFT 26U
#define ESR_EC_MASK 0x3fU

static const char *vector_name(uint64_t vector) {
  switch (vector) {
  case VECTOR_CURRENT_SP0_SYNC:
    return "current EL with SP0 sync";
  case VECTOR_CURRENT_SP0_IRQ:
    return "current EL with SP0 irq";
  case VECTOR_CURRENT_SP0_FIQ:
    return "current EL with SP0 fiq";
  case VECTOR_CURRENT_SP0_SERROR:
    return "current EL with SP0 serror";
  case VECTOR_CURRENT_SPX_SYNC:
    return "current EL with SPx sync";
  case VECTOR_CURRENT_SPX_IRQ:
    return "current EL with SPx irq";
  case VECTOR_CURRENT_SPX_FIQ:
    return "current EL with SPx fiq";
  case VECTOR_CURRENT_SPX_SERROR:
    return "current EL with SPx serror";
  case VECTOR_LOWER_A64_SYNC:
    return "lower EL AArch64 sync";
  case VECTOR_LOWER_A64_IRQ:
    return "lower EL AArch64 irq";
  case VECTOR_LOWER_A64_FIQ:
    return "lower EL AArch64 fiq";
  case VECTOR_LOWER_A64_SERROR:
    return "lower EL AArch64 serror";
  case VECTOR_LOWER_A32_SYNC:
    return "lower EL AArch32 sync";
  case VECTOR_LOWER_A32_IRQ:
    return "lower EL AArch32 irq";
  case VECTOR_LOWER_A32_FIQ:
    return "lower EL AArch32 fiq";
  case VECTOR_LOWER_A32_SERROR:
    return "lower EL AArch32 serror";
  default:
    return "unknown";
  }
}

static const char *esr_ec_name(uint64_t ec) {
  switch (ec) {
  case 0x00:
    return "unknown reason";
  case 0x01:
    return "trapped WFI/WFE";
  case 0x07:
    return "FP/SIMD access";
  case 0x11:
    return "SVC AArch32";
  case 0x15:
    return "SVC AArch64";
  case 0x18:
    return "trapped MSR/MRS";
  case 0x20:
    return "instruction abort lower EL";
  case 0x21:
    return "instruction abort same EL";
  case 0x22:
    return "PC alignment fault";
  case 0x24:
    return "data abort lower EL";
  case 0x25:
    return "data abort same EL";
  case 0x26:
    return "SP alignment fault";
  case 0x2c:
    return "floating point exception";
  case 0x2f:
    return "SError";
  default:
    return "unclassified";
  }
}

static void panic_exception(struct trap_frame *frame, const char *kind) {
  uint64_t ec = (frame->esr_el1 >> ESR_EC_SHIFT) & ESR_EC_MASK;

  put_string("\r\nKERNEL PANIC: ");
  put_string(kind);
  put_string("\r\nvector: ");
  put_string(vector_name(frame->vector));
  put_string("\r\nesr_el1: ");
  put_hex64(frame->esr_el1);
  put_string(" (");
  put_string(esr_ec_name(ec));
  put_string(")\r\n");
  put_string("elr_el1: ");
  put_hex64(frame->elr_el1);
  put_string("\r\nfar_el1: ");
  put_hex64(frame->far_el1);
  put_string("\r\nspsr_el1: ");
  put_hex64(frame->spsr_el1);
  put_string("\r\nsp: ");
  put_hex64(frame->sp);
  put_string("\r\nx0: ");
  put_hex64(frame->x[0]);
  put_string("\r\nx1: ");
  put_hex64(frame->x[1]);
  put_string("\r\nx2: ");
  put_hex64(frame->x[2]);
  put_string("\r\nx3: ");
  put_hex64(frame->x[3]);
  put_string("\r\nx29: ");
  put_hex64(frame->x[29]);
  put_string("\r\nx30: ");
  put_hex64(frame->x[30]);
  put_string("\r\n");

  irq_disable();
  while (1) {
  }
}

void exception_dispatch(struct trap_frame *frame) {
  switch (frame->vector) {
  case VECTOR_CURRENT_SP0_SYNC:
  case VECTOR_CURRENT_SPX_SYNC:
  case VECTOR_LOWER_A64_SYNC:
  case VECTOR_LOWER_A32_SYNC:
    panic_exception(frame, "synchronous exception");
    break;

  case VECTOR_CURRENT_SP0_IRQ:
  case VECTOR_CURRENT_SPX_IRQ:
  case VECTOR_LOWER_A64_IRQ:
  case VECTOR_LOWER_A32_IRQ:
    irq_handle();
    return;
    break;

  case VECTOR_CURRENT_SP0_FIQ:
  case VECTOR_CURRENT_SPX_FIQ:
  case VECTOR_LOWER_A64_FIQ:
  case VECTOR_LOWER_A32_FIQ:
    panic_exception(frame, "unhandled fiq");
    break;

  case VECTOR_CURRENT_SP0_SERROR:
  case VECTOR_CURRENT_SPX_SERROR:
  case VECTOR_LOWER_A64_SERROR:
  case VECTOR_LOWER_A32_SERROR:
    panic_exception(frame, "system error");
    break;

  default:
    panic_exception(frame, "unknown exception vector");
    break;
  }
}
