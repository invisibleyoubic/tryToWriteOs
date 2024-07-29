#!/bin/bash

OUT_DIR="bin"

rm -rf $OUT_DIR
mkdir -p $OUT_DIR

nasm boot.asm -f bin -o $OUT_DIR/boot.bin
nasm kernel_entry.asm -f elf32 -o $OUT_DIR/entry.bin

gcc -m32 -fno-stack-protector -ffreestanding -c main.c -o $OUT_DIR/kernel.o

ld -m elf_i386 -Ttext 0x1000 -o $OUT_DIR/kernel.img $OUT_DIR/entry.bin $OUT_DIR/kernel.o

objcopy -O binary -j .text $OUT_DIR/kernel.img $OUT_DIR/kernel.bin
cat $OUT_DIR/boot.bin $OUT_DIR/kernel.bin > $OUT_DIR/os-image

qemu-system-i386 -drive format=raw,file=$OUT_DIR/os-image