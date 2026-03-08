mkdir -p build build/tmp
export TMPDIR="$PWD/build/tmp"

clang --target=aarch64-none-elf -ffreestanding -mgeneral-regs-only -c boot/boot.S -o build/boot.o
clang --target=aarch64-none-elf -ffreestanding -mgeneral-regs-only -c kernel/kernel.c -o build/kernel.o
clang --target=aarch64-none-elf -ffreestanding -mgeneral-regs-only -c kernel/io.c -o build/io.o
clang --target=aarch64-none-elf \
  -fuse-ld=lld \
  -nostdlib \
  -Wl,-T,linker.ld \
  build/boot.o build/kernel.o build/io.o \
  -o build/kernel.elf
