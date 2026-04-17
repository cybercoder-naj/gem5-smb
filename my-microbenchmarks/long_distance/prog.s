.section .text
.globl _start
_start:
    # ─────────────────────────────────────────
    # TEST 7: long-distance dependence (many ops between store and load)
    # stresses the store buffer depth
    # ─────────────────────────────────────────
    movq $0xFEED, %rax
    movq %rax,    -40(%rsp)     # STORE
    # 20 independent ALU ops to push distance out
    xor %r8,  %r8
    xor %r9,  %r9
    xor %r10, %r10
    xor %r11, %r11
    addq $1,  %r8
    addq $2,  %r9
    addq $3,  %r10
    addq $4,  %r11
    addq $5,  %r8
    addq $6,  %r9
    addq $7,  %r10
    addq $8,  %r11
    addq $9,  %r8
    addq $10, %r9
    addq $11, %r10
    addq $12, %r11
    imulq %r8,  %r9
    imulq %r10, %r11
    imulq %r8,  %r11
    imulq %r9,  %r10
    movq -40(%rsp), %rbx        # LOAD  (distance ~20)
    # ─────────────────────────────────────────
    # exit
    # ─────────────────────────────────────────
    mov  $60,  %rax
    xor  %rdi, %rdi
    syscall