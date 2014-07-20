/*
 * Copyright (C) 2013, 2014 F4OS Authors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

SECTIONS {
    .vector :
    {
        . = ALIGN(8);
        KEEP(*(.vector))
        . = ALIGN(8);
    } > CONFIG_VECTOR_VMA_REGION
    AT> CONFIG_VECTOR_LMA_REGION

    .kernel :
    {
        . = ALIGN(8);
        *(.kernel*)
        . = ALIGN(8);
    } > CONFIG_KERNEL_VMA_REGION
    AT> CONFIG_KERNEL_LMA_REGION

    .text :
    {
        . = ALIGN(8);
        *(.text*)
        . = ALIGN(8);
    } > CONFIG_TEXT_VMA_REGION
    AT> CONFIG_TEXT_LMA_REGION

    .rodata :
    {
        . = ALIGN(8);
        *(.rodata*)
        . = ALIGN(8);
    } > CONFIG_RODATA_VMA_REGION
    AT> CONFIG_RODATA_LMA_REGION

    .linker_array :
    {
        KEEP(*(SORT_BY_NAME(.linker_array.*)))
    } > CONFIG_LINKER_ARRAY_VMA_REGION
    AT> CONFIG_LINKER_ARRAY_LMA_REGION

    .dtb :
    {
        . = ALIGN(8);
        _dtb_start = .;
        *(.dtb*)
        _dtb_end = .;
        . = ALIGN(8);
    } > CONFIG_DTB_VMA_REGION
    AT> CONFIG_DTB_LMA_REGION

    .data :
    {
        . = ALIGN(8);
        _data_start = .;
        *(.data*)
        _data_end = .;
        . = ALIGN(8);
    } > CONFIG_DATA_VMA_REGION
    AT> CONFIG_DATA_LMA_REGION

    _sidata = LOADADDR(.data);

    .bss :
    {
        . = ALIGN(8);
        _bss_start = .;
        *(.bss*)
        *(COMMON)
        _bss_end = .;
        . = ALIGN(8);
    } > CONFIG_BSS_VMA_REGION
    AT> CONFIG_BSS_LMA_REGION
}
