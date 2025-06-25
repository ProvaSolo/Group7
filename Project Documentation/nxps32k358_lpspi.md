# NXP S32K358 LPSPI Controller Documentation

## Overview

The NXP S32K358 LPSPI (Low Power Serial Peripheral Interface) is a QEMU device model that emulates the SPI peripheral of the S32K358 microcontroller. It supports full-duplex SPI communication, programmable FIFO buffers, multiple chip select lines, and interrupt-driven operation.

---

## Header File: `nxps32k358_lpspi.h`

### Key Definitions

-   **Register Offsets**: Defines the memory-mapped offsets for the LPSPI registers.
-   **Bit Masks**: Provides bit-level definitions for control and status registers.
-   **FIFO Depth**: Defines the FIFO depth and capacity.

### Key Constants

-   **Control Register (CR) Bits**:
    -   `LPSPI_CR_MEN`: Module Enable.
    -   `LPSPI_CR_RST`: Software Reset.
    -   `LPSPI_CR_RSTF`: FIFO Reset.
-   **Status Register (SR) Bits**:
    -   `LPSPI_SR_TDF`: Transmit Data Flag.
    -   `LPSPI_SR_RDF`: Receive Data Flag.
    -   `LPSPI_SR_MBF`: Message Buffer Flag.
-   **Receive Status Register (RSR) Bits**:
    -   `LPSPI_RSR_RXEMPTY`: RX FIFO Empty Flag.
-   **Transmit Command Register (TCR) Bits**:
    -   `TCR_PCS_SHIFT`: Position of the PCS field.
    -   `TCR_PCS_MASK`: Mask for the PCS field.
-   **FIFO Configuration**:
    -   `LPSPI_FIFO_WORD_DEPTH`: Depth of the FIFO in words.
    -   `LPSPI_FIFO_BYTE_CAPACITY`: Total FIFO capacity in bytes.
    -   `FCR_TXWATER_MASK`: Mask for the TX FIFO watermark field.
    -   `FCR_RXWATER_MASK`: Mask for the RX FIFO watermark field.

### Key Structures

-   **`NXPS32K358LPSPIState`**: Represents the state of the LPSPI device, including:
    -   **Parent Object**: `SysBusDevice parent_obj`.
    -   **Memory Region**: `MemoryRegion mmio`.
    -   **SSIBus**: `SSIBus *ssi`.
    -   **IRQ Line**: `qemu_irq irq`.
    -   **Chip Select Lines**: `uint8_t num_cs_lines` and `qemu_irq *cs_lines`.
    -   **FIFO Buffers**: `Fifo8 tx_fifo` and `Fifo8 rx_fifo`.
    -   **Registers**:
        -   `uint32_t lpspi_verid`: Version ID Register.
        -   `uint32_t lpspi_param`: Parameter Register.
        -   `uint32_t lpspi_cr`: Control Register.
        -   `uint32_t lpspi_sr`: Status Register.
        -   `uint32_t lpspi_ier`: Interrupt Enable Register.
        -   `uint32_t lpspi_der`: DMA Enable Register.
        -   `uint32_t lpspi_cfgr0`: Configuration Register 0.
        -   `uint32_t lpspi_cfgr1`: Configuration Register 1.
        -   `uint32_t lpspi_ccr`: Clock Configuration Register.
        -   `uint32_t lpspi_fcr`: FIFO Control Register.
        -   `uint32_t lpspi_fsr`: FIFO Status Register.
        -   `uint32_t lpspi_tcr`: Transmit Command Register.
        -   `uint32_t lpspi_tdr`: Transmit Data Register.
        -   `uint32_t lpspi_rsr`: Receive Status Register.
        -   `uint32_t lpspi_rdr`: Receive Data Register.

---

## Source File: `nxps32k358_lpspi.c`

### Debug Logging

The driver uses a macro `DB_PRINT_L` to conditionally emit debug logs. This macro wraps `qemu_log()` and only logs when `NXP_LPSPI_ERR_DEBUG` is enabled. It is primarily used for tracing register accesses, FIFO operations, and error conditions.

### Key Functions

#### `lpspi_update_status()`

-   **Purpose**: Updates the internal status and FIFO status registers (`SR`, `FSR`, and `RSR`) of the LPSPI peripheral to reflect the current state of the transmit (TX) and receive (RX) FIFOs.

