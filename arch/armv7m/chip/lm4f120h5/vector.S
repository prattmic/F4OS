.syntax unified
.thumb

/* Vector table */
.section    .isr_vector,"a"
.word   _estack             /* stack top address */
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
.word   hang                /* 0 GPIO Port A */
.word   hang                /* 1 GPIO Port B */
.word   hang                /* 2 GPIO Port C */
.word   hang                /* 3 GPIO Port D */
.word   hang                /* 4 GPIO Port E */
.word   hang                /* 5 UART0 */
.word   hang                /* 6 UART1 */
.word   hang                /* 7 SSI0 */
.word   hang                /* 8 I2C0 */
.word   hang                /* 9 Reserved */
.word   hang                /* 10 Reserved */
.word   hang                /* 11 Reserved */
.word   hang                /* 12 Reserved */
.word   hang                /* 13 Reserved */
.word   hang                /* 14 ADC0 Sequence 0 */
.word   hang                /* 15 ADC0 Sequence 1 */
.word   hang                /* 16 ADC0 Sequence 2 */
.word   hang                /* 17 ADC0 Sequence 3 */
.word   hang                /* 18 Watchdog Timeers 0 and 1 */
.word   hang                /* 19 16/32-bit Timer 0A */
.word   hang                /* 20 16/32-bit Timer 0B */
.word   hang                /* 21 16/32-bit Timer 1A */
.word   hang                /* 22 16/32-bit Timer 1B */
.word   hang                /* 23 16/32-bit Timer 2A */
.word   hang                /* 24 16/32-bit Timer 2B */
.word   hang                /* 25 Analog Comparator 0 */
.word   hang                /* 26 Analog Comparator 1 */
.word   hang                /* 27 Reserved */
.word   hang                /* 28 System Control */
.word   hang                /* 29 Flash Memory and EEPROM Control */
.word   hang                /* 30 GPIO Port F */
.word   hang                /* 31 Reserved */
.word   hang                /* 32 Reserved */
.word   hang                /* 33 UART2 */
.word   hang                /* 34 SSI1 */
.word   hang                /* 35 Timer 3A */
.word   hang                /* 36 Timer 3B */
.word   hang                /* 37 I2C1 */
.word   hang                /* 38 Reserved */
.word   hang                /* 39 CAN0 */
.word   hang                /* 40 Reserved */
.word   hang                /* 41 Reserved */
.word   hang                /* 42 Reserved */
.word   hang                /* 43 Hibernation Module */
.word   hang                /* 44 USB */
.word   hang                /* 45 Reserved */
.word   hang                /* 46 uDMA Software */
.word   hang                /* 47 uDMA Error */
.word   hang                /* 48 ADC1 Sequence 0 */
.word   hang                /* 49 ADC1 Sequence 1 */
.word   hang                /* 50 ADC1 Sequence 2 */
.word   hang                /* 51 ADC1 Sequence 3 */
.word   hang                /* 52 Reserved */
.word   hang                /* 53 Reserved */
.word   hang                /* 54 Reserved */
.word   hang                /* 55 Reserved */
.word   hang                /* 56 Reserved */
.word   hang                /* 57 SSI2 */
.word   hang                /* 58 SSI3 */
.word   hang                /* 59 UART3 */
.word   hang                /* 60 UART4 */
.word   hang                /* 61 UART5 */
.word   hang                /* 62 UART6 */
.word   hang                /* 63 UART7 */
.word   hang                /* 64 Reserved */
.word   hang                /* 65 Reserved */
.word   hang                /* 66 Reserved */
.word   hang                /* 67 Reserved */
.word   hang                /* 68 I2C2 */
.word   hang                /* 69 I2C3 */
.word   hang                /* 70 16/32-bit Timer 4A */
.word   hang                /* 71 16/32-bit Timer 4B */
.word   hang                /* 72 Reserved */
.word   hang                /* 73 Reserved */
.word   hang                /* 74 Reserved */
.word   hang                /* 75 Reserved */
.word   hang                /* 76 Reserved */
.word   hang                /* 77 Reserved */
.word   hang                /* 78 Reserved */
.word   hang                /* 79 Reserved */
.word   hang                /* 80 Reserved */
.word   hang                /* 81 Reserved */
.word   hang                /* 82 Reserved */
.word   hang                /* 83 Reserved */
.word   hang                /* 84 Reserved */
.word   hang                /* 85 Reserved */
.word   hang                /* 86 Reserved */
.word   hang                /* 87 Reserved */
.word   hang                /* 88 Reserved */
.word   hang                /* 89 Reserved */
.word   hang                /* 90 Reserved */
.word   hang                /* 91 Reserved */
.word   hang                /* 92 32/64-bit Timer 5A */
.word   hang                /* 93 32/64-bit Timer 5B */
.word   hang                /* 94 32/64-bit Timer 0A */
.word   hang                /* 95 32/64-bit Timer 0B */
.word   hang                /* 96 32/64-bit Timer 1A */
.word   hang                /* 97 32/64-bit Timer 1B */
.word   hang                /* 98 32/64-bit Timer 2A */
.word   hang                /* 99 32/64-bit Timer 2B */
.word   hang                /* 100 32/64-bit Timer 3A */
.word   hang                /* 101 32/64-bit Timer 3B */
.word   hang                /* 102 32/64-bit Timer 4A */
.word   hang                /* 103 32/64-bit Timer 4B */
.word   hang                /* 104 32/64-bit Timer 5A */
.word   hang                /* 105 32/64-bit Timer 4B */
.word   hang                /* 106 System Exception */
.word   hang                /* 107 Reserved */
.word   hang                /* 108 Reserved */
.word   hang                /* 109 Reserved */
.word   hang                /* 110 Reserved */
.word   hang                /* 111 Reserved */
.word   hang                /* 112 Reserved */
.word   hang                /* 113 Reserved */
.word   hang                /* 114 Reserved */
.word   hang                /* 115 Reserved */
.word   hang                /* 116 Reserved */
.word   hang                /* 117 Reserved */
.word   hang                /* 118 Reserved */
.word   hang                /* 119 Reserved */
.word   hang                /* 120 Reserved */
.word   hang                /* 121 Reserved */
.word   hang                /* 122 Reserved */
.word   hang                /* 123 Reserved */
.word   hang                /* 124 Reserved */
.word   hang                /* 125 Reserved */
.word   hang                /* 126 Reserved */
.word   hang                /* 127 Reserved */
.word   hang                /* 128 Reserved */
.word   hang                /* 129 Reserved */
.word   hang                /* 130 Reserved */
.word   hang                /* 131 Reserved */
.word   hang                /* 132 Reserved */
.word   hang                /* 133 Reserved */
.word   hang                /* 134 Reserved */
.word   hang                /* 135 Reserved */
.word   hang                /* 136 Reserved */
.word   hang                /* 137 Reserved */
.word   hang                /* 138 Reserved */
