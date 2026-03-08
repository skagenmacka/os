qemu-system-aarch64 \
  -machine virt \
  -cpu cortex-a72 \
  -m 512M \
  -nographic \
  -kernel build/kernel.elf