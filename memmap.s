.cpu cortex-m3
.thumb
.syntax unified
.section .rodata
//.align 4

// Export symbols
.global SCB, NVIC, SYSTICK
.global RCC
.global EXTI
.global AFIO
.global ADC1
.global PA      // GPIOA
.global PB      // GPIOB
.global PC      // GPIOC


/* 
 * Pointers to Cortex-M3 registers.
 * See section 4 in the STM32F10xxx Cortex-M3 programming manual.
 */
NVIC:   .word 0xe000e100
STK:    .word 0xe000e010
SCB:    .word 0xe000ed00

/*
 * Pointers to STM32 registers.
 * See section 3.3 in STM32F103xx MCU reference manual and 
 */
RCC:    .word 0x40021000
EXTI:   .word 0x40010400
AFIO:   .word 0x40010000
PA:     .word 0x40010800
PB:     .word 0x40010c00
PC:     .word 0x40011000
ADC1:   .word 0x40012400
