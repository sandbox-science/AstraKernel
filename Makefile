OUT_DIR = build/
SRC_DIR = src/

CROSS_COMPILE = arm-none-eabi-
AS     = $(CROSS_COMPILE)gcc
CC     = $(CROSS_COMPILE)gcc
LD     = $(CROSS_COMPILE)ld
OBJCOPY= $(CROSS_COMPILE)objcopy

CFLAGS = -ffreestanding -nostdlib -nostartfiles -O2 -Wall -Wextra
LDFLAGS = -T $(SRC_DIR)kernel.ld

all: clean kernel.bin qemu

start.o: $(SRC_DIR)start.S
	@mkdir -p $(OUT_DIR)
	$(AS) -c $< -o $(OUT_DIR)$@

kernel.o: $(SRC_DIR)kernel.c
	$(CC) $(CFLAGS) -c $< -o $(OUT_DIR)$@

kernel.elf: start.o kernel.o $(SRC_DIR)kernel.ld
	$(LD) $(LDFLAGS) $(OUT_DIR)start.o $(OUT_DIR)kernel.o -o $(OUT_DIR)$@

kernel.bin: kernel.elf
	$(OBJCOPY) -O binary $(OUT_DIR)$< $(OUT_DIR)$@

clean:
	rm -f $(OUT_DIR)*.o $(OUT_DIR)*.elf $(OUT_DIR)*.bin

qemu:
	@echo "Press Ctrl-A and then X to exit QEMU"
	@echo
	@qemu-system-arm -M versatilepb -nographic -kernel $(OUT_DIR)kernel.bin