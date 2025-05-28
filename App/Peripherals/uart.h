#ifndef UART_H
#define UART_H

#include <stdint.h>

/* Indirizzi base LPUART (da adattare se diversi nella tua emulazione QEMU) */
/* Questi sono esempi basati sulla GUIDE.md del progetto di riferimento */
#define LPUART0_BASE  0x4006A000
#define LPUART1_BASE  0x4006B000
// Aggiungi altri LPUART base se necessario...
#define LPUART12_BASE 0x40076000 // Usato spesso per console/debug

/* Definizione della struttura dei registri LPUART (semplificata) */
/* Fare riferimento al manuale S32K3 per la struttura completa */
typedef struct {
    volatile uint32_t VERID;    /* Version ID Register */
    volatile uint32_t PARAM;    /* Parameter Register */
    volatile uint32_t GLOBAL;   /* LPUART Global Register */
    volatile uint32_t PINCFG;   /* LPUART Pin Configuration Register */
    volatile uint32_t BAUD;     /* LPUART Baud Rate Register */
    volatile uint32_t STAT;     /* LPUART Status Register */
    volatile uint32_t CTRL;     /* LPUART Control Register */
    volatile uint32_t DATA;     /* LPUART Data Register */
    volatile uint32_t MATCH;    /* LPUART Match Address Register */
    volatile uint32_t MODIR;    /* LPUART Modem IrDA Register */
    volatile uint32_t FIFO;     /* LPUART FIFO Register */
    volatile uint32_t WATER;    /* LPUART Watermark Register */
} LPUART_Type;

#define LPUART0 ((LPUART_Type *)LPUART0_BASE)
#define LPUART1 ((LPUART_Type *)LPUART1_BASE)
// ...
#define LPUART12 ((LPUART_Type *)LPUART12_BASE)


/* Bitfields per LPUART_STAT */
#define LPUART_STAT_TDRE_MASK  (1u << 23) // Transmit Data Register Empty Flag
#define LPUART_STAT_RDRF_MASK  (1u << 21) // Receive Data Register Full Flag

/* Bitfields per LPUART_CTRL */
#define LPUART_CTRL_TE_MASK    (1u << 19) // Transmitter Enable
#define LPUART_CTRL_RE_MASK    (1u << 18) // Receiver Enable

/* Bitfields per LPUART_BAUD */
#define LPUART_BAUD_SBR_MASK   (0x1FFFu)  // Baud Rate Modulo Divisor
#define LPUART_BAUD_OSR_MASK   (0x1Fu << 24) // Over Sampling Ratio
#define LPUART_BAUD_OSR(x)     (((x) & 0x1Fu) << 24)

void UART_Init(LPUART_Type *base, uint32_t sysclk, uint32_t baudrate);
void UART_SendChar(LPUART_Type *base, uint8_t data);
uint8_t UART_ReceiveChar(LPUART_Type *base);
void UART_WriteString(LPUART_Type *base, const char *str);

// Funzione printf (da printf-stdarg.c)
int UART_printf(const char *format, ...);

#endif // UART_H
