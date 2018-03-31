.section .text, "ax", %progbits
.globl __alltraps
.balign 4
__alltraps:
  csrr s0, scause
  csrr s1, sbadaddr
  csrr s2, sepc
  j __alltraps
