.section .text
.globl _start
_start:
    # ─────────────────────────────────────────
    # TEST 4: partial overlap (4-byte store, 8-byte load)
    # classic SMB disambiguation failure case
    # ─────────────────────────────────────────
    sub $8, %rsp
    movl $0xCAFE, (%rsp)    # STORE 4 bytes
    movq (%rsp),  %rbx        # LOAD  8 bytes — partial overlap
    add $8, %rsp

    # ─────────────────────────────────────────
    # exit
    # ─────────────────────────────────────────
    mov  $60,  %rax
    xor  %rdi, %rdi
    syscall
