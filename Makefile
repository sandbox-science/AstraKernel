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

# Compiler flags
INC_DIRS   := -I./include
ARCH_FLAGS := -march=armv5te -mcpu=arm926ej-s -marm

CFLAGS := -ffreestanding -nostdlib -nostartfiles \
          $(ARCH_FLAGS) -O2 -Wall -Wextra \
          -fno-builtin \
          $(INC_DIRS)

LDFLAGS := -T kernel.ld -nostdlib --build-id=none

# Tell make to look for .c in these dirs:
VPATH := $(SRC_DIRS)

all: clean kernel.bin qemu

# Assembly start.o goes to build/
$(OUT_DIR)start.o: kernel/start.s
	@mkdir -p $(OUT_DIR)
	$(AS) -c $< -o $@

# Pattern rule for any .c → build/*.o
$(OUT_DIR)%.o: %.c
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Link everything
$(OUT_DIR)kernel.elf: $(OUT_DIR)start.o $(OBJS) kernel.ld
	$(LD) $(LDFLAGS) $(OUT_DIR)start.o $(OBJS) -o $@

# Binary and others unchanged
kernel.bin: $(OUT_DIR)kernel.elf
	$(OBJCOPY) -O binary $< $(OUT_DIR)$@

clean:
	rm -f $(OUT_DIR)*.o $(OUT_DIR)*.elf $(OUT_DIR)*.bin

qemu:
	@echo "Press Ctrl-A then X to exit QEMU"
	@qemu-system-arm -M versatilepb -nographic -kernel $(OUT_DIR)kernel.bin

docker:
	docker build -t "astra-kernel" .
	docker run -it --rm "astra-kernel"

docker-dev:
	docker build -f Dockerfile.dev -t "astra-kernel-dev" .
	docker run -it --rm "astra-kernel-dev"

.PHONY: all clean qemu docker
