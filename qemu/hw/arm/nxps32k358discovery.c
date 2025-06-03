/*
 * ST STM32VLDISCOVERY machine
 *
 * Copyright (c) 2021 Alexandre Iooss <erdnaxe@crans.org>
 * Copyright (c) 2014 Alistair Francis <alistair@alistair23.me>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "hw/boards.h"
#include "hw/qdev-properties.h"
#include "hw/qdev-clock.h"
#include "qemu/error-report.h"
#include "hw/arm/nxps32k358_soc.h"
#include "hw/arm/boot.h"

/* nxp_s32k358discovery implementation is derived from stm32vldiscovery */

/* Main SYSCLK frequency in Hz (24MHz) */
#define SYSCLK_FRQ 24000000ULL // unsigned long long

static void nxp_s32k358discovery_init(MachineState *machine)
{
        // Cast the NXP machine from the generic machine
    NXPS32K3X8EVBMachineState *m_state = NXPS32K3X8EVB_MACHINE(machine);

    // Initialize system clock
    m_state->sysclk = clock_new(OBJECT(machine), "SYSCLK");
    clock_set_hz(m_state->sysclk, SYSCLK_FRQ);

    // Initialize the SoC
    object_initialize_child(OBJECT(machine), "s32k", &m_state->s32k,
                            TYPE_NXPS32K358_SOC);
    DeviceState *soc_state = DEVICE(&m_state->s32k);
    qdev_connect_clock_in(soc_state, "sysclk", m_state->sysclk);
    sysbus_realize(SYS_BUS_DEVICE(&m_state->s32k), &error_abort);

    // Load kernel image
    armv7m_load_kernel(ARM_CPU(first_cpu), machine->kernel_filename,
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
