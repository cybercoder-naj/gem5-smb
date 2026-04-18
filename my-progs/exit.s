.section .text
.globl _start

_start:
    sub $8, %rsp

    movq $0xDEAD, %rax
    movq %rax, (%rsp)
    movq (%rsp), %rbx

    add $8, %rsp

    mov  $60, %rax      # sys_exit
    xor  %rdi, %rdi
    syscall
