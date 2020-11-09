#ifndef __STM32_F103C8_GPIO_H__
#define __STM32_F103C8_GPIO_H__

#include "register.h"
#include <stdint.h>


/*
 * General purpose I/O port (GPIO).
 * Section 9 in STM32F103xx MCU reference manual.
 */
REGISTER(gpio, struct {
    uint32_t crl;       // Configuration low
    uint32_t crh;       // Configuration high
    uint32_t idr;       // Input data register
    uint32_t odr;       // Output data register
    uint32_t bssr;      // Bit set/reset register
    uint32_t brr;       // Bit reset register
    uint32_t lckr;      // Lock register
});

/*
 * Available GPIO ports
 * (GPIO base addresses)
 */
#define PA      ((gpio_t) 0x40010800)
#define PB      ((gpio_t) 0x40010c00)
#define PC      ((gpio_t) 0x40011000)


/*
 * Alternate function I/O
 * Section 9.4 in STM32F103xx MCU reference manual.
 */
REGISTER(afio, struct {
    uint32_t evcr;      // Event control register
    uint32_t mapr;      // Alternate function remap and debug I/O config
    uint32_t exticr1;   // External interrupt configuration 1
    uint32_t exticr2;   // External interrupt configuration 2
    uint32_t exticr3;   // External interrupt configuration 3
    uint32_t exticr4;   // External interrupt configuration 4
    uint32_t mapr2;     // AF remap and debug I/O config 2
});

#define AFIO    ((afio_t) 0x40010000)


/*
 * External interrupt/event (EXTI)
 * See section 10.2 in STM32F103xx MCU reference manual.
 */
REGISTER(exti, struct {
    uint32_t imr;       // Interrupt mask
    uint32_t emr;       // Event mask
    uint32_t rtsr;      // Rising trigger selection
    uint32_t ftsr;      // Falling trigger selection
    uint32_t swier;     // Software interrupt event
    uint32_t pr;        // Pending register
});

#define EXTI    ((exti_t) 0x40010400)

#endif
