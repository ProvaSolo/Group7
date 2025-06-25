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
#define LPSPI_SR_MBF (1U << 24)

// Receive Status Register (RSR) bitmasks
#define LPSPI_RSR_RXEMPTY (1U << 1)

// CFGR0 Register bits
#define CFGR0_HREN (1U << 0)
#define CFGR0_HRPOL (1U << 1)
#define CFGR0_HRSEL (1U << 2)
#define CFGR0_CIRFIFO (1U << 8)
#define CFGR0_RDMO (1U << 9)

// CFGR1 Register bits
#define CFGR1_MASTER (1U << 0)
#define CFGR1_SAMPLE (1U << 1)
#define CFGR1_AUTOPCS (1U << 2)
#define CFGR1_NOSTALL (1U << 3)
#define CFGR1_PCSPOL_SHIFT 8
#define CFGR1_PCSPOL_MASK (0xF << CFGR1_PCSPOL_SHIFT)
#define CFGR1_MATCFG_SHIFT 16
#define CFGR1_MATCFG_MASK (0x7 << CFGR1_MATCFG_SHIFT)
#define CFGR1_PINCFG_SHIFT 24
#define CFGR1_PINCFG_MASK (0x3 << CFGR1_PINCFG_SHIFT)

// Clock Configuration Register (CCR) fields
#define CCR_SCKDIV_SHIFT 0
#define CCR_SCKDIV_MASK (0xFF << CCR_SCKDIV_SHIFT)
#define CCR_DBT_SHIFT 8
#define CCR_DBT_MASK (0xFF << CCR_DBT_SHIFT)
#define CCR_PCSSCK_SHIFT 16
#define CCR_PCSSCK_MASK (0xFF << CCR_PCSSCK_SHIFT)
#define CCR_SCKPCS_SHIFT 24
#define CCR_SCKPCS_MASK (0xFF << CCR_SCKPCS_SHIFT)
// Transmit Command Register (TCR) bits and masks
#define TCR_FRAMESZ_SHIFT 0
#define TCR_FRAMESZ_MASK (0xFFF << TCR_FRAMESZ_SHIFT)
#define TCR_WIDTH_SHIFT 12
#define TCR_WIDTH_MASK (0x3 << TCR_WIDTH_SHIFT)
#define TCR_TXMSK (1U << 18)
#define TCR_RXMSK (1U << 19)
#define TCR_CONTC (1U << 20)
#define TCR_CONT (1U << 21)
#define TCR_BYSW (1U << 22)
#define TCR_LSBF (1U << 23)
#define TCR_PCS_SHIFT 24
#define TCR_PCS_MASK (0x3 << TCR_PCS_SHIFT)
#define TCR_PRESCALE_SHIFT 27
#define TCR_PRESCALE_MASK (0x7 << TCR_PRESCALE_SHIFT)
#define TCR_CPHA (1U << 30)
#define TCR_CPOL (1U << 31)

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

    bool cs_active;
    uint8_t spi_mode;
};

#endif // HW_NXP_S32K358_LPSPI_H
