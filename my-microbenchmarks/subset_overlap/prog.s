.section .text
.globl _start
_start:
    # ─────────────────────────────────────────
    # TEST 4: partial overlap (4-byte store, 8-byte load)
    # classic SMB disambiguation failure case
    # ─────────────────────────────────────────
    sub $8, %rsp
    movl $0xDEADBEEF, (%rsp)    # STORE 4 bytes
    movw (%rsp),  %bx        # LOAD  2 bytes — partial overlap
    add $8, %rsp

    # ─────────────────────────────────────────
    # exit
    # ─────────────────────────────────────────
    mov  $60,  %rax
    xor  %rdi, %rdi
    syscall
