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
.word   hang                /* 7 RESERVED */
.word   hang                /* 8 RESERVED */
.word   hang                /* 9 RESERVED*/
.word   hang                /* 10 RESERVED */
.word   _svc_asm            /* 11 SVCall */
.word   hang                /* 12 Debug Monitor */
.word   hang                /* 13 RESERVED */
.word   _pendsv             /* 14 PendSV */
.word   systick_handler     /* 15 SysTick */
/* NVIC */
.word   hang                /* 0 Window Watchdog */
.word   hang                /* 1 PVD through EXTI */
.word   hang                /* 2 Tamper and Timestamp through EXTI */
.word   hang                /* 3 RTC Wakeup through EXTI */
.word   hang                /* 4 Flash Global */
.word   hang                /* 5 RCC Global */
.word   hang                /* 6 EXTI Line 0 */
.word   hang                /* 7 EXTI Line 1 */
.word   hang                /* 8 EXTI Line 2 */
.word   hang                /* 9 EXTI Line 3 */
.word   hang                /* 10 EXTI Line 4 */
.word   hang                /* 11 DMA1 Stream 0 */
.word   hang                /* 12 DMA1 Stream 1 */
.word   hang                /* 13 DMA1 Stream 2 */
.word   hang                /* 14 DMA1 Stream 3 */
.word   hang                /* 15 DMA1 Stream 4 */
.word   hang                /* 16 DMA1 Stream 5 */
.word   hang                /* 17 DMA1 Stream 6 */
.word   hang                /* 18 ADC1, 2, 3 */
.word   hang                /* 19 CAN1 TX */
.word   hang                /* 20 CAN1 RX0 */
.word   hang                /* 21 CAN1 RX1 */
.word   hang                /* 22 CAN1 SCE */
.word   hang                /* 23 EXTI Line[9:5] */
.word   hang                /* 24 TIM1 Break and TIM9 Global */
.word   hang                /* 25 TIM1 Update and TIM10 Global */
.word   hang                /* 26 TIM1 Trigger and Commutation and TIM11 Global */
.word   hang                /* 27 TIM1 Capture Compare */
.word   hang                /* 28 TIM2 Global */
.word   hang                /* 29 TIM3 Global */
.word   hang                /* 30 TIM4 Global */
.word   hang                /* 31 I2C1 Event */
.word   hang                /* 32 I2C1 Error */
.word   hang                /* 33 I2C2 Event */
.word   hang                /* 34 I2C2 Error */
.word   hang                /* 35 SPI1 Global */
.word   hang                /* 36 SPI2 Global  */
.word   hang                /* 37 USART1 Global */
.word   hang                /* 38 USART2 Global */
.word   hang                /* 39 USART3 Global */
.word   hang                /* 40 EXTI Line[15:10] */
.word   hang                /* 41 RTC Alarms A/B through EXTI */
.word   hang                /* 42 USB OTG FS Wakeup through EXTI */
.word   hang                /* 43 TIM8 Break and TIM12 Global */
.word   hang                /* 44 TIM8 Update and TIM13 Global */
.word   hang                /* 45 TIM8 Trigger and Commutation and TIM14 Global */
.word   hang                /* 46 TIM8 Capture Compare */
.word   hang                /* 47 DMA1 Stream 7 */
.word   hang                /* 48 FSMC Global */
.word   hang                /* 49 SDIO Global */
.word   hang                /* 50 TIM5 Global */
.word   hang                /* 51 SPI3 Global */
.word   hang                /* 52 UART4 Global */
.word   hang                /* 53 UART5 Global */
.word   hang                /* 54 TIM6 Global and DAC1/2 Underrun Error */
.word   hang                /* 55 TIM7 Global */
.word   hang                /* 56 DMA2 Stream 0 */
.word   hang                /* 57 DMA2 Stream 1 */
.word   hang                /* 58 DMA2 Stream 2 */
.word   hang                /* 59 DMA2 Stream 3 */
.word   hang                /* 60 DMA2 Stream 4 */
.word   hang                /* 61 Ethernet Global */
.word   hang                /* 62 Ethernet Wakeup through EXTI */
.word   hang                /* 63 CAN2 TX */
.word   hang                /* 64 CAN2 RX0 */
.word   hang                /* 65 CAN2 RX1 */
.word   hang                /* 66 CN2 SCE */
#ifdef CONFIG_HAVE_USBDEV
.word   usbdev_handler      /* 67 USB OTG FS Global Interrupt */
#else
.word   hang                /* 67 USB OTG FS Global Interrupt */
#endif
.word   hang                /* 68 DMA2 Stream 5 */
.word   hang                /* 69 DMA2 Stream 6 */
.word   hang                /* 70 DMA2 Stream 7 */
.word   hang                /* 71 USART6 Global */
.word   hang                /* 72 I2C3 Event */
.word   hang                /* 73 I2C3 Error */
.word   hang                /* 74 USB OTG HS EP1 Out Global */
.word   hang                /* 75 USB OTG HS EP1 In Global */
.word   hang                /* 76 USB OTG HS Wakeup through EXTI */
.word   hang                /* 77 USB OTG HS Global */
.word   hang                /* 78 DCMI Global */
.word   hang                /* 79 CRYP Global */
.word   hang                /* 80 Hash and TNG Global */
.word   hang                /* 81 FPU Global */
