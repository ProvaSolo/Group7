#ifndef HW_SSI_NXPS32K358_LPSPI_H
#define HW_SSI_NXPS32K358_LPSPI_H

#include "hw/sysbus.h"
#include "hw/ssi/ssi.h" // Per SSIBus
#include "qom/object.h"

#define TYPE_NXPS32K358_LPSPI "nxps32k358-lpspi"
OBJECT_DECLARE_SIMPLE_TYPE(NXPS32K358LpspiState, NXPS32K358_LPSPI)

// Definizione dei registri LPSPI (offset dal base address della LPSPI)
// DOVRAI VERIFICARE QUESTI OFFSET E NOMI SUL S32K3XXRM.PDF!
#define LPSPI_VERID 0x00 // Version ID Register
#define LPSPI_PARAM 0x04 // Parameter Register
#define LPSPI_CR 0x10	 // Control Register
#define LPSPI_SR 0x14	 // Status Register
#define LPSPI_IER 0x18	 // Interrupt Enable Register
#define LPSPI_DER 0x1C	 // DMA Enable Register
#define LPSPI_CFGR0 0x20 // Configuration Register 0
#define LPSPI_CFGR1 0x24 // Configuration Register 1
#define LPSPI_DMR0 0x30	 // Data Match Register 0 (Esempio)
#define LPSPI_DMR1 0x34	 // Data Match Register 1 (Esempio)
#define LPSPI_CCR 0x40	 // Clock Configuration Register
#define LPSPI_CCR1 0x44	 // Clock Configuration Register (Esempio)
#define LPSPI_FCR 0x58	 // FIFO Control Register
#define LPSPI_FSR 0x5C	 // FIFO Status Register
#define LPSPI_TCR 0x60	 // Transmit Command Register
#define LPSPI_TDR 0x64	 // Transmit Data Register
#define LPSPI_RSR 0x70	 // Receive Status Register
#define LPSPI_RDR 0x74	 // Receive Data Register
#define LPSPI_RDROR 0x78 // Receive Data Overrun Register
#define LPSPI_TCBR 0x3FCh // Transmit Command Burst Register

// Bit dei registri (ESEMPI - verifica sul manuale!)
// Bit dello Status Register (SR)
#define LPSPI_SR_TDF    (1 << 0)   // TX Data Ready (bit 0)
#define LPSPI_SR_RDF    (1 << 1)   // RX Data Ready (bit 1)
#define LPSPI_SR_WCF    (1 << 8)   // Word Complete (bit 8)
#define LPSPI_SR_FCF    (1 << 9)   // Frame Complete (bit 9)
#define LPSPI_SR_TCF    (1 << 10)  // Transfer Complete (bit 10)
#define LPSPI_SR_TEF    (1 << 11)  // TX Error (bit 11)
#define LPSPI_SR_REF    (1 << 12)  // RX Error (bit 12)
#define LPSPI_SR_DMF    (1 << 13)  // Data Match (bit 13)
#define LPSPI_SR_MBF    (1 << 24)  // Module Busy (bit 24)

// Maschera per flag W1C (scrivi 1 per azzerare)
#define LPSPI_SR_W1C_MASK (LPSPI_SR_TCF | LPSPI_SR_TEF | LPSPI_SR_REF | LPSPI_SR_DMF | LPSPI_SR_FCF | LPSPI_SR_WCF) // Mask for W1C bits that can be cleared by writing 1

// CR Register bits
#define LPSPI_CR_MEN (1 << 0)	// Module Enable
#define LPSPI_CR_DBGEN (1 << 3) // Debug Enable

// IER Register bits
#define LPSPI_IER_TDIE   (1 << 0)   // Transmit Data Interrupt Enable (bit 0)
#define LPSPI_IER_RDIE   (1 << 1)   // Receive Data Interrupt Enable (bit 1)
#define LPSPI_IER_WCIE   (1 << 8)   // Word Complete Interrupt Enable (bit 8)
#define LPSPI_IER_FCIE   (1 << 9)   // Frame Complete Interrupt Enable (bit 9)
#define LPSPI_IER_TCIE   (1 << 10)  // Transfer Complete Interrupt Enable (bit 10)
#define LPSPI_IER_TEIE   (1 << 11)  // Transmit Error Interrupt Enable (bit 11)
#define LPSPI_IER_REIE   (1 << 12)  // Receive Error Interrupt Enable (bit 12)
#define LPSPI_IER_DMIE   (1 << 13)  // Data Match Interrupt Enable (bit 13)

