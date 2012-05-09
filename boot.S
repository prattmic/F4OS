/* F4OS boot loader.
   Everything starts here!

   Michael Pratt <michael@pratt.im> */


/* Compile with unified syntax and thumb code, that is what the Cortex-M4 supports */
.syntax unified
.thumb 

.cpu cortex-m4
.arch armv7e-m
.fpu fpv4-sp-d16


/* Reset_Handler
   For now, just infinite loop */
.section .text.Reset_Handler
.weak Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
    B _start

/* Entry point.  Let's see if we can even get here. */
.section .text.boot
.weak _start
.type _start, %function
_start:
    B _start

.weak Default_Handler
.type Default_Handler, %function
Default_Handler:
InfiniteLoop:
    B InfiniteLoop
