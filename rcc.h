#ifndef __STM32_F103C8_RCC_H__
#define __STM32_F103C8_RCC_H__

#include "register.h"
#include <stdint.h>


/*
 * Reset and clock control (RCC).
 * Section 8 in RM0008.
 */
REGISTER(rcc, struct {
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
    uint32_t cfgr2;     // Clock configuration register 2
});


/*
 * Reset and clock control base address.
 */
#define RCC     ((rcc_t) 0x40021000)

#endif
