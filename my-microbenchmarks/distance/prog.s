.section .text
.globl _start
_start:
    # ─────────────────────────────────────────
    # TEST 2: store→load with ALU instructions between (distance=5)
    # SMB predictor needs to learn this pattern
    # ─────────────────────────────────────────
    movq $0xBEEF, %rax
    movq %rax,    -16(%rsp)     # STORE
    addq $1, %rcx
    addq $2, %rcx
    addq $3, %rcx
    addq $4, %rcx
    addq $5, %rcx
    movq -16(%rsp), %rbx        # LOAD  (distance 5)
    # ─────────────────────────────────────────
    # exit
    # ─────────────────────────────────────────
    mov  $60,  %rax
    xor  %rdi, %rdi
    syscall