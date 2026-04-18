.section .text
.globl _start
_start:
    # ─────────────────────────────────────────
    # TEST 3: two stores to same address, load gets youngest
    # ─────────────────────────────────────────
    sub  $8, %rsp
    movq $0x1111, %rax
    movq %rax,    (%rsp)     # STORE (older)
    movq $0x2222, %rax
    movq %rax,    (%rsp)     # STORE (younger, same addr)
    movq (%rsp), %rbx        # LOAD  — must get 0x2222
    add  $8, %rsp
    # ─────────────────────────────────────────
    # exit
    # ─────────────────────────────────────────
    mov  $60,  %rax
    xor  %rdi, %rdi
    syscall
