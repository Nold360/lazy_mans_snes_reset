TARGET=lazy_mans_snes_reset
MCU=attiny13
SOURCES=main.c

# Change me:
PROGRAMMER=arduino
PORT=-P/dev/ttyACM0
BAUD=-B115200

# Don't change me!
OBJECTS=$(SOURCES:.c=.o)
CFLAGS=-c -Os -I /usr/lib/avr/include/
LDFLAGS=-g -mmcu=$(MCU) -Wall -Wl,-Map,$(PRG).map -L /usr/lib/avr/lib/ -l c

all: hex

hex: $(TARGET).hex

$(TARGET).hex: $(TARGET).elf
	avr-objcopy -O ihex -j .data -j .text $(TARGET).elf $(TARGET).hex

$(TARGET).elf: $(OBJECTS)
	avr-gcc $(LDFLAGS) -mmcu=$(MCU) $(OBJECTS) -o $(TARGET).elf

.c.o:
	avr-gcc $(CFLAGS) -mmcu=$(MCU) $< -o $@

size:
	avr-size --mcu=$(MCU) -C $(TARGET).elf

program:
	avrdude -p$(MCU) $(PORT) $(BAUD) -c$(PROGRAMMER) -Uflash:w:$(TARGET).hex:a

fuses:
	avrdude -p$(MCU) $(PORT) $(BAUD) -c$(PROGRAMMER) -U lfuse:w:0x7a:m -U hfuse:w:0xff:m

clean_tmp:
	rm -rf *.o
	rm -rf *.elf

clean:
	rm -rf *.o
	rm -rf *.elf
	rm -rf *.hex
