arm-linux-gcc -O2 -Wall -nostdlib -march=armv4 -Wl,-T,ipl.lds uart.c ipl.c brd.c sdr.c nfc.c nand.c arm.s -o ipl.exe
arm-linux-objcopy -Obinary ipl.exe ipl.bin
