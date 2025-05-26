.syntax unified
.cpu cortex-m7
.thumb

.section .vector_table, "a", %progbits
.word   _stack_top
.word   _reset_handler

.section .text
.global _reset_handler
.type _reset_handler, %function

/* LPUART0 base address */
.equ LPUART0_BASE, 0x40328000
.equ LPUART_TDR,   (LPUART0_BASE + 0x1C)   /* Transmit Data Register */
.equ LPUART_SR,    (LPUART0_BASE + 0x14)   /* Status Register */
.equ LPUART_SR_TDRE, (1 << 23)             /* Transmit Data Register Empty flag (bit 23) */

_reset_handler:
    ldr r4, =10              /* Print count */

print_loop:
    ldr r5, =msg             /* Pointer to string */
next_char:
    ldrb r6, [r5], #1        /* Load next char, increment pointer */
    cmp r6, #0
    beq end_line
wait_tdre:
    ldr r7, =LPUART_SR
    ldr r8, [r7]
    tst r8, #LPUART_SR_TDRE
    beq wait_tdre
    ldr r7, =LPUART_TDR
    str r6, [r7]
    b next_char
end_line:
    subs r4, r4, #1
    bne print_loop

stop:
    wfi
    b stop

.align 2
msg:      .asciz "Hello\n"

.section .stack, "a", %progbits
_stack_top:
    .word 0x20020000