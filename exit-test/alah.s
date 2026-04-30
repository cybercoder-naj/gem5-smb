.section .text
.globl _start

_start:
  xor %rax, %rax
  movb $0xcd, %al
  movb $0xab, %ah

  movq %rax, %rdi
  movq $60, %rax
  syscall
  