TARGET=SNES_IGR_LED
MCU=attiny13
SOURCES=main.c ws2812b_attiny13.c

# Change me:
PROGRAMMER=pi

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
 
dump:
	avrdude -p$(MCU) -C /home/pi/avrdude_gpio.conf -c$(PROGRAMMER) -U flash:r:flashdump.hex:i

dumpeeprom:
	avrdude -p$(MCU) -C /home/pi/avrdude_gpio.conf -c$(PROGRAMMER) -U eeprom:r:EEPROM.hex:i
 
programdump:
	avrdude -p$(MCU) -C /home/pi/avrdude_gpio.conf -c$(PROGRAMMER) -U flash:w:flashdump.hex:a

program:
	avrdude -p$(MCU) -C /home/pi/avrdude_gpio.conf -c$(PROGRAMMER) -U flash:w:$(TARGET).hex:a

eeprom:
	avrdude -p$(MCU) -C /home/pi/avrdude_gpio.conf -c$(PROGRAMMER) -U eeprom:w:$(TARGET)_EEPROM.hex:a

programall:
	avrdude -p$(MCU) -C /home/pi/avrdude_gpio.conf -c$(PROGRAMMER) -U flash:w:$(TARGET).hex:a
	avrdude -p$(MCU) -C /home/pi/avrdude_gpio.conf -c$(PROGRAMMER) -U eeprom:w:$(TARGET)_EEPROM.hex:a
	avrdude -p$(MCU) -C /home/pi/avrdude_gpio.conf -c$(PROGRAMMER) -U lfuse:w:0x7a:m -U hfuse:w:0xff:m

fuses:
	avrdude -p$(MCU) -C /home/pi/avrdude_gpio.conf -c$(PROGRAMMER) -U lfuse:w:0x7a:m -U hfuse:w:0xff:m

clean_tmp:
	rm -rf *.o
	rm -rf *.elf

clean:
	rm -rf *.o
	rm -rf *.elf
	rm -rf *.hex
