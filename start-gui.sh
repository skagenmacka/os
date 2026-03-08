qemu-system-aarch64 \
  -machine virt \
  -cpu cortex-a72 \
  -m 512M \
  -serial vc \
  -monitor none \
  -kernel build/kernel.elf
