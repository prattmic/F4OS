/* Linker script for F4OS 
 * Michael Pratt <michael@pratt.im> */

/* Specify the memory areas */
MEMORY
{
  FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 1024K
  RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 112K
  ETHRAM (xrw)    : ORIGIN = 0x2001C000, LENGTH = 16K
  CCMRAM (xrw)    : ORIGIN = 0x10000000, LENGTH = 64K
}

/* Highest address of the kernel stack */
_estack = 0x1000FFFF;    /* end of 64K CCM RAM */

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
        _stext = .;
        *(.text*)
        . = ALIGN(8);

        _etext = .;
    } > FLASH

    /* .kernel comes after .text because it will be set to privileged only, and we don't want
     * to overreach with the MPU and ban user .text */
    .kernel :
    {
        . = ALIGN(4);
        _skernel = .;
        *(.kernel*)
        . = ALIGN(8);
        _ekernel = .;
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
