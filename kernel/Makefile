MCU = atmega2560
CC  = avr-gcc
AR  = avr-gcc-ar
SIZE = avr-size

CFLAGS = -mmcu=$(MCU) -DF_CPU=16000000UL -Os -ffreestanding -std=c99 \
         -ffunction-sections -fdata-sections -flto

SOURCES = kernel.c usart.c
OBJECTS = $(SOURCES:.c=.o)
LIB     = libkernel.a

all: $(LIB)

$(LIB): $(OBJECTS)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(LIB)

size:
	$(SIZE) -t $(LIB)
