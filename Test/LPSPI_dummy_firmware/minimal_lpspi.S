.syntax unified
.cpu cortex-m7
.thumb

.section .vector_table, "a", %progbits
.word   _stack_top
.word   _reset_handler

.section .text
.global _reset_handler
.type _reset_handler, %function

/* LPSPI0 base address */
.equ LPSPI0_BASE, 0x40358000
.equ LPSPI_TDR,   (LPSPI0_BASE + 0x34)   /* Transmit Data Register (TDR) */
.equ LPSPI_SR,    (LPSPI0_BASE + 0x10)   /* Status Register (SR) */
.equ LPSPI_SR_TDF, (1 << 1)              /* Transmit Data Flag (bit 1) */

_reset_handler:
    ldr r0, =0xA5         /* Valore da trasmettere */
wait_tdf:
    ldr r1, =LPSPI_SR
    ldr r2, [r1]
    tst r2, #LPSPI_SR_TDF
    beq wait_tdf
    ldr r1, =LPSPI_TDR
    str r0, [r1]

stop:
    wfi
    b stop

.section .stack, "a", %progbits
_stack_top:
    .word 0x20020000