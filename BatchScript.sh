#!/bin/bash

# Compile the game using the devkitPro toolchain
devkitARM/bin/arm-none-eabi-gcc -mthumb -mthumb-interwork -specs=gba.specs -o game.elf game.c

# Convert the binary to a GBA ROM format
devkitARM/bin/arm-none-eabi-objcopy -O binary game.elf game.gba

# Run the game using an emulator such as VisualBoy Advance
visualboyadvance game.gba
