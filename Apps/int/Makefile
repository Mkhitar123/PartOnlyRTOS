# Device parameters 
MCU =  atmega2560
BOOT_ADR  = 0x00000

# Target name
TARGET = build/int.elf
BIN    = build/int.bin

# Tools
AS = avr-as
CC = avr-gcc
OBJCOPY = avr-objcopy
SIZE = avr-size

# Compiler flags
CFLAGS = -mmcu=$(MCU) -DF_CPU=16000000UL -DBOOT_ADR=$(BOOT_ADR) -Ilib -Os -flto -fdata-sections -ffunction-sections -std=c99
ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp -DF_CPU=16000000UL 
LDFLAGS = -Wl,-Map,$(TARGET).map -Wl,--section-start=.text=$(BOOT_ADR)
BINFLAGS = -O binary -R .eeprom
LIBS =  -Wl,--whole-archive libkernel.a -Wl,--no-whole-archive

# Source files
SRCS = main.c
ASRC = 
OBJS = $(addprefix build/,$(SRCS:.c=.o)) $(addprefix build/,$(ASRC:.S=.o))

# Default target
all: $(TARGET) $(BIN)

# Compile C sources
build/%.o: %.c | build
	$(CC) $(CFLAGS) -c $< -o $@

# Compile Assembly sources
build/%.o: %.S | build
	$(CC) $(ASFLAGS) -c $< -o $@

# Create build folder
build:
	mkdir -p build

# Link ELF
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) -o $@ $(OBJS)

# Generate BIN
$(BIN): $(TARGET)
	$(OBJCOPY) $(BINFLAGS) $< $@

# Clean
clean:
	rm -rf build

# Size info
size: $(TARGET)
	$(SIZE) -t $<