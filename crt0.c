#include <stddef.h>
#include <stdint.h>

#define STACK_ADDR  0x20005000

extern int main(void);


/*
 * Reset vector routine
 */
__attribute__((noreturn)) static void reset(void)
{
    main();

    // FIXME halt or power off instead
    while (1); 
}


/*
 * Vector table
 */
uint32_t vectors[84] __attribute__((section(".vectors"))) =
{
    STACK_ADDR,
    (uint32_t) reset,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

