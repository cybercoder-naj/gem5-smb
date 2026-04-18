.section .text
.globl _start
_start:
    # ─────────────────────────────────────────
    # TEST 1: back-to-back store→load (distance=1)
    # should forward perfectly
    # ─────────────────────────────────────────
    sub  $8, %rsp              # required so gem5 don't squash instructions

    movq $0xDEAD, %rax
    movq %rax,    (%rsp)       # STORE
    movq (%rsp),  %rbx         # LOAD  (distance 1)

    add $8, %rsp

    # -----------------------------------------
    # Using %rbp
    # -----------------------------------------
    mov %rbx, %rdi

    # ─────────────────────────────────────────
    # exit
    # ─────────────────────────────────────────
    mov  $60,  %rax
    xor  %rdi, %rdi
    syscall
