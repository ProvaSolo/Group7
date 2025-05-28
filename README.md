# Group7
## Requirements
sudo apt install git libglib2.0-dev libfdt-dev libpixman-1-dev zlib1g-dev ninja-build
If issues with ninja-build
sudo apt update
sudo apt upgrade

## Correctly pull

1. git pull
2. git submodule update --init --recursive

```bash
./configure --target-list=arm-softmmu --enable-debug
make -j$(nproc)
./qemu-system-arm -M help
```

## Test

```bash
cd qemu/build
./qemu-system-arm -M nxps32k358discovery -nographic -kernel ../../Test/Dummy_firmware/dummy.bin -bios none
```
Testing the elf file generated from NXP S32 Design Studio

```bash
./build/qemu-system-arm \
    -M nxps32k358discovery \
    -kernel /home/vitoc/Group7/App/Test_S32_Pin/FreeRTOS_Toggle_Led_Example_S32K358_3.elf \
    -nographic \
    -S -s
```

