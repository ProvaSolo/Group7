#ifndef UART_H
#define UART_H

#include <stdint.h>

/* Indirizzi base LPUART (da adattare se diversi nella tua emulazione QEMU) */
/* Questi sono esempi basati sulla GUIDE.md del progetto di riferimento */
#define LPUART0_BASE  0x4006A000
/*
static const uint32_t lpspi_addr[NXP_NUM_LPSPIS] = {0x40358000, 0x4035C000, 0x40360000, 0x40364000};
*/

#define LPUART1_BASE 0x40328000
#define LPUART2_BASE 0x4032C000
#define LPUART3_BASE 0x40330000
#define LPUART4_BASE 0x40334000
#define LPUART5_BASE 0x40338000
#define LPUART6_BASE 0x4033C000
#define LPUART7_BASE 0x40340000
#define LPUART8_BASE 0x40344000

#define LPUART_VERID  0x00  // Version ID Register (Esempio)
#define LPUART_PARAM  0x04  // Parameter Register (Esempio)
#define LPUART_GLOBAL 0x08  // Global Register (Esempio)
#define LPUART_PINCFG 0x0C  // Pin Configuration Register (Esempio)
#define LPUART_BAUD   0x10  // Baud Rate Register
#define LPUART_STAT   0x14  // Status Register - Equivalente di usart_sr
#define LPUART_CTRL   0x18  // Control Register - Equivalente di usart_cr
#define LPUART_DATA   0x1C  // Data Register - Equivalente di usart_dr
#define LPUART_MATCH  0x20  // Match Address Register (Esempio)
#define LPUART_MODIR  0x24  // Modem IrDA Register (Esempio)
#define LPUART_FIFO   0x28  // FIFO Register (Esempio)
#define LPUART_WATER  0x2C  // Watermark Register (Esempio)

// Definizione dei registri LPUART (offset dal base address della LPUART)
#define LPUART_CR1   (*(volatile uint32_t *)(LPUART1_BASE + 0x00))
#define LPUART_CR2   (*(volatile uint32_t *)(LPUART1_BASE + 0x04))
#define LPUART_CR3   (*(volatile uint32_t *)(LPUART1_BASE + 0x08))
#define LPUART_BRR   (*(volatile uint32_t *)(LPUART1_BASE + 0x0C))
#define LPUART_GTPR  (*(volatile uint32_t *)(LPUART1_BASE + 0x10))
#define LPUART_RTOR  (*(volatile uint32_t *)(LPUART1_BASE + 0x14))
#define LPUART_RQR   (*(volatile uint32_t *)(LPUART1_BASE + 0x18))
#define LPUART_ISR   (*(volatile uint32_t *)(LPUART1_BASE + LPUART_DATA ))
#define LPUART_ICR   (*(volatile uint32_t *)(LPUART1_BASE + 0x20))
#define LPUART_RDR   (*(volatile uint32_t *)(LPUART1_BASE + 0x24))
#define LPUART_TDR   (*(volatile uint32_t *)(LPUART1_BASE + 0x28)) 
// Bit dei registri
// STAT Register bits
#define LPUART_STAT_TDRE (1 << 23) // Transmit Data Register Empty
#define LPUART_STAT_RDRF (1 << 21) // Receive Data Register Full

// CTRL Register bits
#define LPUART_CTRL_TE   (1 << 19) // Transmitter Enable
#define LPUART_CTRL_RE   (1 << 18) // Receiver Enable
#define LPUART_CTRL_TIE  (1 << 23) // Transmit Interrupt Enable
#define LPUART_CTRL_RIE  (1 << 21) // Receive Interrupt Enable

/* Bitfields per LPUART_BAUD */
#define LPUART_BAUD_SBR_MASK   (0x1FFFu)  // Baud Rate Modulo Divisor
#define LPUART_BAUD_OSR_MASK   (0x1Fu << 24) // Over Sampling Ratio
#define LPUART_BAUD_OSR(x)     (((x) & 0x1Fu) << 24)

void UART_Init();
void UART_SendChar(uint8_t data);
uint8_t UART_ReceiveChar();
void UART_WriteString(const char *str);

// Funzione printf (da printf-stdarg.c)
int UART_printf(const char *format, ...);

#endif // UART_H
