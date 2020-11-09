.cpu cortex-m3
.text
.thumb
.code 16
.syntax unified
.extern main


/* 
 * Reset handler 
 */
.thumb_func
_reset:

    // All is flashed to ROM.
    // We need to copy ROM into RAM in order to initialize variables.
    ldr     r1, data_start // address of data section
    ldr     r2, text_end   // end of text section

    // Calculate length to copy
    ldr     r3, data_end 
    subs    r3, r3, r1
    beq     init_bss        // if length = 0, skip copying

load_data:
    ldrb    r4, [r2], $1    // read byte from ROM
    strb    r4, [r1], $1    // write byte to RAM
    subs    r3, r3, $1      // decrement length
    bgt     load_data       // repeat

init_bss:
    // Initialize BSS section
    // BSS contains uninitialized variables, i.e., variables set to zero
    ldr     r1, bss_start  // address of BSS section

    // Calclulate length to zero out
    ldr     r3, bss_end
    subs    r3, r3, r1
    beq     _ok             // if length = 0, skip zeroing out

    mov     r2, $0
zero_bss:
    strb    r2, [r1], $1    // write zero byte
    subs    r3, r3, $1      // decrement length
    bgt     zero_bss        // repeat

_ok:
    // Call main
    bl main

    // Loop forever
    b  .


/*
 * Variables filled in by the linker (see the linker script)
 * We use these to calculate how much to copy
 */
text_end:      .word _text_end
data_start:    .word _data_start
data_end:      .word _data_end
bss_start:     .word _bss_start
bss_end:       .word _bss_end


/* 
 * Exceptions (standard for all Cortex-M3) 
 * See section 2.3.2 in STM32F10xxx Cortex-M3 programming manual.
 * 
 * This is loaded into address 0x00000000
 */
.section .vectors
.word stack_addr    // Top of the stack
.word _reset        // Reset handler routine


