.cpu cortex-m3
.text
.thumb
.code 16
.syntax unified
.extern main


/* 
 * Entry point
 */
.thumb_func
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
    // Relocate the IRQ vector table
    mov     r1, $0x0        // original table
    ldr     r2, vtor_addr   // relocation address
    mov     r3, $76         // 76 vectors must be moved

copy_vtor:
    ldr     r4, [r1], $1    // read word from ROM
    str     r4, [r2], $1    // write word to RAM
    subs    r3, r3, $1      // decrement counter
    bgt     copy_vtor       // repeat

    // Set SCB_VTOR to point to the relocated vector table
    ldr     r1, vtor_addr   // Table address
    ldr     r2, scb         // SCB base address
    mov     r3, $0x08       // Offset to SCB_VTOR
    str     r1, [r2, r3]

    // Call main
    mov     r0, $0  // argc = 0
    mov     r1, $0  // argv = NULL
    bl      main

    // Main should not return, but in case
    // it does we loop forever
    b       .


/*
 * Base address of the system control block (SCB)
 * We point the SCB to the relocated vector table by setting SCB_VTOR.
 * See section 4.4 in the STM3210xxx Cortex-M3 programming manual.
 */
scb:        .word 0xe000ed00
vtor_addr:  .word _vtor

/*
 * Addresses filled in by the linker (see the linker script)
 * We use these to calculate the size of each section:
 *   - text section, containing the code and read-only variables
 *   - data section, containing initialized data and 
 *     the relocated vector table (vtor_rel)
 *   - bss section, containing uninitialized data (that is zero'd out)
 */
text_end:   .word _text_end
data_start: .word _data_start 
data_end:   .word _data_end   
bss_start:  .word _bss_start
bss_end:    .word _bss_end


/* 
 * Exceptions (standard for all Cortex-M3) 
 * See section 2.3.2 in STM32F10xxx Cortex-M3 programming manual,
 * and section 10.2.1 in STM32F101xx MCU reference manual.
 *
 * vtor is loaded into address 0x00000000, which the processor
 * then use to initialize the stack and jump to the reset vector (entry point).
 * The reset handler then initializes data and bss segments, before copying
 * the original vtor to a new address.
 */
.section .vtor
.word stack_addr    // Top of the stack
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
.fill 60, 4, 0      // IRQs

/*
 * vtor_rel, is the relocated vector table, which is a 128-byte aligned memory
 * region reserved for software to configure interrupt routines.
 * The original vtor will be copied to this address.
 */
.section .vtor_rel
.fill 76, 4, 0

