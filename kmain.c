#include "const.h"
#include "sbi.h"
#include "fdt.h"
#include "test.h"

size_t n_hart = 0;
static volatile uintptr_t n_boot_hart = 1;

static volatile uintptr_t __lock = 0;
static volatile uintptr_t *lock = &__lock;

static volatile uintptr_t __boot_permit = 1;
static volatile uintptr_t *boot_permit = &__boot_permit;

static volatile bool done = 0;

void spinlock_lock(volatile uintptr_t *lock);
void spinlock_unlock(volatile uintptr_t *lock);

typedef uintptr_t pte_t;

extern pte_t boot_pdpt[];

void hello(uintptr_t hartid) {
  printf("Hello, world. I'm #%lu!\n", hartid);
}

void init_boothart(uintptr_t hartid, uintptr_t fdt) {
  spinlock_lock(lock);
  hello(hartid);
  puts("  I'm the boot hart :)");
  query_harts(fdt);
  printf("  There are %lu harts in the system.\n", n_hart);
  printf("  Flat Device Tree is at %p\n", fdt);
  spinlock_unlock(lock);

  // wait for other harts to boot
  while (n_boot_hart != n_hart);

  // clear temporary mapping
  boot_pdpt[PDPX(PHY_MEM_BASE)] = 0;
  // TODO: do more mapping

  // other harts are blocked.
  for (int i = 1; i < n_hart; ++i) {
    printf("Starting kernel hart %lu.\n", i);
    done = 0;
    spinlock_unlock(boot_permit); // signal
    while (!done);
    printf("Kernel hart %lu started.\n", i);
  }
  test(fdt);
  spinlock_lock(lock);
  puts("System booted successfully! :)");
  putchar('#');
  spinlock_unlock(lock);
  for (;;) {
    int ch = sbi_console_getchar();
    if (ch == 3) {
      putchar('\n');
      return;
    }
    if (ch == '\r') ch = '\n';
    if (ch > 0) {
      putchar(ch);
    }
  }
  // TODO: call scheduler
  for (;;) asm volatile("wfi");
}

void init_others(uintptr_t hartid, uintptr_t fdt) {
  spinlock_lock(lock);
  hello(hartid);
  puts("  I'm not boot hart T_T. Waiting for boot hart...");
  ++n_boot_hart;
  asm volatile("fence"); // wmb?
  spinlock_unlock(lock);
  spinlock_lock(boot_permit); // wait for signal
  puts("Pretend doing something...");
  done = 1;
  // TODO: call scheduler
  for (;;) asm volatile("wfi");
}

void kmain(uintptr_t hartid, uintptr_t fdt) {
  spinlock_lock(lock);
  setup_stdio(sbi_console_putchar, sbi_console_getchar);
  spinlock_unlock(lock);
  if (hartid == 0) {
    init_boothart(hartid, fdt);
  } else {
    init_others(hartid, fdt);
  }
  sbi_shutdown();
}
