/*
 * NXP S32K358 SYSCFG
 */

#ifndef HW_NXPS32K358_SYSCFG_H
#define HW_NXPS32K358_SYSCFG_H

#include "hw/sysbus.h"
#include "qom/object.h"

#define TYPE_NXPS32K358_SYSCFG "nxps32k358-syscfg"
OBJECT_DECLARE_SIMPLE_TYPE(Nxps32k358SyscfgState, NXPS32K358_SYSCFG)

struct Nxps32k358SyscfgState
{
    SysBusDevice parent_obj;

    MemoryRegion mmio;

    uint32_t memrmp;
    uint32_t cfgr1;
    uint32_t scsr;
    uint32_t cfgr2;
    uint32_t swpr;
    uint32_t skr;
    uint32_t swpr2;

    Clock *clk;
};

#endif
