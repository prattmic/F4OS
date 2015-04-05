/*
 * Copyright (C) 2015 F4OS Authors
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

.syntax unified
.thumb

/* Vector table */
.section    .vector,"a"
.word   CONFIG_INITIAL_SP   /* stack top address */
.word   _reset              /* 1 Reset */
.word   hang                /* 2 NMI */
.word   hardfault_handler   /* 3 HardFault */
.word   memmanage_handler   /* 4 MemManage */
.word   busfault_handler    /* 5 BusFault */
.word   usagefault_handler  /* 6 UsageFault */
.word   hang                /* 7 Reserved */
.word   hang                /* 8 Reserved */
.word   hang                /* 9 Reserved*/
.word   hang                /* 10 Reserved */
.word   _svc_asm            /* 11 SVCall */
.word   hang                /* 12 Debug Monitor */
.word   hang                /* 13 Reserved */
.word   _pendsv             /* 14 PendSV */
.word   systick_handler     /* 15 SysTick */
/* Interrupts */
.word   hang                /* 0 FIXME */
.word   hang                /* 1 FIXME */
.word   hang                /* 2 FIXME */
.word   hang                /* 3 FIXME */
.word   hang                /* 4 FIXME */
.word   hang                /* 5 FIXME */
.word   hang                /* 6 FIXME */
.word   hang                /* 7 FIXME */
.word   hang                /* 8 FIXME */
.word   hang                /* 9 FIXME */
.word   hang                /* 10 FIXME */
.word   hang                /* 11 FIXME */
.word   hang                /* 12 FIXME */
.word   hang                /* 13 FIXME */
.word   hang                /* 14 FIXME */
.word   hang                /* 15 FIXME */
.word   hang                /* 16 FIXME */
.word   hang                /* 17 FIXME */
.word   hang                /* 18 FIXME */
.word   hang                /* 19 FIXME */
.word   hang                /* 20 FIXME */
.word   hang                /* 21 FIXME */
.word   hang                /* 22 FIXME */
.word   hang                /* 23 FIXME */
.word   hang                /* 24 FIXME */
.word   hang                /* 25 FIXME */
.word   hang                /* 26 FIXME */
.word   hang                /* 27 FIXME */
.word   hang                /* 28 FIXME */
.word   hang                /* 29 FIXME */
.word   hang                /* 30 FIXME */
.word   hang                /* 31 FIXME */
.word   hang                /* 32 FIXME */
.word   hang                /* 33 FIXME */
.word   hang                /* 34 FIXME */
.word   hang                /* 35 FIXME */
.word   hang                /* 36 FIXME */
.word   hang                /* 37 FIXME */
.word   hang                /* 38 FIXME */
.word   hang                /* 39 FIXME */
.word   hang                /* 40 FIXME */
.word   hang                /* 41 FIXME */
.word   hang                /* 42 FIXME */
.word   hang                /* 43 FIXME */
.word   hang                /* 44 FIXME */
.word   hang                /* 45 FIXME */
.word   hang                /* 46 FIXME */
.word   hang                /* 47 FIXME */
.word   hang                /* 48 FIXME */
.word   hang                /* 49 FIXME */
.word   hang                /* 50 FIXME */
.word   hang                /* 51 FIXME */
.word   hang                /* 52 FIXME */
.word   hang                /* 53 FIXME */
.word   hang                /* 54 FIXME */
.word   hang                /* 55 FIXME */
.word   hang                /* 56 FIXME */
.word   hang                /* 57 FIXME */
.word   hang                /* 58 FIXME */
.word   hang                /* 59 FIXME */
.word   hang                /* 60 FIXME */
.word   hang                /* 61 FIXME */
.word   hang                /* 62 FIXME */
.word   hang                /* 63 FIXME */
.word   hang                /* 64 FIXME */
.word   hang                /* 65 FIXME */
.word   hang                /* 66 FIXME */
.word   hang                /* 67 FIXME */
.word   hang                /* 68 FIXME */
.word   hang                /* 69 FIXME */
.word   hang                /* 70 FIXME */
.word   hang                /* 71 FIXME */
.word   hang                /* 72 FIXME */
.word   hang                /* 73 FIXME */
.word   hang                /* 74 FIXME */
.word   hang                /* 75 FIXME */
.word   hang                /* 76 FIXME */
.word   hang                /* 77 FIXME */
.word   hang                /* 78 FIXME */
.word   hang                /* 79 FIXME */
.word   hang                /* 80 FIXME */
.word   hang                /* 81 FIXME */
.word   hang                /* 82 FIXME */
.word   hang                /* 83 FIXME */
.word   hang                /* 84 FIXME */
.word   hang                /* 85 FIXME */
.word   hang                /* 86 FIXME */
.word   hang                /* 87 FIXME */
.word   hang                /* 88 FIXME */
.word   hang                /* 89 FIXME */
.word   hang                /* 90 FIXME */
.word   hang                /* 91 FIXME */
.word   hang                /* 92 FIXME */
.word   hang                /* 93 FIXME */
.word   hang                /* 94 FIXME */
.word   hang                /* 95 FIXME */
.word   hang                /* 96 FIXME */
.word   hang                /* 97 FIXME */
.word   hang                /* 98 FIXME */
.word   hang                /* 99 FIXME */
.word   hang                /* 100 FIXME */
.word   hang                /* 101 FIXME */
.word   hang                /* 102 FIXME */
.word   hang                /* 103 FIXME */
.word   hang                /* 104 FIXME */
.word   hang                /* 105 FIXME */
.word   hang                /* 106 FIXME */
.word   hang                /* 107 FIXME */
.word   hang                /* 108 FIXME */
.word   hang                /* 109 FIXME */
.word   hang                /* 110 FIXME */
.word   hang                /* 111 FIXME */
.word   hang                /* 112 FIXME */
.word   hang                /* 113 FIXME */
.word   hang                /* 114 FIXME */
.word   hang                /* 115 FIXME */
.word   hang                /* 116 FIXME */
.word   hang                /* 117 FIXME */
.word   hang                /* 118 FIXME */
.word   hang                /* 119 FIXME */
.word   hang                /* 120 FIXME */
.word   hang                /* 121 FIXME */
.word   hang                /* 122 FIXME */
.word   hang                /* 123 FIXME */
.word   hang                /* 124 FIXME */
.word   hang                /* 125 FIXME */
.word   hang                /* 126 FIXME */
.word   hang                /* 127 FIXME */
.word   hang                /* 128 FIXME */
.word   hang                /* 129 FIXME */
.word   hang                /* 130 FIXME */
.word   hang                /* 131 FIXME */
.word   hang                /* 132 FIXME */
.word   hang                /* 133 FIXME */
.word   hang                /* 134 FIXME */
.word   hang                /* 135 FIXME */
.word   hang                /* 136 FIXME */
.word   hang                /* 137 FIXME */
.word   hang                /* 138 FIXME */
