.section .text
.globl _start
_start:
    # ─────────────────────────────────────────
    # TEST 1: back-to-back store→load (distance=1)
    # should forward perfectly
    # ─────────────────────────────────────────
    movq $0xDEAD, %rax
    movq %rax,    -8(%rsp)      # STORE
    movq -8(%rsp), %rbx         # LOAD  (distance 1)
    # ─────────────────────────────────────────
    # exit
    # ─────────────────────────────────────────
    mov  $60,  %rax
    xor  %rdi, %rdi
    syscall