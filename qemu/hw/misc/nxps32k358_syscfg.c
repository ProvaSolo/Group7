/*
 * NXPS32K358 SYSCFG (System Configuration Controller)
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "qemu/osdep.h"
#include "hw/sysbus.h"
#include "system/memory.h"
#include "hw/qdev-core.h"
#include "hw/resettable.h"
#include "qemu/log.h"
#include "trace.h"
#include "hw/irq.h"
#include "migration/vmstate.h"
#include "hw/clock.h"
#include "hw/qdev-clock.h"
#include "qapi/error.h"

#define TYPE_NXPS32K358_SYSCFG "nxps32k358-syscfg"
#define NXPS32K358_SYSCFG(obj) \
	OBJECT_CHECK(NXPS32K358SYSCFGState, (obj), TYPE_NXPS32K358_SYSCFG)

/* Base address and register offsets for S32K358 need to be verified from official documentation */
#define SYSCFG_BASE_ADDR 0x40268000 /* From memory map in Reference Manual */

/* Register offsets - these need to be verified from S32K358 documentation */
#define SYSCFG_CFGR1 0x00  /* Configuration Register 1 */
#define SYSCFG_MEMRMP 0x04 /* Memory Remap Register */
#define SYSCFG_SCSR 0x08   /* System Control/Status Register */
#define SYSCFG_CFGR2 0x0C  /* Configuration Register 2 */
#define SYSCFG_SKR 0x10	   /* Security Key Register */

/* Register masks - these need to be verified from S32K358 documentation */
#define ACTIVABLE_BITS_MEMRP 0x000000FFu /* Placeholder - needs verification */
#define ACTIVABLE_BITS_CFGR1 0x0000FFFFu /* Placeholder - needs verification */
#define ACTIVABLE_BITS_SKR 0x000000FFu	 /* Placeholder - needs verification */

typedef struct NXPS32K358SYSCFGState
{
	SysBusDevice parent_obj;
	MemoryRegion mmio;

	uint32_t memrmp;
	uint32_t cfgr1;
	uint32_t scsr;
	uint32_t cfgr2;
	uint32_t skr;

	Clock *clk;
} NXPS32K358SYSCFGState;

static void nxps32k358_syscfg_hold_reset(Object *obj, ResetType type)
{
	NXPS32K358SYSCFGState *s = NXPS32K358_SYSCFG(obj);

	s->memrmp = 0x00000000;
	s->cfgr1 = 0x7C000001;
	s->scsr = 0x00000000;
	s->cfgr2 = 0x00000000;
	s->skr = 0x00000000;
}

static uint64_t nxps32k358_syscfg_read(void *opaque, hwaddr addr,
									   unsigned int size)
{
	NXPS32K358SYSCFGState *s = opaque;

	switch (addr)
	{
	case SYSCFG_MEMRMP:
		return s->memrmp;
	case SYSCFG_CFGR1:
		return s->cfgr1;
	case SYSCFG_SCSR:
		return s->scsr;
	case SYSCFG_CFGR2:
		return s->cfgr2;
	case SYSCFG_SKR:
		return s->skr;
	default:
		qemu_log_mask(LOG_GUEST_ERROR,
					  "%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__, addr);
		return 0;
	}
}

static void nxps32k358_syscfg_write(void *opaque, hwaddr addr,
									uint64_t value, unsigned int size)
{
	NXPS32K358SYSCFGState *s = opaque;

	switch (addr)
	{
	case SYSCFG_MEMRMP:
		qemu_log_mask(LOG_UNIMP,
					  "%s: Changing the memory mapping isn't supported\n",
					  __func__);
		s->memrmp = value & ACTIVABLE_BITS_MEMRP;
		return;
	case SYSCFG_CFGR1:
		qemu_log_mask(LOG_UNIMP,
					  "%s: Functions in CFGRx aren't supported\n",
					  __func__);
		s->cfgr1 = value & ACTIVABLE_BITS_CFGR1;
		return;
	case SYSCFG_SCSR:
		s->scsr = value;
		return;
	case SYSCFG_CFGR2:
		s->cfgr2 = value;
		return;
	case SYSCFG_SKR:
		s->skr = value & ACTIVABLE_BITS_SKR;
		return;
	default:
		qemu_log_mask(LOG_GUEST_ERROR,
					  "%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__, addr);
	}
}

static const MemoryRegionOps nxps32k358_syscfg_ops = {
	.read = nxps32k358_syscfg_read,
	.write = nxps32k358_syscfg_write,
	.endianness = DEVICE_NATIVE_ENDIAN,
	.impl.min_access_size = 4,
	.impl.max_access_size = 4,
	.impl.unaligned = false,
	.valid.min_access_size = 4,
	.valid.max_access_size = 4,
	.valid.unaligned = false,
};

static void nxps32k358_syscfg_init(Object *obj)
{
	NXPS32K358SYSCFGState *s = NXPS32K358_SYSCFG(obj);

	memory_region_init_io(&s->mmio, obj, &nxps32k358_syscfg_ops, s,
						  TYPE_NXPS32K358_SYSCFG, 0x400);
	sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);
	s->clk = qdev_init_clock_in(DEVICE(s), "clk", NULL, s, 0);
}

static void nxps32k358_syscfg_realize(DeviceState *dev, Error **errp)
{
	NXPS32K358SYSCFGState *s = NXPS32K358_SYSCFG(dev);
	if (!clock_has_source(s->clk))
	{
		error_setg(errp, "SYSCFG: clk input must be connected");
		return;
	}
}

static const VMStateDescription vmstate_nxps32k358_syscfg = {
	.name = TYPE_NXPS32K358_SYSCFG,
	.version_id = 1,
	.minimum_version_id = 1,
	.fields = (VMStateField[]){
		VMSTATE_UINT32(memrmp, NXPS32K358SYSCFGState),
		VMSTATE_UINT32(cfgr1, NXPS32K358SYSCFGState),
		VMSTATE_UINT32(scsr, NXPS32K358SYSCFGState),
		VMSTATE_UINT32(cfgr2, NXPS32K358SYSCFGState),
		VMSTATE_UINT32(skr, NXPS32K358SYSCFGState),
		VMSTATE_CLOCK(clk, NXPS32K358SYSCFGState),
		VMSTATE_END_OF_LIST()}};

static void nxps32k358_syscfg_class_init(ObjectClass *klass, const void *data)
{
	DeviceClass *dc = DEVICE_CLASS(klass);
	ResettableClass *rc = RESETTABLE_CLASS(klass);

	dc->vmsd = &vmstate_nxps32k358_syscfg;
	dc->realize = nxps32k358_syscfg_realize;
	rc->phases.hold = nxps32k358_syscfg_hold_reset;
}

static const TypeInfo nxps32k358_syscfg_info = {
	.name = TYPE_NXPS32K358_SYSCFG,
	.parent = TYPE_SYS_BUS_DEVICE,
	.instance_size = sizeof(NXPS32K358SYSCFGState),
	.instance_init = nxps32k358_syscfg_init,
	.class_init = nxps32k358_syscfg_class_init,
};

static void nxps32k358_syscfg_register_types(void)
{
	type_register_static(&nxps32k358_syscfg_info);
}

type_init(nxps32k358_syscfg_register_types)