-   **Functionality**:

    -   **FIFO Word Count Calculation**:

        -   Calculates the number of 32-bit words currently stored in each FIFO:

            -   **TX Word Count**: Computed by dividing the number of bytes used in the TX FIFO by 4.
            -   **RX Word Count**: Computed by dividing the number of bytes used in the RX FIFO by 4.

    -   **FIFO Status Register (`FSR`) Update**:

        -   Encodes the word counts into the `FSR` register:

            -   Lower 16 bits: TX Word Count.
            -   Upper 16 bits: RX Word Count.

    -   **Status Register (`SR`) Update**:

        -   Evaluates FIFO thresholds to set or clear relevant flags based on the
            programmed watermarks:

            -   Sets the `TDF` (Transmit Data Flag) when the number of words in
                the TX FIFO is less than or equal to `TXWATER`.
            -   Clears `TDF` when more words are present.
            -   Sets the `RDF` (Receive Data Flag) when the RX FIFO contains more
                words than the `RXWATER` threshold.
            -   Clears `RDF` when the RX FIFO has fewer or equal words.

    -   **Receive Status Register (`RSR`) Update**:

        -   Reflects the empty state of the RX FIFO:

            -   Sets the `RXEMPTY` flag if the RX Word Count is zero.
            -   Clears the `RXEMPTY` flag if data is available in the RX FIFO.

#### `lpspi_update_irq()`

-   **Purpose**: Manages the state of the IRQ (Interrupt Request) line by evaluating the current status flags in relation to the interrupt enable register (`IER`), ensuring the CPU is notified of relevant events such as FIFO space or data availability.

-   **Functionality**:

    -   **Status Synchronization**:

        -   Calls `lpspi_update_status()` to update the status (`SR`) and FIFO status (`FSR`) registers, ensuring they accurately represent the current FIFO state.

    -   **Interrupt Condition Evaluation**:

        -   Performs a bitwise AND between the `SR` (Status Register) and `IER` (Interrupt Enable Register) to identify active and enabled interrupt conditions.
        -   Specifically checks:

            -   `LPSPI_SR_TDF` (Transmit Data Flag): Set when the TX FIFO level is below `TXWATER`.
            -   `LPSPI_SR_RDF` (Receive Data Flag): Set when the RX FIFO level exceeds `RXWATER`.

    -   **IRQ Line Control**:

        -   Asserts the IRQ line (`qemu_set_irq(s->irq, 1)`) if any enabled interrupt conditions are active.
        -   Deasserts the IRQ line (`qemu_set_irq(s->irq, 0)`) if no enabled conditions are present.

#### `lpspi_flush_txfifo()`

-   **Purpose**: This function handles SPI transfers by flushing the TX FIFO (Transmit FIFO) and transferring data to the RX FIFO (Receive FIFO) via the SPI bus. It ensures proper chip select handling and updates the device state accordingly.

-   **Functionality**:

    -   **Transfer Conditions**:
        -   The function checks if a transfer is possible by ensuring:
            -   The TX FIFO contains at least 4 bytes of data.
            -   The RX FIFO has at least 4 bytes of free space.
        -   If these conditions are not met, the function logs the state of the FIFOs, updates the IRQ line, and exits without performing a transfer.
    -   **Chip Select Validation**:
        -   Extracts the chip select (CS) value from the Transmit Command Register (`TCR`).
        -   Validates the CS value against the number of available chip select lines.
        -   If the CS value is invalid, logs an error and exits.
    -   **Chip Select Assertion**:
        -   Asserts the appropriate chip select line to initiate the SPI transfer.
    -   **Data Transfer**:
        -   While the TX FIFO has at least 4 bytes of data and the RX FIFO has at least 4 bytes of free space:
            -   Pops 4 bytes from the TX FIFO to form a 32-bit word (`tx_word`).
            -   Transfers the `tx_word` via the SPI bus using the `ssi_transfer()` function.
            -   Pushes the received 32-bit word (`rx_word`) into the RX FIFO as 4 individual bytes.
    -   **Chip Select Deassertion**:
        -   Deasserts the chip select line after completing the transfer burst.
    -   **Status Updates**:
        -   Clears the `MBF` (Message Buffer Flag) in the Status Register (`SR`) if the TX FIFO is empty.
        -   Updates the IRQ line to reflect the current state of the device.

