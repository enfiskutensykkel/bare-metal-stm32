#ifndef __STM32F103C8_CLOCK_H__
#define __STM32F103C8_CLOCK_H__

#include <stdint.h>


/*
 * Reset and clock control (RCC).
 * Section 7 in STM32F103xx MCU reference manual.
 */
struct rcc 
{
    uint32_t cr;        // Clock control
    uint32_t cfgr;      // Clock configuration register
    uint32_t cir;       // Clock interrupt register
    uint32_t apb2rstr;  // APB2 peripheral reset register
    uint32_t apb1rstr;  // APB2 peripheral reset register
    uint32_t ahbenr;    // AHB peripheral clock enable
    uint32_t apb2enr;   // APB2 peripheral clock enable
    uint32_t apb1enr;   // APB2 peripheral clock enable
    uint32_t bdcr;      // Backup domain control
    uint32_t csr;       // Control/status register
    uint32_t ahbrstr;   // AHB peripheral clock resgister
};

extern volatile struct rcc rcc;



/*
 * Different clocks that can be used for the source clock signal.
 *
 * See section 7.2 in the MCU reference manual, particularly 
 * Figure 8. Clock tree.
 *
 * HSI: After system reset, HSI internal oscilator (8 MHz) is used
 *      as system clock. While faster startup time, it is less accurate 
 *      than an external crystal or ceramic oscillator.
 *
 * HSE: We can use the high-speed external clock (HSE) from an external
 *      crystal instead, which in our case is also 8 MHz, but more stable
 *      signal. 
 *
 * PLLCLK: We can use the PLL to multiply the clock signal frequency:
 *         PLLSRC -> PLLMUL (x2, x3, x4, ..., x16) -> PLLCLK
 *         Here, PLLSRC can be HSE, HSE/2 or HSI/2.
 */
enum clksrc
{
    CLK_HSI = 0,    // The internal oscillator as source (SYSCLK=8 MHz)
    CLK_HSE,        // External oscillator as source (SYSCLK=HSE * multiple)
    CLK_HSI_DIV,    // HSI/2 as source (SYSCLK=HSI/2 * multiple)
    CLK_HSE_DIV     // HSE/2 as source (SYSCLK=HSE/2 * multiple)
};


/*
 * Set system clock (SYSCLK) 
 *
 * Note that the clock speed can not exceed 72 MHz.
 *
 * multiple must be in range [1, 16]
 */
int rcc_sysclk(enum clksrc source, int multiple);

#endif
