#ifndef __STM32F103C8_GPIO_H__
#define __STM32F103C8_GPIO_H__

#include <stdint.h>


/*
 * General purpose I/O port (GPIO).
 * Section 9 in STM32F103xx MCU reference manual.
 */
struct gpio 
{
    uint32_t crl;       // Configuration low FIXME: cr union
    uint32_t crh;       // Configuration high
    uint32_t idr;       // Input data register
    uint32_t odr;       // Output data register
    uint32_t bssr;      // Bit set/reset register
    uint32_t brr;       // Bit reset register
    uint32_t lckr;      // Lock register
};


/*
 * Available GPIO ports
 */
extern volatile struct gpio gpioa;  
extern volatile struct gpio gpiob; 
extern volatile struct gpio gpioc;
extern volatile struct gpio gpiod;


/* 
 * Short-hands for output ports
 */
#define out_a       gpioa.odr
#define out_b       gpiob.odr
#define out_c       gpioc.odr
#define out_d       gpiod.odr


/* 
 * Short-hands for input ports.
 */
#define in_a       gpioa.idr
#define in_b       gpiob.idr
#define in_c       gpioc.idr
#define in_d       gpiod.idr


/*
 * Different port modes for a GPIO pin.
 * See section 9.2.1 and 9.2.2 in the STM32F103xx MCU reference manual.
 * Note that lower speeds means less noise.
 */
enum gpio_mode
{
    GPIO_INPUT      = 0, // Configure pin to input mode (reset state)
    GPIO_2MHZ       = 2, // Configure pin to output mode (max 2 MHz)
    GPIO_10MHZ      = 1, // Configure pin to output mode (max 10 MHz)
    GPIO_50MHZ      = 3, // Configure pin to output mode (max 50 MHz)
};


/*
 * Different port configurations for a GPIO pin.
 * See section 9.2.1 and 9.2.2 in the STM32F103xx MCU reference manual.
 */
enum gpio_cnf
{
    GPIO_PUSHPULL       = 0, // General purpose push-pull (vcc high / ground low)
    GPIO_OPENDRAIN      = 1, // General purpose open-drain (hi-imp / ground low)
    GPIO_AFIO           = 2, // Alternate function enable
    GPIO_AFIO_PUSHPULL  = GPIO_AFIO | 0, // Alternate function push-pull
    GPIO_AFIO_OPENDRAIN = GPIO_AFIO | 1, // Alternate function open-drain
    GPIO_ANALOG         = 0, // Analog input mode
    GPIO_HIGHIMP        = 1, // Floating input (reset state, high impedance mode)
    GPIO_PULLUP         = 2, // Input with pull-up / pull-down resistor
};


/*
 * Set configuration for specified GPIO pin.
 * Returns 0 on success, and -ERRNO on failure.
 */
int gpio_cfg(volatile struct gpio* port, int pin, enum gpio_cnf cnf, enum gpio_mode mode);



/*
 * Alternate function I/O
 * Section 9.4 in STM32F103xx MCU reference manual.
 */
struct afio 
{
    uint32_t evcr;      // Event control register
    uint32_t mapr;      // Alternate function remap and debug I/O config
    uint32_t exticr[4]; // External interrupt configuration (1-4) FIXME: union
    uint32_t mapr2;     // AF remap and debug I/O config 2
};

extern volatile struct afio afio;


/*
 * External interrupt/event (EXTI)
 * See section 10.2 in STM32F103xx MCU reference manual.
 */
struct exti 
{
    uint32_t imr;       // Interrupt mask
    uint32_t emr;       // Event mask
    uint32_t rtsr;      // Rising trigger selection
    uint32_t ftsr;      // Falling trigger selection
    uint32_t swier;     // Software interrupt event
    uint32_t pr;        // Pending register
};

extern volatile struct exti exti;


/*
 * EXTI interrupt triggers.
 * See section 10.3.3 and 10.3.4 in the STM32F103xx MCU reference manual.
 */ 
enum exti_trigger
{
    EXTI_TRIGGER_RISING     = 1,
    EXTI_TRIGGER_FALLING    = 2,
    EXTI_TRIGGER_BOTH       = 3
};


/*
 * Enable EXTI interrupt on specified pin (line).
 * This will implicitly set the GPIO pin to input and enable AFIO.
 */
int exti_enable(volatile struct gpio* port, int line, enum exti_trigger trig);


// TODO: mask/unmask
#endif
