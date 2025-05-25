.syntax unified
.cpu cortex-m7
.thumb

.section .vector_table, "a", %progbits
.word   _stack_top      /* Initial Stack Pointer */
.word   _reset_handler  /* Reset Handler */

.section .text
.global _reset_handler
.type _reset_handler, %function

_reset_handler:
    b .

.section .stack, "a", %progbits
_stack_top:
    .word 0x20020000    /* Example stack top (128KB SRAM) */