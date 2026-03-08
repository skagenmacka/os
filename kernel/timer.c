#include "timer.h"

#include <stdint.h>

#include "io.h"

#define GICD_BASE 0x08000000UL
#define GICC_BASE 0x08010000UL

#define GICD_CTLR ((volatile uint32_t *)(GICD_BASE + 0x000))
#define GICD_ISENABLER0 ((volatile uint32_t *)(GICD_BASE + 0x100))
#define GICD_ISENABLER1 ((volatile uint32_t *)(GICD_BASE + 0x104))
#define GICD_IPRIORITYR ((volatile uint8_t *)(GICD_BASE + 0x400))
#define GICD_ITARGETSR ((volatile uint8_t *)(GICD_BASE + 0x800))

#define GICC_CTLR ((volatile uint32_t *)(GICC_BASE + 0x0000))
#define GICC_PMR ((volatile uint32_t *)(GICC_BASE + 0x0004))
#define GICC_IAR ((volatile uint32_t *)(GICC_BASE + 0x000c))
#define GICC_EOIR ((volatile uint32_t *)(GICC_BASE + 0x0010))

#define TIMER_PPI 30U
#define UART0_IRQ 33U
#define TIMER_HZ 100U

static uint64_t ticks;
static uint64_t timer_reload_value;

static uint32_t gic_acknowledge_irq(void) { return *GICC_IAR; }

static void gic_end_of_interrupt(uint32_t irq) { *GICC_EOIR = irq; }

static void timer_rearm(void) {
  __asm__ volatile("msr cntp_tval_el0, %0" ::"r"(timer_reload_value));
}

static void gic_init(void) {
  GICD_IPRIORITYR[TIMER_PPI] = 0x80;
  GICD_IPRIORITYR[UART0_IRQ] = 0x80;
  GICD_ITARGETSR[UART0_IRQ] = 0x01;

  *GICD_ISENABLER0 |= (1U << TIMER_PPI);
  *GICD_ISENABLER1 |= (1U << (UART0_IRQ - 32U));

  *GICC_PMR = 0xff;
  *GICC_CTLR = 0x1;
  *GICD_CTLR = 0x1;
}

void timer_init(void) {
  uint64_t cntfrq;

  gic_init();

  __asm__ volatile("mrs %0, cntfrq_el0" : "=r"(cntfrq));
  timer_reload_value = cntfrq / TIMER_HZ;
  if (timer_reload_value == 0) {
    timer_reload_value = 1;
  }

  timer_rearm();
  __asm__ volatile("msr cntp_ctl_el0, %0" ::"r"(1UL));
  uart_irq_init();
}

void irq_handle(void) {
  uint32_t irq = gic_acknowledge_irq();

  if (irq >= 1020U) {
    return;
  }

  if (irq == TIMER_PPI) {
    ticks++;
    timer_rearm();
  } else if (irq == UART0_IRQ) {
    uart_handle_irq();
  }

  gic_end_of_interrupt(irq);
}

uint64_t timer_get_ticks(void) { return ticks; }
