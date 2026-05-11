# Device parameters 
MCU =  atmega2560
BOOT_ADR  = 0x3E000

# Target name
TARGET = build/main_boot.elf
HEX    = build/main_boot.hex

# Tools
AS = avr-as
CC = avr-gcc
OBJCOPY = avr-objcopy
SIZE = avr-size

# Compiler flags,  Optimize for size, Link-time optimization, Remove unused functions/data
CFLAGS = -mmcu=$(MCU) -DF_CPU=16000000UL -DBOOT_ADR=$(BOOT_ADR) -Ilib -Os -flto -fdata-sections -ffunction-sections -std=c99
ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp -DF_CPU=16000000UL 
LDFLAGS = -Wl,-Map,$(TARGET).map -Wl,--section-start=.text=$(BOOT_ADR)
HEXFLAGS = -O ihex

# Source files
SRCS = bootloader.c pff.c diskio.c usart.c spi.c
ASRC = asmfunc.S
OBJS = $(addprefix build/,$(SRCS:.c=.o)) $(addprefix build/,$(ASRC:.S=.o))
# LIBS =  -Wl,--whole-archive libkernel.a -Wl,--no-whole-archive

# Default target
all: $(TARGET) $(HEX)

# Compile c to obj
build/%.o: %.c | build
	$(CC) $(CFLAGS) -c -T bootloader.ld $< -o $@

# Compile asm to obj
build/%.o: %.S | build
	$(CC) $(ASFLAGS) -c $< -o $@

# Make build folder
build:
	mkdir -p build

# Link ELF
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

# Generate HEX
$(HEX): $(TARGET)
	$(OBJCOPY) $(HEXFLAGS) $< $@

# Clean
clean:
	rm -rf build

# Size info
size: $(TARGET)
	$(SIZE) -t $<
