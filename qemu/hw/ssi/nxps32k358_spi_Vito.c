#include "qemu/osdep.h"
#include "qemu/log.h"
#include "qemu/module.h"
#include "hw/ssi/nxps32k358_spi.h"
#include "migration/vmstate.h"

#ifndef NXP_SPI_ERR_DEBUG
#define NXP_SPI_ERR_DEBUG 0
#endif

// REVISONE 1 -- 20 MAY
#define DB_PRINT_L(lvl, fmt, args...) do { \
    if (NXP_SPI_ERR_DEBUG >= lvl) { \
        qemu_log("%s: " fmt, __func__, ## args); \
    } \
} while (0)

#define DB_PRINT(fmt, args...) DB_PRINT_L(1, fmt, ## args)

static void nxps32k358_spi_reset(DeviceState *dev)
{
    NXPS32K358SPIState *s = nxps32k358_SPI(dev);

    s->spi_cr1 = 0x00000000;
    s->spi_cr2 = 0x00000000;
    s->spi_sr = 0x0000000A;
    s->spi_dr = 0x0000000C;
    s->spi_crcpr = 0x00000007;
    s->spi_rxcrcr = 0x00000000;
    s->spi_txcrcr = 0x00000000;
    s->spi_i2scfgr = 0x00000000;
    s->spi_i2spr = 0x00000002;
}
static void nxps32k358_spi_transfer(nxps32k358SPIState *s)
{
    DB_PRINT("Data to send: 0x%x\n", s->spi_dr);

    s->spi_dr = ssi_transfer(s->ssi, s->spi_dr);
    s->spi_sr |= STM_SPI_SR_RXNE;

    DB_PRINT("Data received: 0x%x\n", s->spi_dr);
}

static uint64_t nxps32k358_spi_read(void *opaque, hwaddr addr,
                                     unsigned int size)
{
    NXPS32K358SPIState *s = opaque;

    DB_PRINT("Address: 0x%" HWADDR_PRIx "\n", addr);

    switch (addr) {
    case STM_SPI_CR1:
        return s->spi_cr1;
    case STM_SPI_CR2:
        qemu_log_mask(LOG_UNIMP, "%s: Interrupts and DMA are not implemented\n",
                      __func__);
        return s->spi_cr2;
    case STM_SPI_SR:
        return s->spi_sr;
    case STM_SPI_DR:
        nxps32k358_spi_transfer(s);
        s->spi_sr &= ~STM_SPI_SR_RXNE;
        return s->spi_dr;
    case STM_SPI_CRCPR:
        qemu_log_mask(LOG_UNIMP, "%s: CRC is not implemented, the registers " \
                      "are included for compatibility\n", __func__);
        return s->spi_crcpr;
    case STM_SPI_RXCRCR:
        qemu_log_mask(LOG_UNIMP, "%s: CRC is not implemented, the registers " \
                      "are included for compatibility\n", __func__);
        return s->spi_rxcrcr;
    case STM_SPI_TXCRCR:
        qemu_log_mask(LOG_UNIMP, "%s: CRC is not implemented, the registers " \
                      "are included for compatibility\n", __func__);
        return s->spi_txcrcr;
    case STM_SPI_I2SCFGR:
        qemu_log_mask(LOG_UNIMP, "%s: I2S is not implemented, the registers " \
                      "are included for compatibility\n", __func__);
        return s->spi_i2scfgr;
    case STM_SPI_I2SPR:
        qemu_log_mask(LOG_UNIMP, "%s: I2S is not implemented, the registers " \
                      "are included for compatibility\n", __func__);
        return s->spi_i2spr;
    default:
        qemu_log_mask(LOG_GUEST_ERROR, "%s: Bad offset 0x%" HWADDR_PRIx "\n",
                      __func__, addr);
    }

    return 0;
}

