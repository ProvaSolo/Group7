#ifndef HW_ARM_NXPS32K358_SOC_H
#define HW_ARM_NXPS32K358_SOC_H

//#include "hw/misc/stm32f2xx_syscfg.h"
#include "hw/char/nxps32k358_lpuart.h"
#include "hw/or-irq.h"
#include "hw/ssi/nxps32k358_lpspi.h"
#include "hw/arm/armv7m.h"
#include "hw/clock.h"
#include "qom/object.h"
#include "hw/misc/nxps32k358_syscfg.h"


#define TYPE_NXPS32K358_SOC "nxps32k358-soc"
OBJECT_DECLARE_SIMPLE_TYPE(NXPS32K358State, NXPS32K358_SOC)

#define NXP_NUM_LPUARTS 8
#define NXP_NUM_LPSPIS 4

#define FLASH_BASE_ADDRESS 0x08000000
#define FLASH_SIZE (1024 * 1024)
#define SRAM_BASE_ADDRESS 0x20000000
#define SRAM_SIZE (128 * 1024)

struct NXPS32K358State {
    SysBusDevice parent_obj;

    ARMv7MState armv7m;

    NXPS32K358SYSCFGState syscfg;
    NXPS32K358LPUARTState lpuarts[NXP_NUM_LPUARTS];
    NXPS32K358LPSPIState lpspis[NXP_NUM_LPSPIS];

    OrIRQState *adc_irqs;

    MemoryRegion sram;
    MemoryRegion flash;
    MemoryRegion flash_alias;

    Clock *sysclk;
    Clock *refclk;

    Clock *aips_plat_clk; 
    Clock *aips_slow_clk; 
};

#endif