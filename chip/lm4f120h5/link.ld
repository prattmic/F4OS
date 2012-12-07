/* Linker script for F4OS 
 * Michael Pratt <michael@pratt.im> */

/* Specify the memory areas */
MEMORY
{
  FLASH (rx)      : ORIGIN = 0x00000000, LENGTH = 256K
  RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 32K
}

/* Highest address of the kernel stack */
_estack = 0x20001FFC;       /* quarter of 32K SRAM */

/* Heap */
_suserheap = 0x20004000;    /* middle of 32K SRAM */
_euserheap = 0x20008000;    /* end of 32K SRAM */
_skernelheap = 0x20002000;  /* quarter of 32K SRAM */
_ekernelheap = 0x20004000;  /* middle of 32K SRAM */

SECTIONS {
    /* Vectors first - Always permitted by MPU */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > FLASH

    /* .kernel comes before .text so .text can be aligned to its
     * size and avoid any MPU effects */
    .kernel :
    {
        . = ALIGN(8);
        _skernel = .;
        *(.kernel*)
        . = ALIGN(8);
        _ekernel = .;
    } > FLASH

    .text :
    {
        . = ALIGN(8);
        _stext = .;
        *(.text*)
        *(.rodata*)
        . = ALIGN(8);

        _etext = .;
    } > FLASH

    _sidata = .;
    .data : AT ( _sidata )
    {
        . = ALIGN(4);
        _sdata = .;        /* create a global symbol at data start */
        *(.data*)           /* .data sections */

        . = ALIGN(4);
        _edata = .;        /* define a global symbol at data end */
    } > RAM

    /* Uninitialized data section */
    _sibss = .;
    .bss : AT ( _sibss )
    {
        . = ALIGN(4);
        _sbss = .;         /* define a global symbol at bss start */
        *(.bss*)
        *(COMMON)

        . = ALIGN(4);
        _ebss = .;         /* define a global symbol at bss end */
    } > RAM
}

PROVIDE(end = .);
_end            = .;
