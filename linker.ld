/* STARTUP(crt0.o); */
ENTRY(vectors)

MEMORY
{
    flash (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
    ram   (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

SECTIONS
{
    .text :
    {
        *(.vectors)
        *(.text*)
        /* *(.rodata) */
    } > flash

    /*
    .bss :
    {
        *(.bss)
    } > ram
    */
}
