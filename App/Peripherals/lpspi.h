#ifndef LPSPI_H
#define LPSPI_H

#include <stdint.h>
#include <stddef.h> // Per size_t

/* Indirizzi base LPSPI (Esempio, verificare con il manuale S32K3 e l'emulazione QEMU) */
#define LPSPI0_BASE     0x40290000
#define LPSPI1_BASE     0x40294000
// Aggiungi altri LPSPI base se necessario

/* Definizione della struttura dei registri LPSPI (semplificata) */
/* Fare riferimento al manuale S32K3 per la struttura completa */
typedef struct {
    volatile uint32_t VERID;      /* Version ID Register */
    volatile uint32_t PARAM;      /* Parameter Register */
    uint32_t RESERVED_0[2];
    volatile uint32_t CR;         /* Control Register */
    volatile uint32_t SR;         /* Status Register */
    volatile uint32_t IER;        /* Interrupt Enable Register */
    volatile uint32_t DER;        /* DMA Enable Register */
    volatile uint32_t CFGR0;      /* Configuration Register 0 */
    volatile uint32_t CFGR1;      /* Configuration Register 1 */
    uint32_t RESERVED_1[2];
    volatile uint32_t DMR0;       /* Data Match Register 0 */
    volatile uint32_t DMR1;       /* Data Match Register 1 */
    uint32_t RESERVED_2[2];
    volatile uint32_t CCR;        /* Clock Configuration Register */
    uint32_t RESERVED_3[5];
    volatile uint32_t FCR;        /* FIFO Control Register */
    volatile uint32_t FSR;        /* FIFO Status Register */
    volatile uint32_t TCR;        /* Transmit Command Register */
    volatile uint32_t TDR;        /* Transmit Data Register */
    uint32_t RESERVED_4[2];
    volatile const uint32_t RSR;  /* Receive Status Register */
    volatile const uint32_t RDR;  /* Receive Data Register */
} LPSPI_Type;

#define LPSPI0 ((LPSPI_Type *)LPSPI0_BASE)
#define LPSPI1 ((LPSPI_Type *)LPSPI1_BASE)
// ...

/* Bitfields per LPSPI_CR */
#define LPSPI_CR_MEN_MASK       (1u << 0)  // Module Enable
#define LPSPI_CR_RST_MASK       (1u << 1)  // Software Reset
#define LPSPI_CR_DBGEN_MASK     (1u << 3)  // Debug Enable

/* Bitfields per LPSPI_SR */
#define LPSPI_SR_TDF_MASK       (1u << 0)  // Transmit Data Flag
#define LPSPI_SR_RDF_MASK       (1u << 1)  // Receive Data Flag
#define LPSPI_SR_MBF_MASK       (1u << 24) // Module Busy Flag

/* Bitfields per LPSPI_CFGR1 */
#define LPSPI_CFGR1_MASTER_MASK (1u << 0)  // Master Mode Select

/* Bitfields per LPSPI_TCR */
#define LPSPI_TCR_FRAMESZ(x)    (((x)-1) & 0xFFF) // Frame Size
#define LPSPI_TCR_PCS(x)        (((x) & 0x3) << 24) // Peripheral Chip Select

typedef enum _lpspi_clock_phase {
    kLPSPI_ClockPhaseFirstEdge = 0U, /*!< Data is captured on the leading edge of SCK and changed on the trailing edge.*/
    kLPSPI_ClockPhaseSecondEdge = 1U /*!< Data is changed on the leading edge of SCK and captured on the trailing edge.*/
} lpspi_clock_phase_t;

typedef enum _lpspi_clock_polarity {
    kLPSPI_PolarityActiveHigh = 0U, /*!< Active-high SCK polarity (idles low). */
    kLPSPI_PolarityActiveLow  = 1U  /*!< Active-low SCK polarity (idles high). */
} lpspi_clock_polarity_t;

typedef struct _lpspi_master_config {
    uint32_t baudRate;
    lpspi_clock_phase_t cpha;
    lpspi_clock_polarity_t cpol;
    uint32_t bitsPerFrame;
    uint8_t pcsToUse; // Peripheral Chip Select (0-3)
    // Aggiungere altri campi di configurazione se necessario
} lpspi_master_config_t;

typedef struct _lpspi_transfer {
    uint8_t *txData;
    uint8_t *rxData;
    size_t dataSize;
    uint32_t flags; // Per opzioni di trasferimento aggiuntive
} lpspi_transfer_t;


void LPSPI_MasterInit(LPSPI_Type *base, const lpspi_master_config_t *config, uint32_t srcClock_Hz);
void LPSPI_MasterDeinit(LPSPI_Type *base);
uint8_t LPSPI_MasterTransferBlocking(LPSPI_Type *base, uint8_t send_byte);
void LPSPI_MasterTransfer(LPSPI_Type *base, lpspi_transfer_t *transfer);


#endif // LPSPI_H
