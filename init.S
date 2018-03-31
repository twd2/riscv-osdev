#include "const.h"

.section .text, "ax", %progbits
.globl kernel_entry
kernel_entry:
  # setup paging
  la t0, boot_pdpt
  srli t0, t0, 12
  li t1, 0x8000000000000000 # Sv39
  or t0, t0, t1
  csrw satp, t0

  # reallocate
  la t0, reallocated
  li t1, KERNBASE
  add t0, t0, t1
  add a1, a1, t1
  jr t0

reallocated:
  # setup traps
  la t0, __alltraps
  csrw stvec, t0

  # setup sp
  mv sp, a0
  slli sp, sp, PGSHIFT
  la t0, boot_kstack
  add sp, sp, t0

  # a0 = hartid
  # a1 = fdt
  tail kmain

.section .data
.balign PGSIZE
.globl boot_pdpt
boot_pdpt:
  .space (0x80000000 / PGSIZE / 512 / 512 * 8) - (. - boot_pdpt)
  # map va 2G~3G to pa 2G~3G (temporary)
  .quad 0x80000000 / 4 + (PTE_V | PTE_R | PTE_W | PTE_X)
  .space (((KERNBASE & SV39_MASK) + 0x80000000) / PGSIZE / 512 / 512 * 8) - (. - boot_pdpt)
  # map va KERNBASE + (2G~3G) to pa 2G~3G
  .quad 0x80000000 / 4 + (PTE_V | PTE_R | PTE_W | PTE_X)
  .space PGSIZE - (. - boot_pdpt)

.section .bss
boot_kstack:
.balign PGSIZE
  .space MAX_HARTS * PGSIZE
boot_kstack_end:

