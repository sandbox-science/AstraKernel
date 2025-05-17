OUT_DIR    := build/
SRC_DIRS   := kernel user

# 1) Find every .c in those dirs
SRCS       := $(foreach d,$(SRC_DIRS),$(wildcard $(d)/*.c))
# 2) Strip their paths and map .c → build/*.o
OBJS       := $(patsubst %.c,$(OUT_DIR)%.o,$(notdir $(SRCS)))

CROSS_COMPILE := arm-none-eabi-
AS            := $(CROSS_COMPILE)gcc
CC            := $(CROSS_COMPILE)gcc
LD            := $(CROSS_COMPILE)ld
OBJCOPY       := $(CROSS_COMPILE)objcopy

CFLAGS      := -ffreestanding -nostdlib -nostartfiles -O2 -Wall -Wextra
LDFLAGS     := -T kernel/kernel.ld

# Tell make to look for .c in these dirs:
VPATH       := $(SRC_DIRS)

all: clean kernel.bin qemu

# Assembly start.o goes to build/
$(OUT_DIR)start.o: kernel/start.S
	@mkdir -p $(OUT_DIR)
	$(AS) -c $< -o $@

# Pattern rule for any .c → build/*.o
$(OUT_DIR)%.o: %.c
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Link everything
$(OUT_DIR)kernel.elf: $(OUT_DIR)start.o $(OBJS) kernel/kernel.ld
	$(LD) $(LDFLAGS) $(OUT_DIR)start.o $(OBJS) -o $@

# Binary and others unchanged
kernel.bin: $(OUT_DIR)kernel.elf
	$(OBJCOPY) -O binary $< $(OUT_DIR)$@

clean:
	rm -f $(OUT_DIR)*.o $(OUT_DIR)*.elf $(OUT_DIR)*.bin

qemu:
	@echo "Press Ctrl-A then X to exit QEMU"
	@qemu-system-arm -M versatilepb -nographic -kernel $(OUT_DIR)kernel.bin
