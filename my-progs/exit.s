.section .text
.globl _start

_start:
    sub  $4, %rsp
    movl $5, (%rsp)
    movl $10, (%rsp)
    movl (%rsp), %ebx

    mov  $60, %rax      # sys_exit
    movq $0, %rdi     # status = 5

    add  $4, %rsp
    syscall
