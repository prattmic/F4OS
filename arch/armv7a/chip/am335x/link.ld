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

    .initializers :
    {
        . = ALIGN(4);
        __begin_early_initializer = .;
        KEEP(*(.initializers.early))
        __end_early_initializer = .;
        . = ALIGN(4);                   /* Although function pointers should always be */
        __begin_core_initializer = .;   /* word aligned, someone could erroneously add */
        KEEP(*(.initializers.core))     /* something not word sized to .initcall.* */
        __end_core_initializer = .;     /* and these otherwise useless aligns would save */
        . = ALIGN(4);                   /* you from a fault! */
        __begin_late_initializer = .;
        KEEP(*(.initializers.late))
        __end_late_initializer = .;
        . = ALIGN(4);
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

    /* Special user section reserved only for general
     * use by user programs. */
    .user :
    {
        _user_start = .;
        *(.user*)
        _user_end = .;
        . = ALIGN(8);
    } > SRAM

    /* Special section for static list of clock drivers */
    .clocks :
    {
        _clocks_start = .;
        *(.clocks*)
        _clocks_end = .;
        . = ALIGN(8);
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
