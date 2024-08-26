OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

MEMORY
{
    ROM (rx) : ORIGIN = 0x1000, LENGTH = 10M   /* Adjust as needed */
    RAM (rwx) : ORIGIN = 0x200000, LENGTH = 40M  /* Adjust as needed */
}

SECTIONS
{
    . = 0x1000;

    .entry : {
        *(.entry)
    } > ROM

    .idt : {
        *(.idt)
    } > ROM

    .text : {
        *(.text)
        *(.text*)
    } > ROM

    .data : {
        *(.data)
        *(.data*)
    } > RAM

    .bss : {
        *(.bss)
        *(.bss*)
        *(COMMON)
    } > RAM

    .stack : {
        . = ALIGN(0x1000);
        _stack_start = .;
        . += 0x10000;
        _stack_end = .;
    } > RAM

    _end = .;
}