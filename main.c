#include "adc.h"
#include "gpio.h"
#include "rcc.h"
#include "m3.h"
#include <stddef.h>
#include <stdint.h>

static int red_pin = 12;
static int green_pin = 13;
static volatile uint16_t threshold; // Potentiometer threshold value


/*
 * Set configuration for specified GPIO pin.
 * See section 9.2.1 and 9.2.2
 */
static void gpio_enable(gpio_t port, int pin, int cnf, int mode)
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
static uint16_t adc_read(adc_t adc, int channel)
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
    return data >> 4;
}

static void delay(uint32_t x)
{
    for (uint32_t i = 0; i < x; ++i);
}


static void toggle_led()
{
    PC->odr = (PC->odr & ~(1 << 13)) | ~(PC->odr & (1 << 13));
}


static void flash_alternate(int n, int speed)
{
    uint32_t value = PB->odr & ((1 << green_pin) | (1 << red_pin));

    for (int i = 0; i < n; ++i) {
        PB->odr &= ~(1 << green_pin);
        PB->odr |= 1 << red_pin;
        delay(speed);
        PB->odr &= ~(1 << red_pin);
        PB->odr |= 1 << green_pin;
        delay(speed);
    }

    PB->odr |= value;
}


static void flash_both(int n, int speed)
{
    uint32_t value = PB->odr & ((1 << green_pin) | (1 << red_pin));

    for (int i = 0; i < n; ++i) {
        PB->odr &= ~((1 << green_pin) | (1 << red_pin));
        delay(speed);
        PB->odr |= (1 << green_pin) | (1 << red_pin);
        delay(speed);
    }

    PB->odr |= value;
}


static void adc_calibrate(adc_t adc)
{
    adc->cr2 |= 1 << 2;
    while (!(adc->cr2 & (1 << 2)));
}


static void button_swap()
{
    flash_alternate(4, 70000);

    int tmp = green_pin;
    green_pin = red_pin;
    red_pin = tmp;

    EXTI->pr |= 1;
}


static void button_reset()
{
    flash_both(6, 25000);
    threshold = adc_read(ADC1, 0);
    EXTI->pr |= 2;
}


static void irq_enable(irqno_t irq)
{
    NVIC->iser[irq / 32] = 1 << (irq % 32);
}


/*
 * Enable external line interrupt.
 */
static void exti_init(gpio_t port, int line)
{
    // Enable input on pin
    gpio_enable(port, line, 2, 0);

    // Set external interrupt configuration
    // See section 9.4.3 - 9.4.6
    uint32_t exticr = 0;
    if (port == PA) {
        exticr = 0x0;
    } else if (port == PB) {
        exticr = 0x1;
    } else if (port == PC) {
        exticr = 0x2;
    } else {
        // what are you doing?
        return;
    }

    AFIO->exticr[line / 4] |= exticr << (line * 4);

    // Set trigger selection (rising)
    // section 10.3.3
    EXTI->rtsr |= 1 << line;

    // Unmask EXTI interrupts
    EXTI->imr |= 1 << line;
}


int main()
{
    // Enable port clocks (PA + PB + PC)
    RCC->apb2enr |= (1 << 4) | (1 << 3) | (1 << 2);

    // Enable ADC1 clock
    RCC->apb2enr |= 1 << 9;

    // Power on ADC by setting ADON
    ADC1->cr2 |= 1;

    // Enable AFIO clock for EXTI interrupts
    RCC->apb2enr |= 1;

    // Enable input on PA0
    gpio_enable(PA, 0, 0, 0);

    // Enable LEDs
    gpio_enable(PB, red_pin, 0, 2);
    gpio_enable(PB, green_pin, 0, 2);
    gpio_enable(PC, 13, 0, 2);

    // After a reset, the ADC requires calibration
    adc_calibrate(ADC1);

    // Set up custom EXTI interrupt vectors
    vtor_set_isr(IRQ_EXTI0, button_reset);
    vtor_set_isr(IRQ_EXTI1, button_swap);

    // Set interrupt priorities (I have no idea what I'm doing here)
    NVIC->ipr[IRQ_EXTI0] = 0x10;
    NVIC->ipr[IRQ_EXTI1] = 0x20;

    // Set up EXTI line interrupts for pins
    exti_init(PB, 0);
    exti_init(PB, 1);

    // Take initial sample
    threshold = adc_read(ADC1, 0);

    // Enable interrupts
    irq_enable(IRQ_EXTI0);
    irq_enable(IRQ_EXTI1);

    while (1) {
        int value = (1 << red_pin) | (1 << green_pin);

        uint16_t sample = adc_read(ADC1, 0);
        if (sample < threshold) {
            value = 1 << red_pin;
        } else if (sample > threshold) {
            value = 1 << green_pin;
        }

        PB->odr = value;

        delay(200000);
        toggle_led();
    }
}
