#!/bin/sh

CROSSPREFIX=arm-none-eabi-

${CROSSPREFIX}gcc -O2 -nostdlib -march=armv4t -mthumb-interwork -T gba.ld -DTARGET_GBA -o main.o head.S main.c -lgcc && \
${CROSSPREFIX}objcopy -O binary main.o main.bin && \
${CROSSPREFIX}gcc -O2 -nostdlib -march=armv4t -mthumb-interwork -T boot.ld -o output.o boot.S && \
${CROSSPREFIX}objcopy -O binary output.o output.gba

