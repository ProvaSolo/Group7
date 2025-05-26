# Group7

## Correctly pull

1. git pull
2. git submodule update --init --recursive

cd build
../configure --target-list=arm-softmmu --enable-debug
make -j$(nproc)
./qemu-system-arm -M help

## Test

cd qemu/build
./qemu-system-arm -M nxps32k358discovery -nographic -kernel ../../Test/Dummy_firmware/dummy.bin -bios none
