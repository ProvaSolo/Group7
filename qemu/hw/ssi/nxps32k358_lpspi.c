#include "qemu/osdep.h"
#include "qemu/log.h"
#include "qemu/module.h"
#include "hw/ssi/nxps32k358_lpspi.h"
#include "migration/vmstate.h"

#ifndef NXP_LPSPI_ERR_DEBUG
#define NXP_LPSPI_ERR_DEBUG 0
#endif

// REVISONE 1 -- 20 MAY
#define DB_PRINT_L(lvl, fmt, args...)               \
    do                                              \
    {                                               \
        if (NXP_LPSPI_ERR_DEBUG >= lvl)             \
        {                                           \
            qemu_log("%s: " fmt, __func__, ##args); \
        }                                           \
    } while (0)

#define DB_PRINT(fmt, args...) DB_PRINT_L(1, fmt, ##args)

// Modificare i reset, per reference a pagina 2862 del Reference Manual
static void nxps32k358_lpspi_reset(DeviceState *dev)
{
    NXPS32K358LPSPIState *s = NXPS32K358_LPSPI(dev);

    s->lpspi_verid = 0x02000004; // Version ID
    s->lpspi_param = 0x00080202; // Parameter Register, idem
    s->lpspi_cr = 0x00000000;
    s->lpspi_sr = 0x00000001; // Di solito TDF=1 dopo reset, controlla RM
    s->lpspi_ier = 0x00000000;
    s->lpspi_der = 0x00000000;
    s->lpspi_cfgr0 = 0x00000000;
    s->lpspi_cfgr1 = 0x00000000;
    // s->lpspi_dmr0 = 0x00000000; // Se usi DMR0/1, decommenta
    // s->lpspi_dmr1 = 0x00000000;
    s->lpspi_ccr = 0x00000000;
    s->lpspi_fcr = 0x00000000;
    s->lpspi_fsr = 0x00000003; // TXCOUNT=0, RXCOUNT=0, TFFF=1, RFFF=1 tipico reset
    s->lpspi_tcr = 0x0000001F;
    s->lpspi_tdr = 0x00000000;
    s->lpspi_rsr = 0x00000002; // RDF=1 tipico reset
    s->lpspi_rdr = 0x00000000;
}
static void nxps32k358_lpspi_transfer(NXPS32K358LPSPIState *s)
{
    DB_PRINT("Data to send: 0x%x\n", s->lpspi_tdr);

    // Perform the transfer
    s->lpspi_rdr = ssi_transfer(s->ssi, s->lpspi_tdr);

    // Update status registers
    s->lpspi_sr |= (1 << 1); // Set RDF (Receive Data Flag)
    s->lpspi_sr |= (1 << 0); // Set TDF (Transmit Data Flag)

    // Update FIFO status if needed
    /*
    s->lpspi_fsr &= ~(0xF << 16); // Clear RXCOUNT
    s->lpspi_fsr |= (1 << 16);    // Set RXCOUNT to 1
    s->lpspi_fsr |= (1 << 0);     // Set RFDF (Receive FIFO Drain Flag)
    */
    // If interrupts are enabled, trigger them
    /*
    if (s->lpspi_ier & (1 << 1))
    { // If RDF interrupt enabled
        qemu_irq_raise(s->irq);
    }
    */
    DB_PRINT("Data received: 0x%x\n", s->lpspi_rdr);
}

static uint64_t nxps32k358_lpspi_read(void *opaque, hwaddr addr, unsigned int size)
{
    NXPS32K358LPSPIState *s = opaque;

    DB_PRINT("Address: 0x%" HWADDR_PRIx "\n", addr);

    switch (addr)
    {
    case S32K_LPSPI_VERID:
        return s->lpspi_verid;
    case S32K_LPSPI_PARAM:
        return s->lpspi_param;
    case S32K_LPSPI_CR:
        return s->lpspi_cr;
    case S32K_LPSPI_SR:
        return s->lpspi_sr;
    case S32K_LPSPI_IER:
        return s->lpspi_ier;
    case S32K_LPSPI_DER:
        return s->lpspi_der;
    case S32K_LPSPI_CFGR0:
        return s->lpspi_cfgr0;
    case S32K_LPSPI_CFGR1:
        return s->lpspi_cfgr1;
    // case S32K_LPSPI_DMR0:
    //     return s->lpspi_dmr0;
    // case S32K_LPSPI_DMR1:
    //     return s->lpspi_dmr1;
    case S32K_LPSPI_CCR:
        return s->lpspi_ccr;
    case S32K_LPSPI_FCR:
        return s->lpspi_fcr;
    case S32K_LPSPI_FSR:
        return s->lpspi_fsr;
    case S32K_LPSPI_TCR:
        return s->lpspi_tcr;
    case S32K_LPSPI_TDR:
        return s->lpspi_tdr;
    case S32K_LPSPI_RSR:
        return s->lpspi_rsr;
    case S32K_LPSPI_RDR:
        return s->lpspi_rdr;
    default:
        qemu_log_mask(LOG_GUEST_ERROR, "%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__, addr);
    }

    return 0;
}

static void nxps32k358_lpspi_write(void *opaque, hwaddr addr, uint64_t val64, unsigned int size)
{
    NXPS32K358LPSPIState *s = opaque;
    uint32_t value = val64;

    DB_PRINT("Address: 0x%" HWADDR_PRIx ", Value: 0x%x\n", addr, value);

    switch (addr)
    {
    case S32K_LPSPI_VERID:
        qemu_log_mask(LOG_GUEST_ERROR, "%s: Read only register: 0x%" HWADDR_PRIx "\n", __func__, addr);
        return;
    case S32K_LPSPI_PARAM:
        qemu_log_mask(LOG_GUEST_ERROR, "%s: Read only register: 0x%" HWADDR_PRIx "\n", __func__, addr);
        return;
    case S32K_LPSPI_CR:
        s->lpspi_cr = value;
        return;
    case S32K_LPSPI_SR:
        qemu_log_mask(LOG_GUEST_ERROR, "%s: Read only register: 0x%" HWADDR_PRIx "\n", __func__, addr);
        return;
    case S32K_LPSPI_IER:
        s->lpspi_ier = value;
        return;
    case S32K_LPSPI_DER:
        s->lpspi_der = value;
        return;
    case S32K_LPSPI_CFGR0:
        s->lpspi_cfgr0 = value;
        return;
    case S32K_LPSPI_CFGR1:
        s->lpspi_cfgr1 = value;
        return;
    // case S32K_LPSPI_DMR0:
    //     s->lpspi_dmr0 = value;
    //     return;
    // case S32K_LPSPI_DMR1:
    //     s->lpspi_dmr1 = value;
    //     return;
    case S32K_LPSPI_CCR:
        s->lpspi_ccr = value;
        return;
    case S32K_LPSPI_FCR:
        s->lpspi_fcr = value;
        return;
    case S32K_LPSPI_FSR:
        qemu_log_mask(LOG_GUEST_ERROR, "%s: Read only register: 0x%" HWADDR_PRIx "\n", __func__, addr);
        return;
    case S32K_LPSPI_TCR:
        s->lpspi_tcr = value;
        return;
    case S32K_LPSPI_TDR:
        s->lpspi_tdr = value;
        // Optionally trigger transfer logic here
        return;
    case S32K_LPSPI_RSR:
        qemu_log_mask(LOG_GUEST_ERROR, "%s: Read only register: 0x%" HWADDR_PRIx "\n", __func__, addr);
        return;
    case S32K_LPSPI_RDR:
        qemu_log_mask(LOG_GUEST_ERROR, "%s: Read only register: 0x%" HWADDR_PRIx "\n", __func__, addr);
        return;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__, addr);
    }
}

static const MemoryRegionOps nxps32k358_lpspi_ops = {
    .read = nxps32k358_lpspi_read,
    .write = nxps32k358_lpspi_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static const VMStateDescription vmstate_nxps32k358_lpspi = {
    .name = TYPE_NXPS32K358_LPSPI,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (const VMStateField[]){

        // NXP S32K358 LPSPI registers
        VMSTATE_UINT32(lpspi_verid, NXPS32K358LPSPIState),
        VMSTATE_UINT32(lpspi_param, NXPS32K358LPSPIState),
        VMSTATE_UINT32(lpspi_cr, NXPS32K358LPSPIState),
        VMSTATE_UINT32(lpspi_sr, NXPS32K358LPSPIState),
        VMSTATE_UINT32(lpspi_ier, NXPS32K358LPSPIState),
        VMSTATE_UINT32(lpspi_der, NXPS32K358LPSPIState),
        VMSTATE_UINT32(lpspi_cfgr0, NXPS32K358LPSPIState),
        VMSTATE_UINT32(lpspi_cfgr1, NXPS32K358LPSPIState),
        // VMSTATE_UINT32(lpspi_dmr0, NXPS32K358LPSPIState), // Uncomment if used
        // VMSTATE_UINT32(lpspi_dmr1, NXPS32K358LPSPIState),
        VMSTATE_UINT32(lpspi_ccr, NXPS32K358LPSPIState),
        VMSTATE_UINT32(lpspi_fcr, NXPS32K358LPSPIState),
        VMSTATE_UINT32(lpspi_fsr, NXPS32K358LPSPIState),
        VMSTATE_UINT32(lpspi_tcr, NXPS32K358LPSPIState),
        VMSTATE_UINT32(lpspi_tdr, NXPS32K358LPSPIState),
        VMSTATE_UINT32(lpspi_rsr, NXPS32K358LPSPIState),
        VMSTATE_UINT32(lpspi_rdr, NXPS32K358LPSPIState),

        VMSTATE_END_OF_LIST()}};

static void nxps32k358_lpspi_init(Object *obj)
{
    NXPS32K358LPSPIState *s = NXPS32K358_LPSPI(obj);
    DeviceState *dev = DEVICE(obj);

    memory_region_init_io(&s->mmio, obj, &nxps32k358_lpspi_ops, s,
                          TYPE_NXPS32K358_LPSPI, 0x400);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);

    sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

    s->ssi = ssi_create_bus(dev, "ssi");
}

static void nxps32k358_lpspi_class_init(ObjectClass *klass, const void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    device_class_set_legacy_reset(dc, nxps32k358_lpspi_reset);
    dc->vmsd = &vmstate_nxps32k358_lpspi;
}

static const TypeInfo nxps32k358_lpspi_info = {
    .name = TYPE_NXPS32K358_LPSPI,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(NXPS32K358LPSPIState),
    .instance_init = nxps32k358_lpspi_init,
    .class_init = nxps32k358_lpspi_class_init,
};

static void nxps32k358_lpspi_register_types(void)
{
    type_register_static(&nxps32k358_lpspi_info);
}

type_init(nxps32k358_lpspi_register_types)
