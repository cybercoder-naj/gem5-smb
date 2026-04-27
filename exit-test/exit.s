.section .text
.globl _start

_start:
    sub  $8, %rsp

    movl $10, -4(%rsp)
    movl -4(%rsp), %ebx
    movl %ebx, (%rsp)
    movl (%rsp), %ecx

    mov  $60, %rax      # sys_exit
    movq %rcx, %rdi     # status = 10

    add  $4, %rsp
    syscall