-   **Debug Logging**:
    -   Logs the state of the FIFOs if a transfer is blocked.
    -   Logs chip select assertion and deassertion events.
    -   Logs when the TX FIFO becomes empty and the `MBF` flag is cleared.

#### `nxps32k358_lpspi_do_reset()`

-   **Purpose**:  
    This function resets the NXPS32K358 LPSPI device to its default state. It is typically called during system initialization or when a reset condition is triggered.
-   **Functionality**:

    -   Sets the `VERID` register to `0x02000004`, indicating the version of the LPSPI module.
    -   Sets the `PARAM` register to `0x00080202`, defining the module's parameters.
    -   Resets the control register (`CR`) to `0x0`, disabling the module.
    -   Clears the status register (`SR`) and sets the transmit data flag (`TDF`).
    -   Disables all interrupts by resetting the interrupt enable register (`IER`) and DMA enable register (`DER`) to `0x0`.
    -   Resets configuration registers (`CFGR0`, `CFGR1`) and clock configuration register (`CCR`) to `0x0`.
    -   Clears FIFO control and status registers (`FCR`, `FSR`).
    -   Initializes transmit command register (`TCR`) to `0xFFFFFFFF`.
    -   Clears transmit data register (`TDR`) and receive data register (`RDR`).
    -   Marks the receive FIFO as empty in the receive status register (`RSR`).
    -   Resets both TX and RX FIFOs using `fifo8_reset()`.
    -   Deasserts all chip select lines by setting them to inactive state.
    -   Updates the interrupt state by calling `lpspi_update_irq()`.

#### `nxps32k358_lpspi_realize()`

-   **Purpose**:
    Initializes the NXPS32K358 LPSPI device during its realization phase, setting up all required hardware structures such as memory regions, interrupt lines, chip select GPIOs, and FIFO buffers. This function ensures the device is fully integrated into the QEMU virtual hardware environment.

-   **Functionality**:
    Performs the following initialization steps in sequence:

1. **Memory Region Initialization**:

    - Initializes the memory-mapped I/O region for the device with `memory_region_init_io()`.
    - Binds the region to the device state (`OBJECT(s)`) and register access callbacks (`nxps32k358_lpspi_ops`).
    - Specifies the size of the region via `S32K_LPSPI_REG_MAX_OFFSET`.
    - Registers the region with the system bus using `sysbus_init_mmio()` to allow access by other components.

2. **SPI Bus Creation**:

    - Creates and attaches a serial peripheral interface (SPI) bus to the device using `ssi_create_bus()`.
    - Associates the bus with the QEMU device object (`dev`) and assigns it the name `"spi"` for identification.

3. **IRQ Initialization**:

    - Initializes the IRQ line (Interrupt request line) with `sysbus_init_irq()`, enabling the device to signal interrupts to the virtual CPU.

4. **Chip Select (CS) Line Configuration**:

    - Allocates memory for chip select GPIO lines based on `s->num_cs_lines` using `g_new0()`.
    - Initializes these lines as GPIO outputs with `qdev_init_gpio_out_named()`, tagging them with the name `"cs"` for each chip select.

5. **FIFO Buffer Initialization**:

    - Initializes the transmit (TX) FIFO using `fifo8_create()` with a predefined capacity (`LPSPI_FIFO_BYTE_CAPACITY`).
    - Initializes the receive (RX) FIFO using the same capacity, ensuring symmetric buffer sizing for full-duplex transfers.

6. **Error Handling**:

    - Ensures each subsystem (memory, bus, IRQ, GPIOs, FIFOs) is initialized in order.
    - If any step fails, the realization halts and logs an appropriate error to aid in debugging and diagnosis.

-   **Debug Logging**:

    -   Provides logs for memory region creation, IRQ setup, GPIO configuration, and FIFO initialization.
    -   Logs errors in case of allocation or setup failures to assist in tracing device bring-up issues.

#### `nxps32k358_lpspi_reset()`

-   **Purpose**:
    This function resets the NXPS32K358 LPSPI device to its default state. It is typically called during system initialization or when a reset condition is triggered.
-   **Functionality**:
    -   Calls `nxps32k358_lpspi_do_reset()` to perform the reset operation.

#### `nxps32k358_lpspi_read()`

-   **Purpose**:  
    Reads data from the specified memory-mapped register of the NXPS32K358 LPSPI device.

