#include "uart.h"
#include "printf-stdarg.h" // Per UART_printf
#include <stdarg.h>
#include <stddef.h>
// Clock di sistema presunto per il calcolo del baud rate.
// Dovrebbe corrispondere a quello configurato in QEMU e FreeRTOSConfig.h
// #define DEFAULT_SYSTEM_CLOCK 143000000UL // Esempio, da configCPU_CLOCK_HZ

void UART_Init(LPUART_Type *base, uint32_t sysclk, uint32_t baudrate) {
    // Nota: l'abilitazione del clock per LPUART dovrebbe essere gestita
    // a un livello più basso (es. SystemInit o specifico init della board in QEMU).
    // Qui assumiamo che il clock sia già abilitato.

    // Disabilita trasmettitore e ricevitore prima della configurazione
    base->CTRL &= ~(LPUART_CTRL_TE_MASK | LPUART_CTRL_RE_MASK);

    // Calcolo SBR (Baud Rate Modulo Divisor) e OSR (Over Sampling Ratio)
    // Questa è una configurazione semplificata. Per una configurazione precisa,
    // consultare il reference manual S32K3xx per la formula completa
    // e la procedura di ottimizzazione di OSR.
    // Assumiamo un OSR fisso per semplicità, es. 16 (valore OSR = 15).
    uint32_t osr_val = 15; // OSR = osr_val + 1 = 16
    uint16_t sbr_val = (uint16_t)(sysclk / (baudrate * (osr_val + 1)));
    
    // Se sbr_val è 0, il baud rate è troppo alto per il clock e OSR dati.
    // Potrebbe essere necessario aggiustare OSR o usare un clock più veloce.
    if (sbr_val == 0) {
        sbr_val = 1; // Valore minimo per SBR
    }

    base->BAUD = LPUART_BAUD_SBR(sbr_val) | LPUART_BAUD_OSR(osr_val);
    // Potrebbe essere necessario configurare LPUART_BAUD_BOTHEDGE_MASK se OSR < 8

    // Configurazione standard: 8N1 (8 data bits, no parity, 1 stop bit)
    // Questa è spesso la configurazione di default o non richiede modifiche esplicite
    // ai registri CTRL/FIFO per la modalità base.
    // base->CTRL &= ~LPUART_CTRL_M_MASK; // 8-bit data
    // base->CTRL &= ~LPUART_CTRL_PE_MASK; // No parity
    // base->BAUD &= ~LPUART_BAUD_SBNS_MASK; // 1 stop bit (se SBNS=0)

    // Abilita trasmettitore e ricevitore
    base->CTRL |= LPUART_CTRL_TE_MASK | LPUART_CTRL_RE_MASK;
}

void UART_SendChar(LPUART_Type *base, uint8_t data) {
    // Attendi che il buffer di trasmissione sia vuoto
    while (!(base->STAT & LPUART_STAT_TDRE_MASK));
    base->DATA = data;
}

uint8_t UART_ReceiveChar(LPUART_Type *base) {
    // Attendi che il buffer di ricezione sia pieno
    while (!(base->STAT & LPUART_STAT_RDRF_MASK));
    return (uint8_t)(base->DATA & 0xFF);
}

void UART_WriteString(LPUART_Type *base, const char *str) {
    while (*str) {
        UART_SendChar(base, *str++);
    }
}

// Implementazione di UART_printf che usa le funzioni di printf-stdarg.c
// Questa funzione wrapper permette a _putchar di inviare caratteri via UART.
static LPUART_Type *g_uart_printf_base = NULL;

void _putchar(char character) {
    if (g_uart_printf_base) {
        UART_SendChar(g_uart_printf_base, (uint8_t)character);
    }
}

int UART_printf(const char *format, ...) {
    va_list args;
    int count;

    // Imposta la UART di default per printf se non già fatto, o permetti di cambiarla.
    // Per semplicità, usiamo LPUART12 come default se non specificato.
    if (!g_uart_printf_base) {
        g_uart_printf_base = LPUART12; 
    }
    
    va_start(args, format);
    count = tfp_format(NULL, _putchar, format, args);
    va_end(args);
    
    return count;
}
