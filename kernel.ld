/* Linker script for F4OS 
 * Michael Pratt <michael@pratt.im> */

/* Specify the memory areas */
MEMORY
{
  FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 1024K
  RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 112K
  ETHRAM (xrw)    : ORIGIN = 0x2001C000, LENGTH = 16K
}

/* Highest address of the user mode stack */
_estack = 0x20020000;    /* end of 128K RAM on AHB bus*/

SECTIONS {
    /* Vectors first */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > FLASH

    .text :
    {
        . = ALIGN(4);
        *(.text*)
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
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;         /* define a global symbol at bss start */
        *(.bss*)

        . = ALIGN(4);
        _ebss = .;         /* define a global symbol at bss end */
    } > RAM

}

PROVIDE(end = .);
_end            = .;
