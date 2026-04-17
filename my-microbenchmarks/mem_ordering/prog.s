.section .text
.globl _start
_start:
    # ─────────────────────────────────────────
    # TEST 3: two stores to same address, load gets youngest
    # ─────────────────────────────────────────
    movq $0x1111, %rax
    movq %rax,    -24(%rsp)     # STORE (older)
    movq $0x2222, %rax
    movq %rax,    -24(%rsp)     # STORE (younger, same addr)
    movq -24(%rsp), %rbx        # LOAD  — must get 0x2222
    # ─────────────────────────────────────────
    # exit
    # ─────────────────────────────────────────
    mov  $60,  %rax
    xor  %rdi, %rdi
    syscall