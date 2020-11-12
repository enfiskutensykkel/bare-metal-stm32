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
 * Different clock signals used for the system clock (SYSCLK).
 *
 * See section 7.2 in the MCU reference manual, particularly 
 * Figure 8. Clock tree.
 *
 * There are two high-speed clocks, the internal oscillator (HSI) and
 * using an external clock (HSE).
 *
 * HSI: After system reset, HSI internal oscilator (8 MHz) is used
 *      as system clock. While faster startup time, it is less accurate 
 *      than an external crystal or ceramic oscillator.
 *
 * HSE: We can use the high-speed external clock (HSE) from an external
 *      crystal instead. In our case, the crystal is 8 MHz, but more stable
 *      signal than the internal HSI.
 *
 * It is possible to use the PLL to multiply the clock signal frequency:
 *    PLLSRC -> PLLMUL (x2, x3, x4, ..., x16) -> PLLCLK -> SYSCLK
 * 
 * Here, PLLSRC can is either HSI/2 or HSE. HSE can also be prescaled to 
 * HSE/2 when used as PLLSRC.
 *
 * Note that the clock speed can not exceed 72 MHz.
 */
#define _PLL(mul, pre, src) \
    ((!!(pre) << 17) | (!!(src) << 16) | ((((mul) - 2) & 0x0f) << 18) | 2)
enum sysclk
{
    SYSCLK_HSI_1    = 0,            // Internal oscillator (HSI=8 MHz)
    SYSCLK_HSI_1_5  = _PLL( 3,0,0), // HSI/2 *  3 = 12 MHz
    SYSCLK_HSI_2    = _PLL( 4,0,0), // HSI/2 *  4 = 16 MHz
    SYSCLK_HSI_2_5  = _PLL( 5,0,0), // HSI/2 *  5 = 20 MHz
    SYSCLK_HSI_3    = _PLL( 6,0,0), // HSI/2 *  6 = 24 MHz
    SYSCLK_HSI_3_5  = _PLL( 7,0,0), // HSI/2 *  7 = 28 MHz
    SYSCLK_HSI_4    = _PLL( 8,0,0), // HSI/2 *  8 = 32 MHz
    SYSCLK_HSI_4_5  = _PLL( 9,0,0), // HSI/2 *  9 = 36 MHz
    SYSCLK_HSI_5    = _PLL(10,0,0), // HSI/2 * 10 = 40 MHz
    SYSCLK_HSI_5_5  = _PLL(11,0,0), // HSI/2 * 11 = 44 MHz
    SYSCLK_HSI_6    = _PLL(12,0,0), // HSI/2 * 12 = 48 MHz
    SYSCLK_HSI_6_5  = _PLL(13,0,0), // HSI/2 * 13 = 52 MHz
    SYSCLK_HSI_7    = _PLL(14,0,0), // HSI/2 * 14 = 56 MHz
    SYSCLK_HSI_7_5  = _PLL(15,0,0), // HSI/2 * 15 = 60 MHz
    SYSCLK_HSI_8    = _PLL(16,0,0), // HSI/2 * 16 = 64 MHz

    SYSCLK_HSE_1    = 1,            // External oscillator (HSE=8 MHz)
    SYSCLK_HSE_1_5  = _PLL( 3,1,1), // HSE/2 *  3 = 12 MHz
    SYSCLK_HSE_2    = _PLL( 2,0,1), // HSE   *  2 = 16 MHz
    SYSCLK_HSE_2_5  = _PLL( 5,1,1), // HSE/2 *  5 = 20 MHz
    SYSCLK_HSE_3    = _PLL( 3,0,1), // HSE   *  3 = 24 MHz
    SYSCLK_HSE_3_5  = _PLL( 7,1,1), // HSE/2 *  7 = 28 MHz
    SYSCLK_HSE_4    = _PLL( 4,0,1), // HSE   *  4 = 32 MHz
    SYSCLK_HSE_4_5  = _PLL( 9,1,1), // HSE/2 *  9 = 36 MHz
    SYSCLK_HSE_5    = _PLL( 5,0,1), // HSE   *  5 = 40 MHz
    SYSCLK_HSE_5_5  = _PLL(11,1,1), // HSE/2 * 11 = 44 MHz
    SYSCLK_HSE_6    = _PLL( 6,0,1), // HSE   *  6 = 48 MHz
    SYSCLK_HSE_6_5  = _PLL(13,1,1), // HSE/2 * 13 = 52 MHz
    SYSCLK_HSE_7    = _PLL( 7,0,1), // HSE   *  7 = 56 MHz
    SYSCLK_HSE_7_5  = _PLL(15,1,1), // HSE/2 * 15 = 60 MHz
    SYSCLK_HSE_8    = _PLL( 8,0,1), // HSE   *  8 = 64 MHz
    SYSCLK_HSE_9    = _PLL( 9,0,1), // HSE   *  9 = 72 MHz
};
#undef _PLL


/*
 * Set system clock (SYSCLK) 
 *
 * If the clock frequency exceeds 36 MHz, this function will also
 * prescale the APB1 clock.
 *
 * This function returns the clock frequency on success, and -ERRNO
 * on failure. The HSE_FREQ macro should be set, in order to indicate
 * the speed of the HSE crystal if it differs from 8 MHz.
 */
int rcc_sysclk(enum sysclk clk);

#endif
