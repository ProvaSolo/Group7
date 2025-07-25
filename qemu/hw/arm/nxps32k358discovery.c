/*
 * NXP S32K358 Discovery board
 */

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "hw/boards.h"
#include "hw/qdev-properties.h"
#include "hw/qdev-clock.h"
#include "qemu/error-report.h"
#include "hw/arm/nxps32k358_soc.h"
#include "hw/arm/boot.h"

/* Main SYSCLK frequency in Hz (24MHz) */
#define SYSCLK_FRQ 24000000ULL // unsigned long long

static void nxp_s32k358discovery_init(MachineState *machine)
{
    DeviceState *dev;
    Clock *sysclk; /* This clock doesn't need migration because it is fixed-frequency */
    sysclk = clock_new(OBJECT(machine), "SYSCLK");
    clock_set_hz(sysclk, SYSCLK_FRQ);

    dev = qdev_new(TYPE_NXPS32K358_SOC);
    object_property_add_child(OBJECT(machine), "soc", OBJECT(dev));
    qdev_connect_clock_in(dev, "sysclk", sysclk);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    armv7m_load_kernel(NXPS32K358_SOC(dev)->armv7m.cpu,
                       machine->kernel_filename,
                       CODE_FLASH_BASE_ADDRESS, CODE_FLASH_BLOCK_SIZE * 4);
}

static void nxp_s32k358discovery_machine_init(MachineClass *mc)
{
    static const char *const valid_cpu_types[] = {
        ARM_CPU_TYPE_NAME("cortex-m7"), NULL};

    mc->desc = "NXP NXPS32K358 (Cortex-M7)";
    mc->init = nxp_s32k358discovery_init;
    mc->valid_cpu_types = valid_cpu_types;
    mc->no_floppy = 1;
    mc->no_cdrom = 1;
    mc->no_parallel = 1;
}

DEFINE_MACHINE("nxps32k358evb", nxp_s32k358discovery_machine_init)
