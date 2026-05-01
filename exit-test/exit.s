.section .text
.globl _start

_start:
    sub  $8, %rsp

    movw $0x1234, %bx    
    movq $0x3421, (%rsp) 

    movb (%rsp), %bl     
    movq (%rsp), %rcx

    mov  $60, %rax      # sys_exit
    movq %rbx, %rdi     # status = 0x1221

    add  $4, %rsp
    syscall
