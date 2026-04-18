.section .text
.globl _start
_start:
    # ─────────────────────────────────────────
    # TEST 2: store→load with ALU instructions between (distance=5)
    # SMB predictor needs to learn this pattern
    # ─────────────────────────────────────────
    sub  $8, %rsp           # so gem5 don't squash

    movq $0xBEEF, %rax
    movq %rax,    (%rsp)     # STORE
    addq $1, %rcx
    addq $2, %rcx
    addq $3, %rcx
    addq $4, %rcx
    addq $5, %rcx
    movq (%rsp), %rbx        # LOAD  (distance 5)

    add  $8, %rsp
    # ─────────────────────────────────────────
    # exit
    # ─────────────────────────────────────────
    mov  $60,  %rax
    xor  %rdi, %rdi
    syscall
