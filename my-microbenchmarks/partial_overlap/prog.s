.section .text
.globl _start
_start:
    # ─────────────────────────────────────────
    # TEST 4: partial overlap (4-byte store, 8-byte load)
    # classic SMB disambiguation failure case
    # ─────────────────────────────────────────
    movl $0xCAFE,  -32(%rsp)    # STORE 4 bytes
    movq -32(%rsp), %rbx        # LOAD  8 bytes — partial overlap
    # ─────────────────────────────────────────
    # exit
    # ─────────────────────────────────────────
    mov  $60,  %rax
    xor  %rdi, %rdi
    syscall