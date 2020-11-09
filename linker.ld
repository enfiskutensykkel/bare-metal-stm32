STARTUP(crt0.o);

/* 
 * Define memory addresses for flash memory (ROM)
 * and SRAM (RAM).
 */
MEMORY
{
    flash (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
    ram   (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}


/* 
 * In theory, the top of the stack should be at the end of memory.
 * Not sure why it doesn't work for higher addresses.
 */
stack_addr = 0x20005000;


SECTIONS
{
    /* ROM */
    . = 0x00000000;
    .text :
    {
        *(.vtor)    /* Vector table indicating stack address and entry point */
        *(.text*)   /* Code (_reset and all C code) */
        *(.rodata)  /* Read-only variables (const variables) */
        . = ALIGN(4);
        _text_end = .;  
    } > flash

    /* 
     * Initialized data 
     * This must be copied from ROM to RAM by the entry point.
     */
    . = 0x20000000;
    .data :
    {
        _data_start = .;
        *(.data*)

        /* Relocated interrupt vector table needs to be 128 byte aligned */
        . = ALIGN(128);
        _vtor = .;
        *(.vtor_rel)

        . = ALIGN(4);
        _data_end = .;
    } > ram AT > flash
    
    /* Uninitialized data (zero'd data) */
    .bss :
    {
        _bss_start = .;
        *(.bss)
        . = ALIGN(4);
        _bss_end = .;
    } > ram
}
