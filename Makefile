IMG := image

# Configure toolchain
CC := arm-none-eabi-gcc
AS := arm-none-eabi-as
LD := arm-none-eabi-ld
OBJCOPY := arm-none-eabi-objcopy
ARCH := -mthumb -mcpu=cortex-m3

# Compiler flags
CFLAGS := -Wall -Wextra -pedantic -Werror=implicit-function-declaration 
#CFLAGS += -nostdlib -nostartfiles -ffreestanding

# Objects
OBJS := crt0.o memmap.o main.o

# Targets
.PHONY: all clean flash erase
all: $(IMG).bin

$(IMG).bin: $(IMG).elf
	$(OBJCOPY) -O binary $< $@

$(IMG).elf: linker.ld $(OBJS)
	$(LD) -T linker.ld -o $@ $(OBJS) 

clean:
	-$(RM) $(OBJS) $(IMG).elf $(IMG).bin

flash: $(IMG).bin
	st-flash --reset write $< 0x08000000

erase:
	st-flash erase

# How to assemble CRT0
crt0.o: crt0.s
	$(AS) -c --warn --fatal-warnings -o $@ $< 

# How to assemble memory layout
memmap.o: memmap.s
	$(AS) -c -o $@ $<

# How to compile source files
%.o: %.c
	$(CC) $(ARCH) $(CFLAGS) -c -o $@ $<