static void nxps32k358_spi_write(void *opaque, hwaddr addr,
                                uint64_t val64, unsigned int size)
{
    NXPS32K358SPIState *s = opaque;
    uint32_t value = val64;

    DB_PRINT("Address: 0x%" HWADDR_PRIx ", Value: 0x%x\n", addr, value);

    switch (addr) {
    case STM_SPI_CR1:
        s->spi_cr1 = value;
        return;
    case STM_SPI_CR2:
        qemu_log_mask(LOG_UNIMP, "%s: " \
                      "Interrupts and DMA are not implemented\n", __func__);
        s->spi_cr2 = value;
        return;
    case STM_SPI_SR:
        /* Read only register, except for clearing the CRCERR bit, which
         * is not supported
         */
        return;
    case STM_SPI_DR:
        s->spi_dr = value;
        nxps32k358_spi_transfer(s);
        return;
    case STM_SPI_CRCPR:
        qemu_log_mask(LOG_UNIMP, "%s: CRC is not implemented\n", __func__);
        return;
    case STM_SPI_RXCRCR:
        qemu_log_mask(LOG_GUEST_ERROR, "%s: Read only register: " \
                      "0x%" HWADDR_PRIx "\n", __func__, addr);
        return;
    case STM_SPI_TXCRCR:
        qemu_log_mask(LOG_GUEST_ERROR, "%s: Read only register: " \
                      "0x%" HWADDR_PRIx "\n", __func__, addr);
        return;
    case STM_SPI_I2SCFGR:
        qemu_log_mask(LOG_UNIMP, "%s: " \
                      "I2S is not implemented\n", __func__);
        return;
    case STM_SPI_I2SPR:
        qemu_log_mask(LOG_UNIMP, "%s: " \
                      "I2S is not implemented\n", __func__);
        return;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__, addr);
    }
}

static const MemoryRegionOps nxps32k358_spi_ops = {
    .read = nxps32k358_spi_read,
    .write = nxps32k358_spi_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static const VMStateDescription vmstate_nxps32k358_spi = {
    .name = TYPE_NXPS32K358_SPI,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (const VMStateField[]) {
        VMSTATE_UINT32(spi_cr1, NXPS32K358SPIState),
        VMSTATE_UINT32(spi_cr2, NXPS32K358SPIState),
        VMSTATE_UINT32(spi_sr, NXPS32K358SPIState),
        VMSTATE_UINT32(spi_dr, NXPS32K358SPIState),
        VMSTATE_UINT32(spi_crcpr, NXPS32K358SPIState),
        VMSTATE_UINT32(spi_rxcrcr, NXPS32K358SPIState),
        VMSTATE_UINT32(spi_txcrcr, NXPS32K358SPIState),
        VMSTATE_UINT32(spi_i2scfgr, NXPS32K358SPIState),
        VMSTATE_UINT32(spi_i2spr, NXPS32K358SPIState),
        VMSTATE_END_OF_LIST()
    }
};

static void nxps32k358_spi_init(Object *obj)
{
    NXPS32K358SPIState *s = nxps32k358_SPI(obj);
    DeviceState *dev = DEVICE(obj);

    memory_region_init_io(&s->mmio, obj, &nxps32k358_spi_ops, s,
                          TYPE_NXPS32K358_SPI, 0x400);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);

    sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

    s->ssi = ssi_create_bus(dev, "ssi");
}

static void nxps32k358_spi_class_init(ObjectClass *klass, const void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    device_class_set_legacy_reset(dc, nxps32k358_spi_reset);
    dc->vmsd = &vmstate_nxps32k358_spi;
}

static const TypeInfo nxps32k358_spi_info = {
    .name          = TYPE_NXPS32K358_SPI,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(NXPS32K358SPIState),
    .instance_init = nxps32k358_spi_init,
    .class_init    = nxps32k358_spi_class_init,
};

static void nxps32k358_spi_register_types(void)
{
    type_register_static(&nxps32k358_spi_info);
}

type_init(nxps32k358_spi_register_types)
