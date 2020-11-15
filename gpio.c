#include <stdint.h>
#include <errno.h>
#include "gpio.h"


/*
 * Set configuration for specified GPIO pin.
 * See section 9.2.1 and 9.2.2
 */
int gpio_cfg(volatile struct gpio* port, int pin, enum gpio_cnf cnf, enum gpio_mode mode)
{
    int y = pin;
    volatile uint32_t* cr = &port->crl;

#ifndef NDEBUG
    // Do some sanity checking
    if (!(0 <= pin && pin <= 15)) {
        return -EINVAL;
    } 

    if (mode == GPIO_INPUT && cnf >= 3) {
        return -EINVAL;
    } else if (mode > 3 || cnf > 3) {
        return -EINVAL;
    }
#endif
    
    if (pin >= 8) {
        y = pin - 8;
        cr = &port->crh;
    }

    // Mask away previous configurations
    *cr &= ~(0xf << (y * 4));

    // Set configuration
    *cr |= (cnf << (y * 4 + 2)) | (mode << (y * 4));

    return 0;
}


/*
 * Enable external line interrupt.
 * See section 10.2 and 10.3. in STM32F03xx MCU reference manual.
 */
int exti_enable(volatile struct gpio* port, int line, enum exti_trigger trig)
{
    // Set external interrupt configuration
    // See section 9.4.3 - 9.4.6
    uint32_t exticr = 0;
    if (port == &gpioa) {
        exticr = 0x0;
    } else if (port == &gpiob) {
        exticr = 0x1;
    } else if (port == &gpioc) {
        exticr = 0x2;
    } else if (port == &gpiod) {
        exticr = 0x3;
    } else {
        return -EINVAL;
    }

    // Enable input on pin
    gpio_cfg(port, line, GPIO_PULLUP, GPIO_INPUT);

    // Configure AFIO exti
    afio.exticr[line / 4] &= ~(0xf << (line * 4)); 
    afio.exticr[line / 4] |= exticr << (line * 4);

    // Set trigger selection (rising)
    // section 10.3.3
    exti.rtsr &= ~(1 << line);
    exti.rtsr |= (trig & 1) << line;

    // Set trigger selection (falling)
    // section 10.3.4
    exti.ftsr &= ~(1 << line);
    exti.ftsr |= (trig & 2) << line;

    // Unmask EXTI interrupts
    // TODO: move this to separate function
    exti.imr |= 1 << line;

    return 0;
}

