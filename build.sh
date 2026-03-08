clang --target=aarch64-none-elf -ffreestanding -c boot/boot.S -o build/boot.o
clang --target=aarch64-none-elf -ffreestanding -c kernel/kernel.c -o build/kernel.o
clang --target=aarch64-none-elf \
  -fuse-ld=lld \
  -nostdlib \
  -Wl,-T,linker.ld \
  build/boot.o build/kernel.o \
  -o build/kernel.elf