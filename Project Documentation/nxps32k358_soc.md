# Documentation: NXP S32K358 SoC QEMU Model

## Overview

The `nxps32k358_soc` module provides a QEMU System-on-Chip (SoC) model for the NXP S32K358 microcontroller. This model implements a Cortex-M7 CPU core with various peripherals including UARTs (LPUART), Serial Peripheral Interfaces (LPSPI), clock management, memory regions, and system configuration blocks.

## Header File: `nxps32k358_soc.h`

The header file defines the structure and configuration macros for the S32K358 SoC:

```c
#ifndef HW_ARM_NXPS32K358_SOC_H
#define HW_ARM_NXPS32K358_SOC_H

#include "hw/char/nxps32k358_lpuart.h"
#include "hw/ssi/nxps32k358_lpspi.h"
#include "hw/arm/armv7m.h"
#include "qom/object.h"
#include "hw/misc/nxps32k358_syscfg.h"

#define TYPE_NXPS32K358_SOC "nxps32k358-soc"
OBJECT_DECLARE_SIMPLE_TYPE(NXPS32K358State, NXPS32K358_SOC)

#define NXP_NUM_LPUARTS 8
#define NXP_NUM_LPSPIS 4

#define FLASH_BASE_ADDRESS 0x08000000
#define FLASH_SIZE (1024 * 1024) // 1MB Flash memory
#define SRAM_BASE_ADDRESS 0x20000000
#define SRAM_SIZE (128 * 1024) // 128KB RAM memory

struct NXPS32K358State {
    SysBusDevice parent_obj;

    ARMv7MState armv7m;       // Cortex-M7 CPU model

    NXPS32K358SYSCFGState syscfg; // System configuration controller

    NXPS32K358LPUARTState lpuarts[NXP_NUM_LPUARTS]; // LPUART peripherals
    NXPS32K358LPSPIState lpspis[NXP_NUM_LPSPIS];    // LPSPI peripherals

    OrIRQState *adc_irqs;     // ADC interrupt controller

    MemoryRegion sram;        // SRAM memory region
    MemoryRegion flash;       // Flash memory region
    MemoryRegion flash_alias; // Flash alias (for booting)

    Clock *sysclk;            // System clock
    Clock *refclk;            // Reference clock

    Clock *aips_plat_clk;     // AIPS platform clock
    Clock *aips_slow_clk;     // AIPS slow clock
};

#endif
```

### Key Definitions in the Header

1. **Constants and Macros**:

    - `NXP_NUM_LPUARTS`: Number of Low-Power Universal Asynchronous Receiver/Transmitters (8)
    - `NXP_NUM_LPSPIS`: Number of Low-Power Serial Peripheral Interfaces (4)
    - Memory addresses:
        - `FLASH_BASE_ADDRESS`: Base address of the Flash memory (0x08000000)
        - `FLASH_SIZE`: Size of the Flash memory (1MB)
        - `SRAM_BASE_ADDRESS`: Base address of the SRAM (0x20000000)
        - `SRAM_SIZE`: Size of the SRAM (128KB)

2. **Structure Definition** (`NXPS32K358State`):
    - `parent_obj`: Base structure for system bus devices
    - `armv7m`: Cortex-M7 CPU model
    - `syscfg`: System configuration controller
    - Arrays of peripherals:
        - `lpuarts`: Array of LPUART controllers
        - `lpspis`: Array of LPSPI controllers
    - Clock pointers:
        - `sysclk`, `refclk`: Primary system clocks
        - `aips_plat_clk`, `aips_slow_clk`: Additional peripheral clocks
    - Memory regions:
        - `sram`, `flash`: Main memory regions
        - `flash_alias`: Memory alias for booting

## Source File: `nxps32k358_soc.c`

The implementation file contains the SoC initialization and configuration logic:

```c
#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu/module.h"
#include "hw/arm/boot.h"
#include "system/address-spaces.h"
#include "hw/arm/nxps32k358_soc.h"
#include "hw/qdev-properties.h"
// ... [other includes] ...

static const uint32_t lpuart_addr[NXP_NUM_LPUARTS] = { ... };
static const uint32_t lpspi_addr[NXP_NUM_LPSPIS] = { ... };
static const int lpuart_irq[NXP_NUM_LPUARTS] = { ... };
static const int lpspi_irq[NXP_NUM_LPSPIS] = { ... };

// ... [unimplemented devices creation] ...

static void nxps32k358_soc_initfn(Object *obj) {
    // Initialization code
}

static void nxps32k358_soc_realize(DeviceState *dev_soc, Error **errp) {
    // Realization code
}

static void nxps32k358_soc_class_init(ObjectClass *klass, const void *data) {
    // Class initialization
}

static const TypeInfo nxps32k358_soc_info = {
    .name = TYPE_NXPS32K358_SOC,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(NXPS32K358State),
    .instance_init = nxps32k358_soc_initfn,
    .class_init = nxps32k358_soc_class_init,
};

void nxps32k358_soc_types(void) {
    type_register_static(&nxps32k358_soc_info);
}

type_init(nxps32k358_soc_types)
```

