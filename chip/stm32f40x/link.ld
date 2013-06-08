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
_estack = 0x10008000;    /* middle of 64K CCM RAM */

/* Heap */
_suserheap = 0x20000000;    /* beginning of 112K SRAM */
_euserheap = 0x2001FFFC;    /* end of 16K ETHRAM */
_skernelheap = 0x10008000;  /* Middle of 64K CCM RAM */
_ekernelheap = 0x1000FFFC;  /* End of 64K CCM RAM */

SECTIONS {
    /* Vectors first - Always permitted by MPU */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > FLASH

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

    /* Special user section reserved only for general
     * use by user programs. */
    .user :
    {
        _user_start = .;
        *(.user*)
        . = ALIGN(8);
        _user_end = .;
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
        *(COMMON)

        . = ALIGN(4);
        _ebss = .;         /* define a global symbol at bss end */
    } > CCMRAM
}

PROVIDE(end = .);
_end            = .;
