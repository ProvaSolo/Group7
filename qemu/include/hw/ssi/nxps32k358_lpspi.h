#ifndef HW_NXPS32K358_LPSPI_H
#define HW_NXPS32K358_LPSPI_H

#include "hw/sysbus.h"
#include "hw/ssi/ssi.h"
#include "qom/object.h"

// REVISONE 1 -- 20 MAY

/*
 * NOTE: This header is based on the NXP S32K1xx Series Reference Manual.
 * Registers and bit definitions should be verified against the specific
 * NXP S32K358 Reference Manual for full accuracy, as peripheral
 * details can vary between S32K1xx and S32K3xx families.
 */

/* S32K1xx LPSPI Register Offsets (refer to S32K1xx RM, Chapter 51) */
#define S32K_LPSPI_VERID 0x00 // Version ID Register
#define S32K_LPSPI_PARAM 0x04 // Parameter Register
#define S32K_LPSPI_CR 0x10    // Control Register
#define S32K_LPSPI_SR 0x14    // Status Register
#define S32K_LPSPI_IER 0x18   // Interrupt Enable Register
#define S32K_LPSPI_DER 0x1C   // DMA Enable Register
#define S32K_LPSPI_CFGR0 0x20 // Configuration Register 0
#define S32K_LPSPI_CFGR1 0x24 // Configuration Register 1
#define S32K_LPSPI_DMR0 0x30  // Data Match Register 0
#define S32K_LPSPI_DMR1 0x34  // Data Match Register 1
#define S32K_LPSPI_CCR 0x40   // Clock Configuration Register
#define S32K_LPSPI_FCR 0x58   // FIFO Control Register
#define S32K_LPSPI_FSR 0x5C   // FIFO Status Register
#define S32K_LPSPI_TCR 0x60   // Transmit Command Register
#define S32K_LPSPI_TDR 0x64   // Transmit Data Register
#define S32K_LPSPI_RSR 0x70   // Receive Status Register
#define S32K_LPSPI_RDR 0x74   // Receive Data Register

/* LPSPI Control Register (LPSPI_CR) bits */
#define LPSPI_CR_MEN (1 << 0)   // Module Enable
#define LPSPI_CR_RST (1 << 1)   // Software Reset
#define LPSPI_CR_DBGEN (1 << 3) // Debug Enable

/* LPSPI Status Register (LPSPI_SR) bits */
#define LPSPI_SR_TDF (1 << 0)  // Transmit Data Flag
#define LPSPI_SR_RDF (1 << 1)  // Receive Data Flag
#define LPSPI_SR_WCF (1 << 8)  // Word Complete Flag
#define LPSPI_SR_FCF (1 << 9)  // Frame Complete Flag
#define LPSPI_SR_TCF (1 << 10) // Transfer Complete Flag
#define LPSPI_SR_TEF (1 << 11) // Transmit Error Flag
#define LPSPI_SR_REF (1 << 12) // Receive Error Flag
#define LPSPI_SR_DMF (1 << 13) // Data Match Flag
#define LPSPI_SR_MBF (1 << 24) // Module Busy Flag

/* LPSPI Configuration Register 1 (LPSPI_CFGR1) bits */
#define LPSPI_CFGR1_MASTER (1 << 0)  // Master Mode
#define LPSPI_CFGR1_SAMPLE (1 << 1)  // Sample Point
#define LPSPI_CFGR1_AUTOPCS (1 << 2) // Automatic PCS
#define LPSPI_CFGR1_NOSTALL (1 << 3) // No Stall

/*
 * CRC and I2S functionality are typically handled by separate peripherals
 * (e.g., SAI for I2S, and a dedicated CRC module) on NXP S32K MCUs,
 * not directly within the LPSPI module. The original STM32-style CRC/I2S
 * register defines are therefore omitted here.
 */

#define TYPE_NXPS32K358_LPSPI "nxps32k358-lpspi" // Changed from -spi to -lpspi for clarity
OBJECT_DECLARE_SIMPLE_TYPE(NXPS32K358LPSPIState, NXPS32K358_LPSPI)

struct NXPS32K358LPSPIState
{
    /* <private> */
    SysBusDevice parent_obj;

    /* <public> */
    MemoryRegion mmio;

    /* LPSPI Registers based on S32K1xx RM */
    uint32_t lpspi_verid;
    uint32_t lpspi_param;
    uint32_t lpspi_cr;
    uint32_t lpspi_sr;
    uint32_t lpspi_ier;
    uint32_t lpspi_der;
    uint32_t lpspi_cfgr0;
    uint32_t lpspi_cfgr1;
    // DMR0 and DMR1 are less commonly used in basic QEMU models,
    // but can be added if match functionality is needed.
    // uint32_t lpspi_dmr0;
    // uint32_t lpspi_dmr1;
    uint32_t lpspi_ccr;
    uint32_t lpspi_fcr;
    uint32_t lpspi_fsr;
    uint32_t lpspi_tcr;
    uint32_t lpspi_tdr; // Transmit Data Register
    uint32_t lpspi_rsr;
    uint32_t lpspi_rdr; // Receive Data Register
                        // The original single spi_dr can be conceptually mapped
                        // by the C model to read from RDR and write to TDR.

    qemu_irq irq;
    SSIBus *ssi;
};

#endif /* HW_NXPS32K358_LPSPI_H */