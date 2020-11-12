#include <stdint.h>
#include <errno.h>
#include "clock.h"

#ifndef HSE_FREQ
#define HSE_FREQ    8000000
#endif


/*
 * Flash access control register (FLASH ACR)
 * See section 3.3.3 in STM32F101xx MCU reference manual.
 *
 * Bit 4 PRFTBE: Prefetch buffer enable
 * Bit 3 HLFCYA: half cycle access enable
 * Bits 2:0 LATENCY: Latency/wait states
 *
 * Prefetch buffer must be kept on when using a prescaler different
 * from 1 on the AHB clock.
 *
 * The prefetch buffer can only be switched on/off when SYSCLK is lower than
 * 24 MHz, i.e., when initializing and the controller is running using the 
 * internal 8 HMz HSI oscillator.
 *
 * Half cycle configuration is not available in combination with a prescaler
 * on the AHB, and can only be used with a low-frequency clock of 8 MHz or less.
 * (For power optimization)
 *
 * No wait states:  0 < SYSCLK <= 24 MHz
 * 1 wait state:    24 MHz < SYSCLK <= 48 MHz
 * 2 wait states:   48 MHz < SYSCLK <= 72 MHz
 */
extern volatile uint32_t _flash;


/*
 * Set system clock (and APB1 prescale if necessary)
 */
int rcc_sysclk(enum sysclk clk)
{
    int freq = 8000000; // HSI frequency
    int hseon = 0;
    
    // Which clock are we going to use?
    switch (clk & 0x3) {
        case 0x2: // PLL
            if (clk & (1 << 16)) {
                // HSE is used as PLLSRC
                freq = HSE_FREQ;
                if (clk & (1 << 17)) {
                    // PLLXTPRE prescale HSE for PLLSRC
                    freq >>= 1;
                }
                hseon = 1;
            } else {
                // HSI/2 is used as PLLSRC
                freq >>= 1;
            }

            if (((clk >> 18) & 0xf) == 0xf) {
                freq <<= 4;
            } else {
                freq *= ((clk >> 18) & 0xf) + 2;
            }
            break;

        case 0x1: // HSE
            freq = HSE_FREQ;
            hseon = 1;
            break;

        default: // HSI
            break;
    }

    if (freq > 72000000) {
        return -EINVAL;
    }

    // Calculate the necessary flash memory wait states
    uint32_t flash_acr = (_flash & 0xffffffe0) | (1 << 4);
    if (freq >= 48000000) {
        flash_acr |= 2;
    } else if (freq >= 24000000) {
        flash_acr |= 1;
    }
    _flash = flash_acr;

    // Enable HSE clock (if necessary)
    if (hseon) {
        rcc.cr |= 1 << 16;
        
        // Wait until HSE becomes stable
        while (!(rcc.cr & (1 << 17)));
    }

    // Set clock configuration (but do not switch yet)
    rcc.cfgr |= clk & ((0xf << 18) | (1 << 17) | (1 << 16));

    // Do we need to prescale the APB1 clock?
    if (freq >= 36000000) {
        rcc.cfgr |= 1 << 10;
    }

    // Enable PLL clock (if necessary)
    if ((clk & 0x3) == 2) {
        rcc.cr |= 1 << 24;

        // Wait until PLL becomes stable
        while (!(rcc.cr & (1 << 25)));
    }

    // Select SYSCLK
    rcc.cfgr |= clk & 0x3;

    return freq;
}
