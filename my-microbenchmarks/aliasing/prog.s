.section .text
.globl _start
_start:
    # ─────────────────────────────────────────
    # TEST 6: aliasing — two different addresses collide
    # in a direct-mapped store buffer (same low bits)
    # ─────────────────────────────────────────
    sub  $128, %rsp
    movq $0xAAAA, %rax
    movq %rax,    64(%rsp)     # STORE  addr A
    movq $0xBBBB, %rax
    movq %rax,    (%rsp)    # STORE  addr B (64 bytes apart — same cache line index on many configs)
    movq 64(%rsp),  %rbx       # LOAD   addr A — may alias with B in predictor
    movq (%rsp), %rdx       # LOAD   addr B
    add $128, %rsp
    # ─────────────────────────────────────────
    # exit
    # ─────────────────────────────────────────
    mov  $60,  %rax
    xor  %rdi, %rdi
    syscall
    