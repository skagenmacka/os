mkdir -p build build/tmp
export TMPDIR="$PWD/build/tmp"

clang --target=aarch64-none-elf -ffreestanding -mgeneral-regs-only -c boot/boot.S -o build/boot.o
clang --target=aarch64-none-elf -ffreestanding -mgeneral-regs-only -c kernel/interrupts.S -o build/int.o
clang --target=aarch64-none-elf -ffreestanding -mgeneral-regs-only -c kernel/kernel.c -o build/kernel.o
clang --target=aarch64-none-elf -ffreestanding -mgeneral-regs-only -c kernel/io.c -o build/io.o
clang --target=aarch64-none-elf -ffreestanding -mgeneral-regs-only -c kernel/timer.c -o build/timer.o
clang --target=aarch64-none-elf -ffreestanding -mgeneral-regs-only -c kernel/lib/string.c -o build/string.o
clang --target=aarch64-none-elf -ffreestanding -mgeneral-regs-only -c kernel/lib/cmd.c -o build/cmd.o
clang --target=aarch64-none-elf -ffreestanding -mgeneral-regs-only -c kernel/fs/vfs.c -o build/vfs.o
clang --target=aarch64-none-elf -ffreestanding -mgeneral-regs-only -c kernel/fs/ramfs.c -o build/ramfs.o
clang --target=aarch64-none-elf -ffreestanding -mgeneral-regs-only -c kernel/interrupts.c -o build/interrupts.o
clang --target=aarch64-none-elf -ffreestanding -mgeneral-regs-only -c kernel/kalloc.c -o build/kalloc.o
clang --target=aarch64-none-elf \
  -fuse-ld=lld \
  -nostdlib \
  -Wl,-T,linker.ld \
  build/boot.o build/int.o build/kernel.o build/io.o build/timer.o build/string.o build/cmd.o build/vfs.o build/ramfs.o build/interrupts.o build/kalloc.o \
  -o build/kernel.elf
