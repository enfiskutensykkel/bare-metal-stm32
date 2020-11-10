#ifndef __STM32_F103C8_GPIO_H__
#define __STM32_F103C8_GPIO_H__

#include <stdint.h>


/*
 * General purpose I/O port (GPIO).
 * Section 9 in STM32F103xx MCU reference manual.
 */
typedef volatile struct {
    uint32_t crl;       // Configuration low FIXME: cr union
    uint32_t crh;       // Configuration high
    uint32_t idr;       // Input data register
    uint32_t odr;       // Output data register
    uint32_t bssr;      // Bit set/reset register
    uint32_t brr;       // Bit reset register
    uint32_t lckr;      // Lock register
}* gpio_t;

/*
 * Available GPIO ports
 * (GPIO base addresses)
 */
extern gpio_t const PA;
extern gpio_t const PB;
extern gpio_t const PC;


/*
 * Alternate function I/O
 * Section 9.4 in STM32F103xx MCU reference manual.
 */
typedef volatile struct {
    uint32_t evcr;      // Event control register
    uint32_t mapr;      // Alternate function remap and debug I/O config
    uint32_t exticr[4]; // External interrupt configuration (1-4) FIXME: union
    uint32_t mapr2;     // AF remap and debug I/O config 2
}* afio_t;

extern afio_t const AFIO;


/*
 * External interrupt/event (EXTI)
 * See section 10.2 in STM32F103xx MCU reference manual.
 */
typedef volatile struct {
    uint32_t imr;       // Interrupt mask
    uint32_t emr;       // Event mask
    uint32_t rtsr;      // Rising trigger selection
    uint32_t ftsr;      // Falling trigger selection
    uint32_t swier;     // Software interrupt event
    uint32_t pr;        // Pending register
}* exti_t;

extern exti_t const EXTI;

#endif
