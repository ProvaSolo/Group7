#include "qemu/osdep.h"
#include "hw/sysbus.h"
#include "hw/ssi/ssi.h"
#include "qemu/log.h"
#include "qemu/module.h"
#include "hw/registerfields.h"
#include "hw/qdev-properties.h"
#include "hw/irq.h"
#include "qapi/error.h"
#include "hw/ssi/nxps32k358_lpspi.h" // Include the correct device header

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

// Stubs for missing functions to avoid linker errors
static void nxps32k358_lpspi_switch_to_controller(NXPS32K358LpspiState *s) { /* ... */ }
static void nxps32k358_lpspi_switch_to_peripheral(NXPS32K358LpspiState *s) { /* ... */ }

// Register offset for DMR0 (Data Match Register 0)
#define LPSPI_DMR0 0x30

// Device state definition
typedef struct NXPS32K358LpspiState
{
	/* ... other device state fields ... */
	uint32_t dmr0_reg; // DMR0 register: holds MATCH0 value for comparison
	uint32_t cfgr1_reg;
	int instance_id;
	/* ... other device state fields ... */
} NXPS32K358LpspiState;

// Write handler for CFGR1 register
static void nxps32k358_lpspi_write_cfgr1(NXPS32K358LpspiState *s, uint32_t value)
{
	uint32_t new_cfgr1 = s->cfgr1_reg;

	// ------ Handle instance-dependent fields ------
	// PCSPOL: apply mask based on instance
	uint32_t pcspol_mask;
	switch (s->instance_id)
	{
	case LPSPI_INSTANCE_0:
		pcspol_mask = 0xFF; // Bits 15-8
		break;
	case LPSPI_INSTANCE_1:
		pcspol_mask = 0x3F; // Bits 13-8
		break;
	case LPSPI_INSTANCE_2:
	case LPSPI_INSTANCE_3:
	case LPSPI_INSTANCE_4:
	case LPSPI_INSTANCE_5:
		pcspol_mask = 0x0F; // Bits 11-8
		break;
	default:
		pcspol_mask = 0x00;
		break;
	}
	new_cfgr1 = (new_cfgr1 & ~(LPSPI_CFGR1_PCSPOL_MASK & (pcspol_mask << LPSPI_CFGR1_PCSPOL_SHIFT))) |
				(value & (LPSPI_CFGR1_PCSPOL_MASK & (pcspol_mask << LPSPI_CFGR1_PCSPOL_SHIFT)));

	// ------ MATCFG: only allow transitions to/from 0 ------
	uint32_t old_matcfg = (new_cfgr1 & LPSPI_CFGR1_MATCFG_MASK);
	uint32_t new_matcfg = (value & LPSPI_CFGR1_MATCFG_MASK);
	if (old_matcfg != 0 && new_matcfg != 0)
	{
		qemu_log_mask(LOG_GUEST_ERROR, "LPSPI: Invalid MATCFG transition 0x%x → 0x%x\n",
					  old_matcfg, new_matcfg);
		new_matcfg = 0; // Fallback to "Match disabled"
	}
	new_cfgr1 = (new_cfgr1 & ~LPSPI_CFGR1_MATCFG_MASK) | new_matcfg;

	// ------ Update other fields ------
	// Simple fields (MASTER, SAMPLE, etc.)
	new_cfgr1 = (new_cfgr1 & ~(LPSPI_CFGR1_MASTER | LPSPI_CFGR1_SAMPLE | LPSPI_CFGR1_AUTOPCS |
							   LPSPI_CFGR1_NOSTALL | LPSPI_CFGR1_PARTIAL | LPSPI_CFGR1_OUTCFG |
							   LPSPI_CFGR1_PINCFG_MASK)) |
				(value & (LPSPI_CFGR1_MASTER | LPSPI_CFGR1_SAMPLE | LPSPI_CFGR1_AUTOPCS |
						  LPSPI_CFGR1_NOSTALL | LPSPI_CFGR1_PARTIAL | LPSPI_CFGR1_OUTCFG |
						  LPSPI_CFGR1_PINCFG_MASK));

	// Apply mask for reserved bits
	new_cfgr1 &= ~LPSPI_CFGR1_RESERVED_MASK;
	s->cfgr1_reg = new_cfgr1;

	// Additional logic for dynamic changes
	if (value & LPSPI_CFGR1_MASTER)
	{
		// Switch to Controller mode: reset PCS/SCK logic
		nxps32k358_lpspi_switch_to_controller(s);
	}
	else
	{
		// Switch to Peripheral mode
		nxps32k358_lpspi_switch_to_peripheral(s);
	}
}