// DER Register bits
#define LPSPI_DER_TODE   (1 << 0)   // Transmit Data DMA Enable (bit 0)
#define LPSPI_DER_RODE   (1 << 1)   // Receive Data DMA Enable (bit 1)

// CFGR0 Register bits
#define LPSPI_CFGR0_HREN    (1 << 0)   // Host Request Enable (bit 0)
#define LPSPI_CFGR0_HRPOL   (1 << 1)   // Host Request Polarity (bit 1)
#define LPSPI_CFGR0_HRSEL   (1 << 2)   // Host Request Select (bit 2)
#define LPSPI_CFGR0_HRDIR   (1 << 3)   // Host Request Direction (bit 3)
#define LPSPI_CFGR0_CIRFIFO (1 << 8)   // Circular FIFO Enable (bit 8)
#define LPSPI_CFGR0_RDMO    (1 << 9)   // Receive Data Match Only (bit 9)

// CFGR1 Register bits
#define LPSPI_CFGR1_MATCFG_SHIFT    8   // Verifica offset dal manuale!
#define LPSPI_CFGR1_MATCFG_MASK     (0x7 << LPSPI_CFGR1_MATCFG_SHIFT)

	// Match Conditions
#define LPSPI_MATCFG_FIRST_WORD_OR           (0x0 << LPSPI_CFGR1_MATCFG_SHIFT)
#define LPSPI_MATCFG_ANY_WORD_OR             (0x1 << LPSPI_CFGR1_MATCFG_SHIFT)
#define LPSPI_MATCFG_SEQ_MATCH_FIRST         (0x2 << LPSPI_CFGR1_MATCFG_SHIFT)
#define LPSPI_MATCFG_SEQ_MATCH_ANY           (0x3 << LPSPI_CFGR1_MATCFG_SHIFT)
#define LPSPI_MATCFG_FIRST_WORD_MASKED_AND   (0x4 << LPSPI_CFGR1_MATCFG_SHIFT)
#define LPSPI_MATCFG_ANY_WORD_MASKED_AND     (0x5 << LPSPI_CFGR1_MATCFG_SHIFT)




struct NXPS32K358LpspiState
{
	SysBusDevice parent_obj;

	MemoryRegion iomem;
	qemu_irq irq;
	SSIBus *ssi_bus; // Il bus SPI a cui questo controller è connesso

	// Registri principali (aggiungi gli altri)
	uint32_t cr_reg;  // Control Register
	uint32_t sr_reg;  // Status Register
	uint32_t ier_reg; // Interrupt Enable Register
	uint32_t ccr_reg; // Clock Configuration Register
	uint32_t tcr_reg; // Transmit Command Register
	uint32_t verid_reg;   // Version ID Register
	uint32_t param_reg;   // Parameter Register
	uint32_t der_reg;     // DMA Enable Register
	uint32_t cfgr0_reg;   // Configuration Register 0
	uint32_t cfgr1_reg;   // Configuration Register 1
	uint32_t dmr0_reg;    // Data Match Register 0
	uint32_t dmr1_reg;    // Data Match Register 1
	uint32_t ccr1_reg;    // Clock Configuration Register 1
	uint32_t fcr_reg;     // FIFO Control Register
	uint32_t fsr_reg;     // FIFO Status Register
	uint32_t rsr_reg;     // Receive Status Register
	uint32_t rdr_reg;     // Receive Data Register
	uint32_t rdror_reg;   // Receive Data Overrun Register
	uint32_t tcbr_reg;    // Transmit Command Burst Register


	uint32_t tx_fifo[4]; // Esempio FIFO di trasmissione (dimensione da verificare)
	uint32_t rx_fifo[4]; // Esempio FIFO di ricezione
	unsigned int tx_fifo_len;
	unsigned int rx_fifo_len;
	unsigned int tx_fifo_idx_w; // Write index
	unsigned int tx_fifo_idx_r; // Read index
	unsigned int rx_fifo_idx_w;
	unsigned int rx_fifo_idx_r;

	// Stato interno per i trasferimenti
	// Altri registri e stati necessari per il tuo controller LPSPI
	// ...

};

#endif // HW_SSI_NXPS32K358_LPSPI_H