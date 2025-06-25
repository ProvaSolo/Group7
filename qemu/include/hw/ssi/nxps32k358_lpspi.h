#ifndef HW_NXP_S32K358_LPSPI_H
#define HW_NXP_S32K358_LPSPI_H

#include "hw/sysbus.h"
#include "qemu/fifo8.h"
#include "qom/object.h"
#include "hw/ssi/ssi.h"

#define TYPE_NXPS32K358_LPSPI "nxps32k358-lpspi"
OBJECT_DECLARE_SIMPLE_TYPE(NXPS32K358LPSPIState, NXPS32K358_LPSPI)

// Register offsets for the LPSPI (Low Power Serial Peripheral Interface)
#define S32K_LPSPI_VERID 0x00
#define S32K_LPSPI_PARAM 0x04
#define S32K_LPSPI_CR 0x10
#define S32K_LPSPI_SR 0x14
#define S32K_LPSPI_IER 0x18
#define S32K_LPSPI_DER 0x1C
#define S32K_LPSPI_CFGR0 0x20
#define S32K_LPSPI_CFGR1 0x24
#define S32K_LPSPI_CCR 0x40
#define S32K_LPSPI_FCR 0x58
#define S32K_LPSPI_FSR 0x5C
#define S32K_LPSPI_TCR 0x60
#define S32K_LPSPI_TDR 0x64
#define S32K_LPSPI_RSR 0x70
#define S32K_LPSPI_RDR 0x74
#define S32K_LPSPI_REG_MAX_OFFSET 0x80

// Bitmask definitions for the LPSPI registers
#define LPSPI_CR_MEN (1U << 0)
#define LPSPI_CR_RST (1U << 1)
#define LPSPI_CR_RSTF (1U << 9)

// Status Register (SR) bitmasks
#define LPSPI_SR_TDF (1U << 0)
#define LPSPI_SR_RDF (1U << 1)
// Word, frame and transfer completion flags
#define LPSPI_SR_WCF (1U << 8)
#define LPSPI_SR_FCF (1U << 9)
#define LPSPI_SR_TCF (1U << 10)
// Error and data match flags
#define LPSPI_SR_TEF (1U << 11)
#define LPSPI_SR_REF (1U << 12)
#define LPSPI_SR_DMF (1U << 13)
// Module busy flag
#define LPSPI_SR_MBF (1U << 24)

// Interrupt Enable Register (IER) bitmasks
#define LPSPI_IER_TDIE (1U << 0)
#define LPSPI_IER_RDIE (1U << 1)
#define LPSPI_IER_WCIE (1U << 8)
#define LPSPI_IER_FCIE (1U << 9)
#define LPSPI_IER_TCIE (1U << 10)
#define LPSPI_IER_TEIE (1U << 11)
#define LPSPI_IER_REIE (1U << 12)
#define LPSPI_IER_DMIE (1U << 13)

// DMA Enable Register (DER) bitmasks
#define LPSPI_DER_TDDE (1U << 0)
#define LPSPI_DER_RDDE (1U << 1)

// Receive Status Register (RSR) bitmasks
#define LPSPI_RSR_RXEMPTY (1U << 1)

// Transmit Command Register (TCR) bits and masks
#define TCR_PCS_SHIFT 24
#define TCR_PCS_MASK (0x3 << TCR_PCS_SHIFT)
/* Continuous transfer bit in the Transmit Command Register */
#define TCR_CONT (1U << 23)
/* Masked transmit/receive bits in the Transmit Command Register */
#define TCR_TXMSK (1U << 18)
#define TCR_RXMSK (1U << 19)

/* FIFO Control Register field masks */
#define FCR_TXWATER_SHIFT 0
#define FCR_TXWATER_MASK  (0x3 << FCR_TXWATER_SHIFT)
#define FCR_RXWATER_SHIFT 16
#define FCR_RXWATER_MASK  (0x3 << FCR_RXWATER_SHIFT)

// FIFO depth and capacity definitions
#define LPSPI_FIFO_WORD_DEPTH 4
#define LPSPI_FIFO_BYTE_CAPACITY (LPSPI_FIFO_WORD_DEPTH * 4)

struct NXPS32K358LPSPIState
{
    SysBusDevice parent_obj;

    MemoryRegion mmio;
    SSIBus *ssi;
    qemu_irq irq;

    uint8_t num_cs_lines;
    qemu_irq *cs_lines;

    // FIFO software
    Fifo8 tx_fifo;
    Fifo8 rx_fifo;

    uint32_t lpspi_verid;
    uint32_t lpspi_param;
    uint32_t lpspi_cr;
    uint32_t lpspi_sr;
    uint32_t lpspi_ier;
    uint32_t lpspi_der;
    uint32_t lpspi_cfgr0;
    uint32_t lpspi_cfgr1;
    uint32_t lpspi_ccr;
    uint32_t lpspi_fcr;
    uint32_t lpspi_fsr;
    uint32_t lpspi_tcr;
    uint32_t lpspi_tdr;
    uint32_t lpspi_rsr;
    uint32_t lpspi_rdr;
};

#endif // HW_NXP_S32K358_LPSPI_H
