#include "irq.h"
#include "adc.h"
#include "gpio.h"
#include "clock.h"
#include <stddef.h>
#include <stdint.h>

static int red_pin = 12;
static int green_pin = 13;
static volatile uint16_t threshold; // Potentiometer threshold value


/*
 * Set configuration for specified GPIO pin.
 * See section 9.2.1 and 9.2.2
 */
static void gpio_enable(volatile struct gpio* port, int pin, int cnf, int mode)
{
    int y = pin;
    volatile uint32_t* cr = &port->crl;
    
    if (pin >= 8) {
        y = pin - 8;
        cr = &port->crh;
    }

    *cr &= ~(0xf << (y * 4));
    *cr |= (cnf << (y * 4 + 2)) | (mode << (y * 4));
}


/*
 * Read analog value.
 */
static uint16_t adc_read(volatile struct adc* adc, int channel)
{
    // One conversion 
    adc->sqr1 &= 0xff000000;

    // Select channel to read
    adc->sqr3 &= 0xc0000000;
    adc->sqr3 |= channel;

    // Start converting by setting ADON
    adc->cr2 |= 1;

    // Poll for end of conversion bit (EOC)
    while (!(adc->sr & (1 << 1)));

    // Reading the data register will clear EOC
    uint16_t data = adc->dr & 0xffff;

    // Remove some granularity from sample
    return data >> 9;
}


static void delay(uint32_t ms)
{
    const uint32_t freq = 500;

    for (uint32_t i = 0; i < ms * freq; ++i);
}


static void toggle_led()
{
    out_c = (out_c & ~(1 << 13)) | ~(out_c & (1 << 13));
}


static void flash_alternate(int n, int speed)
{
    uint32_t value = out_b & ((1 << green_pin) | (1 << red_pin));

    for (int i = 0; i < n; ++i) {
        out_b &= ~(1 << green_pin);
        out_b |= 1 << red_pin;
        delay(speed);
        out_b &= ~(1 << red_pin);
        out_b |= 1 << green_pin;
        delay(speed);
    }

    out_b |= value;
}


static void flash_both(int n, int speed)
{
    uint32_t value = out_b & ((1 << green_pin) | (1 << red_pin));

    for (int i = 0; i < n; ++i) {
        out_b &= ~((1 << green_pin) | (1 << red_pin));
        delay(speed);
        out_b |= (1 << green_pin) | (1 << red_pin);
        delay(speed);
    }

    out_b |= value;
}


static void adc_calibrate(volatile struct adc* adc)
{
    adc->cr2 |= 1 << 3;
    while (adc->cr2 & (1 << 3));
    adc->cr2 |= 1 << 2;
    while (adc->cr2 & (1 << 2));
}


static void button_swap()
{
    flash_alternate(4, 500);

    int tmp = green_pin;
    green_pin = red_pin;
    red_pin = tmp;

    exti.pr |= 1;
}


static void button_reset()
{
    flash_both(6, 500);
    threshold = adc_read(&adc1, 0);
    exti.pr |= 2;
}


/*
 * Enable external line interrupt.
 */
static void exti_init(volatile struct gpio* port, int line)
{
    // Enable input on pin
    gpio_enable(port, line, 2, 0);

    // Set external interrupt configuration
    // See section 9.4.3 - 9.4.6
    uint32_t exticr = 0;
    if (port == &gpioa) {
        exticr = 0x0;
    } else if (port == &gpiob) {
        exticr = 0x1;
    } else if (port == &gpioc) {
        exticr = 0x2;
    } else {
        // what are you doing?
        return;
    }

    afio.exticr[line / 4] |= exticr << (line * 4);

    // Set trigger selection (rising)
    // section 10.3.3
    exti.rtsr |= 1 << line;

    // Unmask EXTI interrupts
    exti.imr |= 1 << line;
}



int main()
{
    rcc_sysclk(CLK_HSE, 9);

    // Enable port clocks (PA + PB + PC)
    rcc.apb2enr |= (1 << 4) | (1 << 3) | (1 << 2);

    // Enable ADC1 clock
    rcc.apb2enr |= 1 << 9;

    // Power on ADC by setting ADON
    adc1.cr2 |= 1;

    // Enable AFIO clock for EXTI interrupts
    rcc.apb2enr |= 1;

    // Enable input on PA0
    gpio_enable(&gpioa, 0, 0, 0);

    // Enable LEDs
    gpio_enable(&gpiob, red_pin, 0, 2);
    gpio_enable(&gpiob, green_pin, 0, 2);
    gpio_enable(&gpioc, 13, 0, 2);

    // After a reset, the ADC requires calibration
    adc_calibrate(&adc1);

    // Set up custom EXTI interrupt vectors
    irq_set_handler(IRQ_EXTI0, button_reset);
    irq_set_handler(IRQ_EXTI1, button_swap);

    // Set interrupt priorities (I have no idea what I'm doing here)
    irq_set_priority(IRQ_EXTI0, 1);
    irq_set_priority(IRQ_EXTI1, 2);

    // Set up EXTI line interrupts for pins
    exti_init(&gpiob, 0);
    exti_init(&gpiob, 1);

    // Take initial sample
    threshold = adc_read(&adc1, 0);

    // Enable interrupts
    irq_enable(IRQ_EXTI0);
    irq_enable(IRQ_EXTI1);

    while (1) {
        int value = (1 << red_pin) | (1 << green_pin);

        uint16_t sample = adc_read(&adc1, 0);
        if (sample < threshold) {
            value = 1 << red_pin;
        } else if (sample > threshold) {
            value = 1 << green_pin;
        }

        out_b = value;

        delay(500);
        toggle_led();
    }
}
