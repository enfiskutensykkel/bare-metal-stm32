#include <stdint.h>
#include <errno.h>
#include "clock.h"

#define HSI_FREQ    8000000

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
 * TODO rcc_sysclk_hse(frequency)
 *      figure out correct multiplier by dividing by HSE_CLK
 */
int rcc_sysclk(enum clksrc source, int multiple)
{
    int freq = HSE_FREQ * multiple;
    int ppre1 = 0;
    int pllxptre = 0;
    int pllsrc = 1;
    int pllmul = 0;
    int sw = 1;

    if (!(1 < multiple && multiple <= 16)) {
        return -EINVAL;
    }

    if (multiple > 1) {
        sw = 2;
        pllmul = multiple - 1;
    }

    switch (source) {
        case CLK_HSI:
            freq = HSI_FREQ;
            sw = 0;
            if (multiple > 1) {
                return -EINVAL;
            }
            break;

        case CLK_HSE:
            freq = HSE_FREQ * multiple;
            break;

        case CLK_HSI_DIV:
            freq = HSI_FREQ / 2 * multiple;
            pllsrc = 0;
            break;

        case CLK_HSE_DIV:
            freq = HSE_FREQ / 2 * multiple;
            pllxptre = 1;
            break;

        default:
            return -EINVAL;
    }

    uint32_t flash_acr = (_flash & 0xffffffe0) | 1 << 4;;
    if (freq >= 48000000) {
        flash_acr |= 2;
    } else if (freq >= 24000000) {
        flash_acr |= 1;
    }

    if (freq > 36000000) {
        ppre1 = 1;
    }

    // Set necessary flash states
    _flash = flash_acr;

    // Enable high-speed clock (HSE)
    if (sw) {
        rcc.cr |= 1 << 16;
        while (!(rcc.cr & (1 << 17)));
    }

    // Set clock multiplication factor (PLLMUL)
    rcc.cfgr |= pllmul << 18;

    // Set HSE divisor
    rcc.cfgr |= pllxptre << 17;

    // Set PLL SRC
    rcc.cfgr |= pllsrc << 16;

    // Set APB1 prescale
    rcc.cfgr |= !!ppre1 << 10;

    // Enable PLL
    if (sw == 2) {
        rcc.cr |= 1 << 24;
        while (!(rcc.cr & (1 << 25)));
    }

    // Select SYSCLK
    rcc.cfgr |= sw;

    return freq;
}
