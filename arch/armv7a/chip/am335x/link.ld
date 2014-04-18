/* Linker script for F4OS
 * Michael Pratt <michael@pratt.im> */

/* Specify the memory areas */
MEMORY
{
  SRAM (xrw)    : ORIGIN = 0x402F0400, LENGTH = 63K
  OCMC_RAW (xrw): ORIGIN = 0x40300000, LENGTH = 64K
}

/* Heap */
_suserheap = CONFIG_SUSERHEAP;
_euserheap = CONFIG_EUSERHEAP;
_skernelheap = CONFIG_SKERNELHEAP;
_ekernelheap = CONFIG_EKERNELHEAP;

SECTIONS {
    /* Vectors first */
    .vector :
    {
        . = ALIGN(4);
        KEEP(*(.vector))
        . = ALIGN(4);
    } > SRAM

    .kernel :
    {
        . = ALIGN(8);
        _skernel = .;
        *(.kernel*)
        . = ALIGN(8);
        _ekernel = .;
    } > SRAM

    .text :
    {
        . = ALIGN(8);
        _text_start = .;

        *(.text*)

        . = ALIGN(8);
        _text_end = .;
    } > SRAM

    .rodata :
    {
        . = ALIGN(8);
        *(.rodata*)
        . = ALIGN(8);
    } > SRAM

    .linker_array :
    {
        KEEP(*(SORT_BY_NAME(.linker_array.*)))
    } > SRAM

    .dtb :
    {
        . = ALIGN(8);
        _dtb_start = .;
        *(.dtb*)
        _dtb_end = .;
        . = ALIGN(8);
    } > SRAM

    .data :
    {
        . = ALIGN(4);

        _data_start = .;
        *(.data*)

        . = ALIGN(4);
        _data_end = .;
    } > SRAM

    /* Uninitialized data section */
    .bss :
    {
        . = ALIGN(4);
        _bss_start = .;

        *(.bss*)
        *(COMMON)

        . = ALIGN(4);
        _bss_end = .;
    } > SRAM
}

PROVIDE(end = .);
_end            = .;
