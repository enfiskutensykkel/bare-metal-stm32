#ifndef __STM32_F103C8_GPIO_H__
#define __STM32_F103C8_GPIO_H__

#include "register.h"
#include <stdint.h>


/*
 * General purpose I/O port (GPIO).
 * Section 9 in RM0008.
 */
REGISTER(gpio, struct {
    uint32_t crl;   // Configuration low
    uint32_t crh;   // Configuration high
    uint32_t idr;   // Input data register
    uint32_t odr;   // Output data register
    uint32_t bssr;  // Bit set/reset register
    uint32_t brr;   // Bit reset register
    uint32_t lckr;  // Lock register
});


/*
 * Available GPIO ports
 * (GPIO base addresses)
 */
#define PA  ((gpio_t) 0x40010800)
#define PB  ((gpio_t) 0x40010c00)
#define PC  ((gpio_t) 0x40011000)


#endif