static uint64_t nxps32k358_lpspi_read(void *opaque, hwaddr offset, unsigned size)
{
	NXPS32K358LpspiState *s = NXPS32K358_LPSPI(opaque);
	switch (offset)
	{
	// ...
	case LPSPI_DMR0:
		return s->dmr0_reg; // Restituisce il valore di MATCH0
	case LPSPI_DMR1:
		return s->dmr1_reg; // Restituisce il valore di MATCH1
							// ...
	}
}

static void nxps32k358_lpspi_write(void *opaque, hwaddr offset, uint64_t value, unsigned size)
{
	NXPS32K358LpspiState *s = NXPS32K358_LPSPI(opaque);
	switch (offset)
	{
	// ...
	case LPSPI_DMR0:
		s->dmr0_reg = value; // Imposta il valore di MATCH0
		break;
	case LPSPI_DMR1:
		s->dmr1_reg = value; // Imposta il valore di MATCH1
		break;
		// ...
	}
}

static void update_ccr1_from_ccr(NXPS32K358LpspiState *s)
{
	// Aggiorna CCR1 in base a CCR[DBT] e CCR[SCKDIV]
	uint8_t dbt = (s->ccr_reg & LPSPI_CCR_DBT_MASK) >> 8;
	uint8_t sckdiv = (s->ccr_reg & LPSPI_CCR_SCKDIV_MASK) >> 0;

	// CCR1[PCSPCS] = (DBT >> 4) + 1 (esempio)
	// CCR1[SCKSCK] = (DBT & 0x0F) + 1
	s->ccr1_reg = ((dbt + 2) << 16) | ((dbt + 1) << 24);

	// CCR1[SCKSET] = (sckdiv >> 4) + 1
	// CCR1[SCKHLD] = (sckdiv & 0x0F) + 1
	s->ccr1_reg |= ((sckdiv + 2) << 0) | ((sckdiv + 1) << 8);
}

// Fix: add offset parameter to match usage in function
static void nxps32k358_lpspi_write_ccr(NXPS32K358LpspiState *s, hwaddr offset, uint64_t value, unsigned size)
{
	if (!(s->cr_reg & LPSPI_CR_MEN))
	{
		uint32_t mask = 0;
		switch (size)
		{
		case 4:
			mask = 0xFFFFFFFF;
			s->ccr1_reg = 0;
			break;
		case 2:
			mask = LPSPI_CCR_SCKPCS_MASK | LPSPI_CCR_PCSSCK_MASK;
			break;
		case 1:
			if (offset == LPSPI_CCR + 1)
			{
				mask = LPSPI_CCR_DBT_MASK;
			}
			else
			{
				mask = LPSPI_CCR_SCKDIV_MASK;
			}
			break;
		}
		s->ccr_reg = (s->ccr_reg & ~mask) | (value & mask);
		update_ccr1_from_ccr(s);
	}
}

static uint64_t nxps32k358_lpspi_read_ccr(NXPS32K358LpspiState *s)
{
	// DBT e SCKDIV leggono sempre 0 se CCR1 è stato scritto
	return s->ccr_reg & ~(LPSPI_CCR_DBT_MASK | LPSPI_CCR_SCKDIV_MASK);
}

static void nxps32k358_lpspi_reset(DeviceState *dev)
{
	NXPS32K358LpspiState *s = NXPS32K358_LPSPI(dev);
	s->ccr_reg = 0;
	s->ccr1_reg = 0;
}

static void update_fifo_flags(NXPS32K358LpspiState *s)
{
	uint8_t rxwater = MIN((s->fcr_reg & LPSPI_FCR_RXWATER_MASK) >> LPSPI_FCR_RXWATER_SHIFT, LPSPI_FIFO_SIZE);
	uint8_t txwater = MIN((s->fcr_reg & LPSPI_FCR_TXWATER_MASK) >> LPSPI_FCR_TXWATER_SHIFT, LPSPI_FIFO_SIZE);

	if (s->rx_fifo_len > rxwater)
		s->sr_reg |= LPSPI_SR_RDF;
	else
		s->sr_reg &= ~LPSPI_SR_RDF;

	if (s->tx_fifo_len <= txwater)
		s->sr_reg |= LPSPI_SR_TDF;
	else
		s->sr_reg &= ~LPSPI_SR_TDF;
}

static void nxps32k358_lpspi_write_fcr(NXPS32K358LpspiState *s, uint32_t value)
{
	s->fcr_reg = (s->fcr_reg & LPSPI_FCR_RESERVED_MASK) |
				 (value & (LPSPI_FCR_RXWATER_MASK | LPSPI_FCR_TXWATER_MASK));
	update_fifo_flags(s);
}

static uint32_t nxps32k358_lpspi_read_fcr(NXPS32K358LpspiState *s)
{
	return s->fcr_reg;
}

static void nxps32k358_lpspi_reset(DeviceState *dev)
{
	NXPS32K358LpspiState *s = NXPS32K358_LPSPI(dev);
	s->fcr_reg = 0; // Valori di default: RXWATER=0, TXWATER=0
}