# Group7
## Requirements
sudo apt install git libglib2.0-dev libfdt-dev libpixman-1-dev zlib1g-dev ninja-build
If issues with ninja-build
sudo apt update
sudo apt upgrade

## Correctly pull

1. git pull
2. git submodule update --init --recursive

Verificare anche il comando ./configure, forse piu completo
```bash
./configure --target-list=arm-softmmu --enable-debug
make -j$(nproc)
./qemu-system-arm -M help
```


Anche noi abbiamo utilizzato la lpuart 3

## Test

Testing with a dummy firmware
```bash
cd qemu/build
./qemu-system-arm -M nxps32k358ev -nographic -kernel ../../Test/Dummy_firmware/dummy.bin -bios none

./qemu-system-arm -M nxps32k3x8evb -nographic -kernel kernel.elf -serial none -serial none -serial none -serial mon:stdio -d guest_errors

```
Testing the elf file generated from NXP S32 Design Studio

```bash
./build/qemu-system-arm \
    -M nxps32k358evb \
    -cpu cortex-m7 \
    -kernel /home/vitoc/Group7/App/Test_S32_Pin/FreeRTOS_Toggle_Led_Example_S32K358_3.elf \
    -nographic \
    -S -s
```

gdb multi-arch /home/vitoc/Group7/App/Test_S32_Pin/FreeRTOS_Toggle_Led_Example_S32K358_3.elf

Running our FreeRTOS OS on modified qemu
```bash

```

./qemu-system-arm -M nxps32k358evb -nographic -kernel ../../Demo_FreeRTOS.elf -serial none -serial none -serial no
ne -serial mon:stdio -d guest_errors

gdb --args ./qemu-system-arm -M nxps32k358evb -nographic -kernel ../../Demo_FreeRTOS.elf -serial none -serial none -serial no
ne -serial mon:stdio -d guest_errors

`-serial none` (three times) and then `-serial mon:stdio` since there are 16 LPUART interfaces and we are using LPUART 3 (the fourth) in our DEMO project, we disable the first three and then bind the fourth to STDIO