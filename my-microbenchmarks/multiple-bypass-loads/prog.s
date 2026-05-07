.section .text         
.globl _start
_start:
    sub  $16,      %rsp
    movl  $5,    (%rsp)
    movl $10,  4(%rsp)
    movl $15,  8(%rsp)
    movl $20, 12(%rsp)

    movl    (%rsp), %eax   # eax = 5
    movl  4(%rsp), %esi    # esi = 10 
    movl  8(%rsp), %ecx    # ecx = 15
    movl 12(%rsp), %edx    # edx = 20

    addl %eax, %esi        # esi = 5  + 10 = 15
    addl %ecx, %edx        # edx = 15 + 20 = 35
    subl %esi, %edx        # edx = 35 - 15 = 20

    add $16, %rsp      

    movl %edx, %edi        # exit code = 20
    mov  $60,  %rax        # syscall: exit
    syscall
