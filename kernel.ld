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
_estack = 0x10010000;    /* end of 64K CCM RAM */

/* Heap */
_suserheap = 0x20000000;    /* beginning of 112K SRAM */
_euserheap = 0x2001BFFC;    /* end of 112K SRAM */
_skernelheap = _ebss;
_ekernelheap = 0x10007FFC;

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
    } > CCMRAM

    /* Uninitialized data section */
    _sibss = .;
    .bss : AT ( _sibss )
    {
        . = ALIGN(4);
        _sbss = .;         /* define a global symbol at bss start */
        *(.bss*)

        . = ALIGN(4);
    } > CCMRAM
    _ebss = .;         /* define a global symbol at bss end */
}

PROVIDE(end = .);
_end            = .;
