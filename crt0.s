.cpu cortex-m3
.thumb
.syntax unified
.extern main


/* 
 * Memory map of Cortex-M3 and STM32 registers.
 * We define them here, so that C code may use them as global variables.
 *
 * See section 4 in the STM32F10xxx Cortex-M3 programming manual, and
 * section 3.3 in STM32F103xx MCU reference manual.
 */
.section .rodata
.global SCB, NVIC, STK
NVIC:   .word 0xe000e100
STK:    .word 0xe000e010
SCB:    .word 0xe000ed00

.global RCC
RCC:    .word 0x40021000

.global EXTI
EXTI:   .word 0x40010400

.global AFIO
AFIO:   .word 0x40010000

.global PA, PB, PC, pD
PA:     .word 0x40010800
PB:     .word 0x40010c00
PC:     .word 0x40011000
PD:     .word 0x40011400

.global ADC1, ADC2, ADC3
ADC1:   .word 0x40012400
ADC2:   .word 0x40012800
ADC3:   .word 0x40013c00


/* 
 * Reset handler routine.
 * (Entry point)
 */
.text
.thumb_func
.global _reset
_reset:
    // Everything is flashed to ROM, RAM is unitialized at this point.
    // We need to copy ROM into RAM in order to initialize variables.
    ldr     r1, data_start // address of data section
    ldr     r2, text_end   // end of text section

    // Calculate the length of the data section
    ldr     r3, data_end 
    subs    r3, r3, r1
    beq     init_bss        // if length = 0, skip copying

load_data:
    ldrb    r4, [r2], $1    // read byte from ROM
    strb    r4, [r1], $1    // write byte to RAM
    subs    r3, r3, $1      // decrement length
    bgt     load_data       // repeat

init_bss:
    // Initialize bss section
    // bss contains uninitialized variables, i.e., variables set to zero
    ldr     r1, bss_start  // address of bss section

    // Calclulate length of section to zero out
    ldr     r3, bss_end
    subs    r3, r3, r1
    beq     relocate_vtor   // if length = 0, skip zeroing out

    mov     r2, $0
zero_bss:
    strb    r2, [r1], $1    // write zero byte
    subs    r3, r3, $1      // decrement length
    bgt     zero_bss        // repeat

relocate_vtor:
    // Set SCB_VTOR to point to the relocated vector table
    // See section 4.4.4 in the STM32F10xxx Cortex-M3 programming manual.
    ldr     r1, vtor_addr   // Table address
    ldr     r2, scb_addr    // SCB base address
    mov     r3, $0x08       // Offset to SCB_VTOR
    str     r1, [r2, r3]

    // Relocate the IRQ vector table
    mov     r1, $0x0        // original table
    ldr     r2, vtor_addr   // relocation address
    mov     r3, (_vtor_end - _vtor_start) / 4

copy_vtor:
    ldr     r4, [r1], $4    // read word from ROM
    str     r4, [r2], $4    // write word to RAM
    subs    r3, r3, $1      // decrement counter
    bgt     copy_vtor       // repeat

    // Call main
    mov     r0, $0  // argc = 0
    mov     r1, $0  // argv = NULL
    bl      main

    // Main should not return, but in case
    // it does we loop forever
    b       .


/*
 * Addresses filled in by the linker (see the linker script).
 *
 * We use these to calculate the size of each section:
 *   - text section contains the code and read-only variables
 *
 *   - data section contains initialized data (and also vtor_rel)
 *     that needs to be copied from ROM
 *
 *   - bss section contains uninitialized data that needs to 
 *     be zero'd out
 */
text_end:   .word _text_end
data_start: .word _data_start 
data_end:   .word _data_end   
bss_start:  .word _bss_start
bss_end:    .word _bss_end
vtor_addr:  .word _vtor_addr 
scb_addr:   .word 0xe000ed00 // Not sure why I can't use SCB here


/* 
 * Exception/interrupt vector table (vtor)
 *
 * See section 2.3.2 in STM32F10xxx Cortex-M3 programming manual,
 * and section 10.2.1 in STM32F101xx MCU reference manual.
 * 
 * The first two entries are used for the stack pointer and reset handler
 * routine respectively. The vtor is loaded into address 0x00000000, which 
 * the processor then uses to initialize the stack pointer and jump to the 
 * reset vector (entry point).
 */
.section .vtor
_vtor_start:
.word _stack_addr   // Top of the stack
.word _reset        // Reset handler routine (entry point)
.word 0             // Non-maskable interrupt
.word 0             // Hard fault
.word 0             // Memory fault
.word 0             // Bus fault
.word 0             // Usage fault
.word 0             // Reserved
.word 0             // Reserved
.word 0             // Reserved
.word 0             // Reserved
.word 0             // Supervisor call (SVCall)
.word 0             // Debug monitor
.word 0             // Reserved
.word 0             // PendSV
.word 0             // SysTick
.fill 60, 4, 0      // Interrupts (IRQs)
_vtor_end:


/*
 * vtor_rel is the relocated vector table, which is a 128-byte aligned memory
 * region reserved for software to configure interrupt routines.
 *
 * The original vtor needs to be copied to the address of this section, so
 * we reserve a size equal to the size of the original vtor table.
 */
.section .vtor_rel
.fill _vtor_end - _vtor_start, 1, 0
