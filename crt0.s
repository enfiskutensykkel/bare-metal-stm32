.cpu cortex-m3
.thumb
.syntax unified
.extern main


/* 
 * Reset handler routine.
 * (Entry point)
 */
.text
.thumb_func
.global _reset
_reset:
    // Everything is flashed to ROM, RAM is unitialized at this point.
    // We need to copy values from ROM into RAM in order to initialize variables.
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
    // bss contains uninitialized variables, i.e., static variables 
    // that are set to zero
    ldr     r1, bss_start   // address of bss section

    // Calclulate length of section to zero out
    ldr     r3, bss_end
    subs    r3, r3, r1
    beq     relocate_vectors // if length = 0, skip zeroing out

    mov     r2, $0
zero_bss:
    strb    r2, [r1], $1    // write zero byte
    subs    r3, r3, $1      // decrement length
    bgt     zero_bss        // repeat

relocate_vectors:
    // Relocate the interrupt vector table
    // In order for software to set up custom ISRs,
    // we must relocate the vector table to RAM.
    //
    // First, set SCB_VTOR to point to the relocated vector table
    // (see section 4.4.4 in the STM32F10xxx Cortex-M3 programming manual)
    ldr     r1, vtor_addr   // Table address
    ldr     r2, scb_addr    // SCB base address
    mov     r3, $0x08       // Offset to SCB_VTOR
    str     r1, [r2, r3]

    // Then, relocate the vector table by copying each vector
    // from the original table to the new address
    mov     r1, $0x0        // original table
    ldr     r2, vtor_addr   // relocation address
    mov     r3, (_vt_end - _vt_start) / 4

relocate:
    ldr     r4, [r1], $4    // read word from ROM
    str     r4, [r2], $4    // write word to RAM
    subs    r3, r3, $1      // decrement counter
    bgt     relocate        // repeat

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
scb_addr:   .word SCB


/* 
 * Exception/interrupt vector table
 *
 * See section 2.3.2 in STM32F10xxx Cortex-M3 programming manual,
 * and section 10.2.1 in STM32F101xx MCU reference manual.
 * 
 * The first two entries are used for the stack pointer and reset handler
 * routine respectively. The vtor is loaded into address 0x00000000, which 
 * the processor then uses to initialize the stack pointer and jump to the 
 * reset vector (entry point).
 *
 * See the linker script for where this section is placed in memory.
 */
 // FIXME: use .weak and default handler so C code can override this
.section .vt
_vt_start:
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
_vt_end:


/*
 * vtor is the relocated vector table, which is a 128-byte aligned memory
 * region reserved for software to configure interrupt routines (ISRs).
 *
 * The original table needs to be copied to this section, so we reserve
 * a size equal to the size of the original vector table.
 */
.section .vtor
.fill _vt_end - _vt_start, 1, 0 // Reserve space for the relocated table.
