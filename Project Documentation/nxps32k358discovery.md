# NXP S32K358 Discovery Machine Emulation

Located in /qemu/hw/arm/nxps32k358discovery.c

## Overview

This file implements QEMU machine emulation for the NXP S32K358 board.

## Key Components

-   **System Clock**: Fixed at 24 MHz (SYSCLK_FRQ)
-   **SoC**: Uses NXPS32K358_SOC type
-   **CPU**: Cortex-M7 processor

## Initialization Flow

1. Creates fixed-frequency system clock (24MHz)
2. Instantiates NXP S32K358 SoC device
3. Connects system clock to SoC
4. Loads kernel image into flash memory:
    - Base address: `CODE_FLASH_BASE_ADDRESS`
    - Size: `CODE_FLASH_BLOCK_SIZE * 4`

## Machine Configuration

-   **Name**: `nxps32k358evb`
-   **Description**: "NXP NXPS32K358 (Cortex-M7)"
-   **Features**:
    -   No floppy support (`mc->no_floppy = 1`)
    -   No CD-ROM support (`mc->no_cdrom = 1`)
    -   No parallel port support (`mc->no_parallel = 1`)
-   **Valid CPU Types**: Only Cortex-M7 is supported

## Constants

```c
#define SYSCLK_FRQ 24000000ULL  // System clock frequency (24MHz)
```

## Functions

### `nxp_s32k358discovery_init(MachineState *machine)`

Initializes the machine:

1. Creates system clock with fixed frequency
2. Instantiates SoC device
3. Connects system clock to SoC
4. Realizes SoC device
5. Loads kernel into flash memory

### `nxp_s32k358discovery_machine_init(MachineClass *mc)`

Configures machine class properties:

-   Sets machine description
-   Assigns initialization function
-   Defines valid CPU types
-   Configures hardware limitations

## Implementation Notes

-   Derived from STM32VLDISCOVERY machine implementation
-   Uses QEMU's device model and clock infrastructure
-   Follows ARM boot protocol for loading kernels

## Dependencies

-   `qemu/osdep.h`
-   `qapi/error.h`
-   `hw/boards.h`
-   `hw/qdev-properties.h`
-   `hw/qdev-clock.h`
-   `qemu/error-report.h`
-   `hw/arm/nxps32k358_soc.h`
-   `hw/arm/boot.h`
