IMG := image

# Configure toolchain
CC := arm-none-eabi-gcc
AS := arm-none-eabi-as
LD := arm-none-eabi-ld
OBJCOPY := arm-none-eabi-objcopy

# Compiler flags
CFLAGS := -Wall -Wextra -pedantic
CFLAGS += -mthumb -mcpu=cortex-m3
#CFLAGS += -ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,static


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

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