-   **Functionality**:

    -   Determines the register to read based on the provided `addr` offset.
    -   Returns the value of the corresponding register.
    -   Handles special cases:
        -   For the `RDR` (Receive Data Register), it reads 4 bytes from the RX FIFO, combines them into a 32-bit value, and updates the `RDR` register.
        -   If the RX FIFO has fewer than 4 bytes, it returns `0`.
    -   Logs an error if the `addr` does not match any valid register offset.

-   **Key Steps**:

    1. Calls `lpspi_update_status()` to ensure the device state is up-to-date.
    2. Uses a `switch` statement to map the `addr` to the corresponding register.
    3. For the `RDR` case:
        - Checks if the RX FIFO contains at least 4 bytes.
        - Pops 4 bytes from the RX FIFO, combines them into a 32-bit value, and updates the `RDR` register.
        - Calls `lpspi_flush_txfifo()` to handle any pending SPI transfers.
    4. Logs an error for invalid `addr` values.

-   **Registers Handled**:

    -   `VERID`, `PARAM`, `CR`, `SR`, `IER`, `DER`, `CFGR0`, `CFGR1`, `CCR`, `FCR`, `FSR`, `TCR`, `RSR`, `RDR`.

-   **Error Handling**:

    -   Logs an error message if the `addr` is invalid.

-   **Return Value**:

    -   The value of the specified register, or `0` for invalid reads or empty RX FIFO.

#### `nxps32k358_lpspi_write()`

-   **Purpose**:  
    Writes data to the specified memory-mapped register of the NXPS32K358 LPSPI device.

-   **Functionality**:

    -   Determines the register to write to based on the provided `addr` offset.
    -   Updates the value of the corresponding register.
    -   Handles special cases:
        -   Logs an error if attempting to write to a read-only register.
        -   Resets the device if the `RST` bit in the `CR` register is set.
        -   Resets the FIFOs if the `RSTF` bit in the `CR` register is set.
        -   Handles FIFO operations for the `TDR` (Transmit Data Register).
        -   Updates the IRQ line after each write.

-   **Key Steps**:

    1. Uses a `switch` statement to map the `addr` to the corresponding register.
    2. For read-only registers (`VERID`, `PARAM`, `FSR`, `RSR`, `RDR`):
        - Logs an error and ignores the write.
    3. For the `CR` (Control Register):
        - Resets the device if the `RST` bit is set.
        - Resets the FIFOs if the `RSTF` bit is set.
        - Updates the `CR` register value.
    4. For the `SR` (Status Register):
        - Clears the specified bits in the `SR` register.
    5. For the `TCR` (Transmit Command Register):
        - Updates the `TCR` register value.
        - If the module is enabled (`MEN` bit in `CR`), checks the FIFO state and flushes the TX FIFO.
    6. For the `TDR` (Transmit Data Register):
        - Writes data to the TX FIFO if the module is enabled.
        - Logs an error if the TX FIFO is full or the module is not enabled.
    7. For other writable registers (`IER`, `DER`, `CFGR0`, `CFGR1`, `CCR`, `FCR`):
        - Updates the corresponding register value.
    8. Logs an error for invalid `addr` values.
    9. Calls `lpspi_update_irq()` to update the IRQ line.

-   **Registers Handled**:

    -   **Read-Only**: `VERID`, `PARAM`, `FSR`, `RSR`, `RDR`.
    -   **Writable**: `CR`, `SR`, `TCR`, `TDR`, `IER`, `DER`, `CFGR0`, `CFGR1`, `CCR`, `FCR`.

-   **Error Handling**:

    -   Logs an error message if attempting to write to a read-only register.
    -   Logs an error if the TX FIFO is full or the module is not enabled when writing to `TDR`.
    -   Logs an error for invalid `addr` values.

#### `nxps32k358_lpspi_class_init()`

-   **Purpose**: This function initializes the class-level properties of the NXP S32K358 LPSPI device type, preparing it for use within the QEMU device model infrastructure.