### Key Components of the Implementation

1. **Initialization Function** (`nxps32k358_soc_initfn`):

    - Initializes the ARMv7M CPU core (Cortex-M7)
    - Sets up memory regions for Flash and SRAM
    - Initializes system configuration controllers
    - Configures CPU and clock settings

2. **Realization Function** (`nxps32k358_soc_realize`):

    - Creates unimplemented devices for placeholder peripherals
    - Configures clocks (refclk, sysclk, AIPS clocks)
    - Initializes peripheral controllers (LPUARTs, LPSPIs)
    - Maps device registers to specific memory addresses
    - Connects interrupts for peripherals

3. **Clock Configuration**:

    - The reference clock (refclk) is set to the system clock divided by 8
    - Various peripheral clocks (aips_plat_clk, aips_slow_clk) are configured with specific frequencies

4. **Memory Region Setup**:

    - Flash memory (1MB) is mapped to 0x08000000
    - SRAM memory (128KB) is mapped to 0x20000000
    - Flash alias is created for booting purposes

5. **Peripheral Initialization**:
    - LPUART (serial communication) controllers
    - LPSPI (I2S/SPI) controllers
    - System configuration controller

### Peripheral Details

**LPUART (Low-Power Universal Asynchronous Receiver/Transmitter)**:

-   Number: 8 instances
-   Base addresses: Defined in `lpuart_addr` array
-   Interrupts: Defined in `lpuart_irq` array
-   Features:
    -   Asynchronous serial communication
    -   Low-power modes
    -   Configurable baud rates
    -   FIFO buffers

**LPSPI (Low-Power Serial Peripheral Interface)**:

-   Number: 4 instances
-   Base addresses: Defined in `lpspi_addr` array
-   Interrupts: Defined in `lpspi_irq` array
-   Features:
    -   Synchronous serial communication
    -   Master/slave modes
    -   Configurable data formats
    -   Low-power operation

### Usage and Integration

To use this SoC model in QEMU:

1. **Register the Type**:

    ```c
    #include "nxps32k358_soc.h"
    type_init(nxps32k358_soc_types);
    ```

2. **Create the SoC Instance**:

    ```c
    DeviceState *soc = qdev_new("nxps32k358-soc");
    ```

3. **Configure Clocks**:

    ```c
    // Set system clock frequency
    qdev_prop_set_uint32(OBJECT(soc), "sysclk-frequency", 100000000);

    // Set reference clock source
    qdev_prop_set_string(OBJECT(soc), "refclk-source", "external_xtal");
    ```

4. **Connect Peripherals**:

    ```c
    // Connect UART devices
    for (int i = 0; i < NXP_NUM_LPUARTS; i++) {
        qdev_connect_gpio_out(DEVICE(soc), lpuart_irq[i], ...);
    }
    ```

5. **Add to System Bus**:
    ```c
    SysBusDevice *busdev = SYS_BUS_DEVICE(soc);
    sysbus_realize(busdev, errp);
    sysbus_mmio_map(busdev, 0, ...);
    ```

### Clock Configuration Details

The SoC implements a hierarchical clocking structure:

1. **Reference Clock (refclk)**:

    - Base clock for the system
    - Typically derived from an external crystal
    - Divided by 8 to generate system clock (sysclk)

2. **System Clock (sysclk)**:
    - Primary clock for the CPU and most peripherals
    - Default frequency: 100 MHz (can be configured)
3. **AIPS Clocks**:
    - `aips_plat_clk`: 80 MHz (for high-performance peripherals)
    - `aips_slow_clk`: 40 MHz (for low-power peripherals)

### Memory Mapping

The S32K358 SoC implements a standard memory map:

```
+---------------------+-----------------------+--------------------+--------------------+
| Memory Type         | Base Address          | Size               | Description        |
+---------------------+-----------------------+--------------------+--------------------+
| Flash Memory        | 0x08000000            | 1,048,576 B (1MB)  | Program memory     |
| System Memory Alias | 0x00000000            | 1,048,576 B (1MB)  | Boot memory alias  |
| SRAM                | 0x20000000            | 131,072 B (128KB)  | Data memory        |
+---------------------+-----------------------+--------------------+--------------------+
```

### Register Interface

Each peripheral implements a set of configuration and status registers through memory-mapped I/O:

```c
// Example: LPUART register access
void lpuart_set_baudrate(uint32_t base, uint32_t baudrate) {
    uint32_t sysclk = get_current_clock_frequency(SYSCLK);
    uint32_t divider = sysclk / (16 * baudrate);
    // Write to register at base + 0x2C
}

uint32_t lpuart_get_status(uint32_t base) {
    return readl(base + LPUART_SR_OFFSET);
}
```

### Conclusion

The `nxps32k358_soc` module provides a comprehensive and flexible model of the NXP S32K358 SoC for QEMU-based development and simulation. This model includes support for multiple peripherals, configurable clocking, and appropriate memory mapping, making it suitable for embedded development, firmware testing, and system-level simulation.
