#ifndef HW_CHAR_NXPS32K358_LPUART_H
#define HW_CHAR_NXPS32K358_LPUART_H

#include "hw/sysbus.h"
#include "chardev/char-fe.h"
#include "qom/object.h"



// Definizione dei registri LPUART (offset dal base address della LPUART)
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

// Bit dei registri (ESEMPI - verifica sul manuale!)
// STAT Register bits
#define LPUART_STAT_TDRE (1 << 23) // Transmit Data Register Empty
#define LPUART_STAT_RDRF (1 << 21) // Receive Data Register Full

// CTRL Register bits
#define LPUART_CTRL_TE   (1 << 19) // Transmitter Enable
#define LPUART_CTRL_RE   (1 << 18) // Receiver Enable
#define LPUART_CTRL_TIE  (1 << 23) // Transmit Interrupt Enable
#define LPUART_CTRL_RIE  (1 << 21) // Receive Interrupt Enable

// BAUD masks
// --- Campo OSR (Over Sampling Ratio) ---
// Bits 28-24 (5 bit totali)
#define LPUART_BAUD_OSR_SHIFT   24U // Posizione del bit meno significativo (LSB) del campo OSR
#define LPUART_BAUD_OSR_WIDTH   5U  // Numero di bit occupati dal campo OSR
// Maschera per isolare i bit di OSR: crea una maschera di 'WIDTH' bit a 1 ( (1U << WIDTH) - 1 )
// e poi la sposta nella posizione corretta (_SHIFT).
#define LPUART_BAUD_OSR_MASK    (((1U << LPUART_BAUD_OSR_WIDTH) - 1) << LPUART_BAUD_OSR_SHIFT)
// Esempio: ( (1U << 5) - 1 )      -> (32 - 1) = 31 (0b11111)
//          (0b11111 << 24)         -> 0x1F000000

// --- Campo SBR (Baud Rate Modulo Divisor) ---
// Bits 12-0 (13 bit totali)
#define LPUART_BAUD_SBR_SHIFT   0U  // Posizione del LSB del campo SBR
#define LPUART_BAUD_SBR_WIDTH   13U // Numero di bit occupati da SBR
#define LPUART_BAUD_SBR_MASK    (((1U << LPUART_BAUD_SBR_WIDTH) - 1) << LPUART_BAUD_SBR_SHIFT)
// Esempio: ( (1U << 13) - 1 )      -> (8192 - 1) = 8191 (0x1FFF)
//          (0x1FFF << 0)           -> 0x00001FFF
#define TYPE_NXPS32K358_LPUART "nxps32k358-lpuart"
OBJECT_DECLARE_SIMPLE_TYPE(NXPS32K358LPUARTState, NXPS32K358_LPUART)


struct NXPS32K358LPUARTState {
    SysBusDevice parent_obj;

    MemoryRegion iomem;

    CharBackend chr; // Per l'I/O seriale
    qemu_irq irq;     // Linea di interrupt

    uint32_t baud_rate_config; // Valore del registro BAUD
    uint32_t lpuart_cr;         // Valore del registro CTRL
    uint32_t lpuart_sr;         // Valore del registro STAT
    uint32_t lpuart_dr;
};

#endif // HW_CHAR_NXPS32K358_LPUART_H