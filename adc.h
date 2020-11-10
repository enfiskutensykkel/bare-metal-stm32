#ifndef __STM32_F103C8_ADC_H__
#define __STM32_F103C8_ADC_H__

#include <stdint.h>


/*
 * Analog-to-digital converter (ADC).
 * Section 11 in STM32F103xx MCU reference manual.
 */
typedef volatile struct {
    uint32_t sr;        // Status register
    uint32_t cr1;       // Control register 1
    uint32_t cr2;       // Control register 2
    uint32_t smpr1;     // Sample time register 1
    uint32_t smpr2;     // Sample time register 2
    uint32_t jofr[4];   // Injected channel data offset register FIXME: union
    uint32_t htr;       // Watchdog high threshold
    uint32_t ltr;       // Watchdog low threshold
    uint32_t sqr1;      // Regular sequence 1
    uint32_t sqr2;      // Regular sequence 2
    uint32_t sqr3;      // Regular sequence 3
    uint32_t jsqr;      // Injected sequence 
    uint32_t jdr[4];    // Injected data FIXME: union
    uint32_t dr;        // Regular data 
}* adc_t;


/*
 * Analog to digital converter base addresses
 */
extern adc_t const ADC1;

#endif
