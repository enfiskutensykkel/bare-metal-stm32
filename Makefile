IMG := image

# Configure toolchain
CC := arm-none-eabi-gcc
AS := arm-none-eabi-as
LD := arm-none-eabi-ld
GDB := arm-none-eabi-gdb
OBJCOPY := arm-none-eabi-objcopy
ARCH := -mthumb -mcpu=cortex-m3

#INTERFACE := /usr/share/openocd/scripts/interface/stlink-v2.cfg 
#TARGET := /usr/share/openocd/scripts/target/stm32f1x.cfg
#openocd -f $(INTERFACE) -f clone.cfg -f $(TARGET) -c "program image.bin verify reset exit"

# Compiler flags
CFLAGS := -Wall -Wextra -pedantic -Werror=implicit-function-declaration 
CFLAGS += -nostdlib -nostartfiles -ffreestanding
CFLAGS += -DHSE_FREQ=8000000
CFLAGS += -g

# Objects
OBJS := crt0.o main.o clock.o

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
	#openocd -c "program image.bin verify reset exit"

erase:
	st-flash erase

debug:
	-$(GDB) -tui --eval-command="target extended-remote localhost:3333" --eval-command="monitor reset halt" $(IMG).elf

# How to assemble CRT0
crt0.o: crt0.s
	$(AS) -c --warn --fatal-warnings -o $@ $< 

# How to compile source files
%.o: %.c
	$(CC) $(ARCH) $(CFLAGS) -c -o $@ $<