-   **Functionality**:

    -   **Device Class Casting**:

        -   Casts the provided `ObjectClass` to a `DeviceClass` to access standard device initialization hooks.

    -   **Realize Function Assignment**:

        -   Sets the `realize` function pointer to `nxps32k358_lpspi_realize`, defining how the device is instantiated and initialized at runtime.

    -   **Legacy Reset Handler**:

        -   Assigns `nxps32k358_lpspi_reset` as the legacy reset function, enabling the device to support system resets.

    -   **Device Properties Setup**:

        -   Registers the property list of the device by calling `device_class_set_props` with `nxps32k358_lpspi_properties`.

    -   **VMState Description Assignment**:

        -   Sets the `vmsd` field to `vmstate_nxps32k358_lpspi`, allowing the device to support VM snapshot and migration by describing how its state should be saved and restored.

#### `nxps32k358_lpspi_register_types()`

-   **Purpose**: Registers the NXP S32K358 LPSPI device type with the QEMU type system so it can be instantiated and used during emulation.

-   **Functionality**:

    -   Calls `type_register_static()` with the `nxps32k358_lpspi_info` structure to make the device type available to the QEMU object model.

### Key Constants

#### Memory Region Operations

-   **Read**: Implemented via `nxps32k358_lpspi_read()`.
-   **Write**: Implemented via `nxps32k358_lpspi_write()`.
-   **Endianess**: Default to little-endian for 32-bit accesses.
-   **Minimum Access Size**: 4 bytes.
-   **Maximum Access Size**: 4 bytes.

### VMState Description

-   **Name**: The name of the VMState structure, set to "nxps32k358_lpspi".
-   **Version**: The version of the VMState structure, set to 1.
-   **Fields**: An array of VMStateField structures that describe the individual fields to be saved and restored during migration. These fields include:
    -   tx_fifo: Transmit FIFO buffer.
    -   rx_fifo: Receive FIFO buffer.
    -   lpspi_verid: Version ID register.
    -   lpspi_param: Parameter register.
    -   lpspi_cr: Control register.
    -   lpspi_sr: Status register.
    -   lpspi_ier: Interrupt enable register.
    -   lpspi_der: DMA enable register.
    -   lpspi_cfgr0: Configuration register 0.
    -   lpspi_cfgr1: Configuration register 1.
    -   lpspi_ccr: Clock configuration register.
    -   lpspi_fcr: FIFO control register.
    -   lpspi_fsr: FIFO status register.
    -   lpspi_tcr: Transmit command register.
    -   lpspi_tdr: Transmit data register.
    -   lpspi_rsr: Receive status register.
    -   lpspi_rdr: Receive data register.
    -   VMSTATE_END_OF_LIST(): Marks the end of the field list.

### Property Definitions

-   **num-cs-lines**: Specifies the number of chip select lines available for the LPSPI device. This property is defined with a default value of 1, indicating that at least one chip select line is required.

---

## Register Map

| Register | Offset | Description                  |
| -------- | ------ | ---------------------------- |
| VERID    | 0x00   | Version ID                   |
| PARAM    | 0x04   | Parameter information        |
| CR       | 0x10   | Control Register             |
| SR       | 0x14   | Status Register              |
| IER      | 0x18   | Interrupt Enable Register    |
| DER      | 0x1C   | DMA Enable Register          |
| CFGR0    | 0x20   | Configuration Register 0     |
| CFGR1    | 0x24   | Configuration Register 1     |
| CCR      | 0x40   | Clock Configuration Register |
| FCR      | 0x58   | FIFO Control Register        |
| FSR      | 0x5C   | FIFO Status Register         |
| TCR      | 0x60   | Transmit Command Register    |
| TDR      | 0x64   | Transmit Data Register       |
| RSR      | 0x70   | Receive Status Register      |
| RDR      | 0x74   | Receive Data Register        |

---

## Key Features

### FIFO Management

-   4-word deep TX and RX FIFOs (16 bytes each).
-   Automatic flushing when:
    -   TX FIFO has at least 1 word.
    -   RX FIFO has space for 1 word.
    -   Module is enabled (`MEN` bit set).
-   Programmable watermarks (`TXWATER`/`RXWATER`) in `FCR` determine when the
    `TDF` and `RDF` status flags assert.

### Interrupt Handling

-   Interrupts are triggered when FIFO levels cross the watermarks:
    -   TX FIFO drops below `TXWATER`, setting `TDF`.
    -   RX FIFO rises above `RXWATER`, setting `RDF`.
-   Enabled through the Interrupt Enable Register (`IER`).

### Chip Select Handling

-   Supports multiple chip select lines.
-   Automatically asserts/deasserts CS during transfers.
-   Configurable via the Transmit Command Register (`TCR`).

---
