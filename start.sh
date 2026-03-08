qemu-system-aarch64 \
  -machine virt \
  -cpu cortex-a72 \
  -m 512M \
  -nographic \
  -serial mon:stdio \
  -monitor none \
  -kernel build/kernel.elf
