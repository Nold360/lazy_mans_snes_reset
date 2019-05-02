# Lazy Man's SNES Reset

"Lazy Man's SNES Reset" is nothing more, than an attiny microcontroller, sniffing the SNES controller port for a certain key combination. [Default: Start + Select + R].

When this button combination is pressed, the microcontroller will set the *RESET-pin* to "high" for a few milli seconds. Is this pin correctly connected to the SNES's reset button, it will trigger a (hard-)reset of the console.

This repository contains everything you need to build your own & become lazy!



# Prerequirements
## Hardware
 - ATtiny13(a)
 - 10k ohm resistor
 - Some kind of ISP-Programmer (e.g. Arduino + ArduinoISP)
 - Some wire, soldering iron, SNES (duh..)

## Software
 - Clone git repository & switch to directory using: 
```
git clone https://gitlab.com/nold360/lazy_mans_snes_reset.git
cd lazy_mans_snes_reset
```

 - Install some kind of ISP flashing software. In this example we're going to use "[avrdude](https://www.nongnu.org/avrdude/)"
 - This was tested on Linux only!

You can also just download the precompiled hex-file & flash it using your prefered software: [Download](https://github.com/Nold360/lazy_mans_snes_reset)

**Important:** Don't forget to set the fuses correctly!


# Flash the attiny [Required]
First we need to burn the LMSR-Firmware to the microcontroller.

## Flashing precompiled hex-file
 - Connect attiny to your ISP programmer
 - Connect ISP to PC
 - Flash hex-file to attiny using: **make program** (or your prefered flashing software)
 - Program attiny fuses using: **make fuses** (or your prefered flashing software)




# Building the Sourcecode [Optional]
This step is optinal and just needed, if you want to modify the source code. Tested on Linux only.

## Reqired Software
 - avr-gcc 
 - avr-libc
 - make
 - git

## Build
```
make
```

If succeeded, you should have a new .hex-file & you're ready to flash your attiny.



# Soldering the attiny into the SNES
**NOTICE:** This guide has been created to the best of my knowledge and certain. But without any guarantee of correctness! **Make sure that you know what you are doing**, as I'm **not** responsible for any damage or harm you might do to your SNES or yourself!



## Attiny Pinout




## SNES Junior
