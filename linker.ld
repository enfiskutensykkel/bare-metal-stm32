/* 
 * Define memory addresses for flash memory (ROM)
 * and SRAM (RAM).
 */
MEMORY
{
    flash (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
    ram   (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}


/* How to place sections in memory */
SECTIONS
{
    /* Flash memory */
    . = 0x00000000;
    .text :
    {
        KEEP(*(.vt)) /* Vector table indicating stack address and entry point */
        *(.text*)    /* Code (_reset and all C code) */
        *(.rodata)   /* Read-only variables (const variables) */

        . = ALIGN(4);
        PROVIDE(_text_end = .);
    } > flash

    /* 
     * Initialized data 
     * This must be copied from ROM to RAM by the entry point.
     */
    . = 0x20000000;
    .data :
    {
        PROVIDE(_data_start = .);
        *(.data*)

        /* Relocated interrupt vector table needs to be 128 byte aligned 
         * See section 4.4.4 in the STM32F10xxx Cortex-M3 programming manual.
         */
        . = ALIGN(128);
        PROVIDE(_vtor_addr = .);
        KEEP(*(.vtor))

        . = ALIGN(4);
        PROVIDE(_data_end = .);
    } > ram AT > flash
    
    /* Uninitialized data (zero'd data) */
    .bss :
    {
        PROVIDE(_bss_start = .);
        *(.bss)
        . = ALIGN(4);
        PROVIDE(_bss_end = .);
    } > ram
}


/* 
 * In theory, the top of the stack should be at the end of memory.
 * Not sure why it doesn't work for higher addresses.
 */
PROVIDE(_stack_addr = 0x20005000);


/*
 * Memory map of Cortex-M3 and STM32 registers.
 * We define them here, so that C code may use them as global symbols.
 *
 * See section 4 in the STM32F10xxx Cortex-M3 programming manual, and
 * section 3.3 in STM32F103xx MCU reference manual.
 */
STK     = 0xe000e010;
NVIC    = 0xe000e100;
SCB     = 0xe000ed00;

AFIO    = 0x40010000;
EXTI    = 0x40010400;
PA      = 0x40010800; /* GPIOA */
PB      = 0x40010c00; /* GPIOB */
PC      = 0x40011000; /* GPIOC */
PD      = 0x40011400; /* GPIOD */

ADC1    = 0x40012400;
ADC2    = 0x40012800;
ADC3    = 0x40013c00;

RCC     = 0x40021000;
