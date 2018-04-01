#ifndef _CONST_H_
#define _CONST_H_

#define PGSIZE 4096
#define PGSHIFT 12
#define MAX_HARTS 16
#define PHY_MEM_BASE 0x80000000
#define KERNBASE 0xffffffc000000000 // Sv39
// #define KERNBASE 0xffff800000000000 // Sv48
#define VA_MASK 0x7fffffffff // Sv39
// #define VA_MASK 0xffffffffffff // Sv48
#define PTE_V 1
#define PTE_R 2
#define PTE_W 4
#define PTE_X 8
#define PTE_SIZE 8
#define SATP_SV39 0x8000000000000000
#define SATP_SV48 0x9000000000000000
#define N_PTE 512
#define PTX_SHIFT (PGSHIFT)
#define PTX(x) ((((x) & VA_MASK) >> PTX_SHIFT) & 0x1ff)
#define N_PDE 512
#define PDX_SHIFT (PTX_SHIFT + 9)
#define PDX(x) ((((x) & VA_MASK) >> PDX_SHIFT) & 0x1ff)
#define N_PDPE 512
#define PDPX_SHIFT (PDX_SHIFT + 9)
#define PDPX(x) ((((x) & VA_MASK) >> PDPX_SHIFT) & 0x1ff)
#define N_PML4E 512
#define PML4X_SHIFT (PDPX_SHIFT + 9)
#define PML4X(x) ((((x) & VA_MASK) >> PML4X_SHIFT) & 0x1ff)
#define __VA(x) ((void *)((x) + KERNBASE))
#define __PA(x) ((uintptr_t)((uintptr_t)(x) - KERNBASE))

#define mb() do { asm volatile ("fence"); } while (0)
#define rmb() do { asm volatile ("fence"); } while (0)
#define wmb() do { asm volatile ("fence"); } while (0)

#endif // _CONST_H_
