.section .data
array:  .quad 0, 0, 0, 0, 0, 0, 0, 0   # 8 x 8-byte slots

.section .text
.globl _start
_start:
    # ─────────────────────────────────────────
    # TEST 5: strided loop — repeated store→load on array
    # high-frequency, regular pattern — easy to predict
    # ─────────────────────────────────────────
    lea   array(%rip), %rsi
    xor   %rcx, %rcx
.loop_stride:
    movq  %rcx,        (%rsi, %rcx, 8)   # STORE array[i] = i
    movq  (%rsi, %rcx, 8), %rbx          # LOAD  array[i]
    inc   %rcx
    cmp   $8, %rcx
    jl    .loop_stride
    # ─────────────────────────────────────────
    # exit
    # ─────────────────────────────────────────
    mov  $60,  %rax
    xor  %rdi, %rdi
    syscall