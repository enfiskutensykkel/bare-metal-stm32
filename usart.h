#ifndef __STM32F103C8T6_USART_H__
#define __STM32F103C8T6_USART_H__

#include <stdint.h>


/*
 * Universal synchronous/asynchronous receiver/transmitter (USART)
 * Section 27 in the STM32F103xx MCU reference manual.
 */
struct usart
{
    uint32_t sr;        // Status register
    uint32_t dr;        // Data register
    uint32_t brr;       // Baud rate
    uint32_t cr1;       // Control register 1
    uint32_t cr2;       // Control register 2
    uint32_t cr3;       // Control register 3
    uint32_t gtpr;      // Guard time and prescaler
};


/*
 * Available USARTs
 */ 
extern volatile struct usart usart2;

#endif
