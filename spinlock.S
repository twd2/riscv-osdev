.section .text, "ax", %progbits
.globl spinlock_lock
spinlock_lock:
  li t0, 1 # Initialize swap value.
  again:
    amoswap.d.aq t0, t0, (a0) # Attempt to acquire lock.
    bnez t0, again # Retry if held.
  ret

.globl spinlock_unlock
spinlock_unlock:
  amoswap.d.rl x0, x0, (a0) # Release lock by storing 0.
  ret
