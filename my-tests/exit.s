.section .text
.globl _start

_start:
    sub  $8, %rsp

    movw $0x1234, %bx    
    movw $0x3421, -4(%rsp) 

    movb -4(%rsp), %bl     

    mov  $60, %rax      # sys_exit
    movq %rbx, %rdi     # status = 0x1221

    add  $8, %rsp
    syscall
