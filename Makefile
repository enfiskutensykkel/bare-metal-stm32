IMG := image

# Configure toolchain
CC := arm-none-eabi-gcc
AS := arm-none-eabi-as
LD := arm-none-eabi-ld
OBJCOPY := arm-none-eabi-objcopy
ARCH := -mthumb -mcpu=cortex-m3

# Compiler flags
CFLAGS := -Wall -Wextra -pedantic -Werror=implicit-function-declaration


# Objects
OBJS := crt0.o main.o

# Targets
.PHONY: all clean flash erase
all: $(IMG).img

$(IMG).img: $(IMG).elf
	$(OBJCOPY) -O binary $< $@

$(IMG).elf: linker.ld $(OBJS)
	$(LD) -T linker.ld -o $@ $(OBJS)

clean:
	-$(RM) $(OBJS) $(IMG).elf $(IMG).img

flash: $(IMG).img
	st-flash --reset write $< 0x08000000

erase:
	st-flash erase

# How to assemble CRT0
crt0.o: crt0.s
	$(AS) -o $@ $<

# How to compile source files
%.o: %.c
	$(CC) $(ARCH) $(CFLAGS) -c -o $@ $<

