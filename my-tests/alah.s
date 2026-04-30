.section .text
.globl _start

_start:
  xor %rax, %rax
  
  movb $0xab, %ah         # index 64, flat: 0
  movb $0xcd, %al         # index 0, flat: 0
  movw $0xabcd, %ax       # index 0, flat: 0
  movl $0xabcdef, %eax    # index 0, flat: 0
  movq $0x90abcdef, %rax  # index 0, flat: 0

  movb $0xab, %ch         # index 65, flat: 1
  movb $0xcd, %cl         # index 1, flat: 1
  movw $0xabcd, %cx       # index 1, flat: 1
  movl $0xabcdef, %ecx    # index 1, flat: 1
  movq $0x90abcdef, %rcx  # index 1, flat: 1

  movb $0xab, %dh         # index 66, flat: 2
  movb $0xcd, %dl         # index 2, flat: 2
  movw $0xabcd, %dx       # index 2, flat: 2
  movl $0xabcdef, %edx    # index 2, flat: 2
  movq $0x90abcdef, %rdx  # index 2, flat: 2

  movb $0xab, %bh         # index 67, flat: 3
  movb $0xcd, %bl         # index 3, flat: 3
  movw $0xabcd, %bx       # index 3, flat: 3
  movl $0xabcdef, %ebx    # index 3, flat: 3
  movq $0x90abcdef, %rbx  # index 3, flat: 3

  movb $0xab, %spl        # index 4, flat: 4
  movw $0xabcd, %sp       # index 4, flat: 4
  movl $0xabcdef, %esp    # index 4, flat: 4
  movq $0x90abcdef, %rsp  # index 4, flat: 4
  
  movb $0xab, %bpl        # index 5, flat: 5
  movw $0xabcd, %bp       # index 5, flat: 5
  movl $0xabcdef, %ebp    # index 5, flat: 5
  movq $0x90abcdef, %rbp  # index 5, flat: 5

  movb $0xab, %sil        # index 6, flat: 6
  movw $0xabcd, %si       # index 6, flat: 6
  movl $0xabcdef, %esi    # index 6, flat: 6
  movq $0x90abcdef, %rsi  # index 6, flat: 6

  movb $0xab, %dil        # index 7, flat: 7
  movw $0xabcd, %di       # index 7, flat: 7
  movl $0xabcdef, %edi    # index 7, flat: 7
  movq $0x90abcdef, %rdi  # index 7, flat: 7

  movb $0xab, %r8b        # index 8, flat:
  movw $0xabcd, %r8w      # index 8, flat:
  movl $0xabcdef, %r8d    # index 8, flat:
  movq $0x90abcdef, %r8   # index 8, flat:

  movb $0xab, %r9b        # index 9, flat:
  movw $0xabcd, %r9w      # index 9, flat:
  movl $0xabcdef, %r9d    # index 9, flat:
  movq $0x90abcdef, %r9   # index 9, flat:

  movb $0xab, %r10b       # index 10, flat:
  movw $0xabcd, %r10w     # index 10, flat:
  movl $0xabcdef, %r10d   # index 10, flat:
  movq $0x90abcdef, %r10  # index 10, flat:

  movb $0xab, %r11b       # index 11, flat:
  movw $0xabcd, %r11w     # index 11, flat:
  movl $0xabcdef, %r11d   # index 11, flat:
  movq $0x90abcdef, %r11  # index 11, flat:

  movb $0xab, %r12b       # index 12, flat:
  movw $0xabcd, %r12w     # index 12, flat:
  movl $0xabcdef, %r12d   # index 12, flat:
  movq $0x90abcdef, %r12  # index 12, flat:

  movb $0xab, %r13b       # index 13, flat:
  movw $0xabcd, %r13w     # index 13, flat:
  movl $0xabcdef, %r13d   # index 13, flat:
  movq $0x90abcdef, %r13  # index 13, flat:

  movb $0xab, %r14b       # index 14, flat:
  movw $0xabcd, %r14w     # index 14, flat:
  movl $0xabcdef, %r14d   # index 14, flat:
  movq $0x90abcdef, %r14  # index 14, flat:

  movb $0xab, %r15b        # index 15, flat: 15
  movw $0xabcd, %r15w      # index 15, flat: 15
  movl $0xabcdef, %r15d    # index 15, flat: 15
  movq $0x90abcdef, %r15   # index 15, flat: 15

  movq %rax, %rdi
  movq $60, %rax
  syscall
  