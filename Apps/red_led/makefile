MCU = atmega2560

TARGET = build/Led.elf
BIN = build/Led.bin
MAP = build/Led.map   

CC = avr-gcc
OBJCOPY = avr-objcopy

CFLAGS = -mmcu=$(MCU) -DF_CPU=16000000UL -Os -std=c99 \
         -fdata-sections -ffunction-sections \
         -fno-jump-tables -mrelax -Ilib

LDFLAGS = -mmcu=$(MCU) \
          -Wl,--section-start=.text=0x00000 \
          -Wl,--gc-sections \
          -Wl,-Map=$(MAP)

# BIN flags
BINFLAGS = -O binary -R .eeprom


SRCS_C = led.c usart.c
OBJS_C = $(addprefix build/,$(SRCS_C:.c=.o))
OBJS_ASM = $(addprefix build/,$(SRCS_ASM:.S=.o))
OBJS = $(OBJS_C) $(OBJS_ASM)

all: $(BIN)

build:
	mkdir -p build

build/%.o: %.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build/%.o: %.S | build
	$(CC) $(CFLAGS) -x assembler-with-cpp -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

# Generate BIN in build/ folder
$(BIN): $(TARGET)
	$(OBJCOPY) $(BINFLAGS) $< $@

clean:
	rm -rf build

size:
	$(SIZE) -t $(TARGET)

.PHONY: all clean size