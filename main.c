#include "adc.h"
#include "gpio.h"
#include "rcc.h"
#include "sys.h"
#include <stddef.h>
#include <stdint.h>

static int red_pin = 12;
static int green_pin = 13;


/*
 * Stick the relocated vector table into its own section.
 */
uint32_t vtor[NUM_IRQ + OFFS_IRQ] __attribute__((section(".vtor")));


/*
 * Set configuration for specified GPIO pin.
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

    return data;
}

static void delay(uint32_t x)
{
    for (uint32_t i = 0; i < x; ++i);
}


static void toggle_led()
{
    PC->odr = (PC->odr & ~(1 << 13)) | ~(PC->odr & (1 << 13));
}


static void adc_calibrate(adc_t adc)
{
    adc->cr2 |= 1 << 2;
    while (!(adc->cr2 & (1 << 2)));
}


void irq_exti0()
{
    uint32_t orig = PB->odr;

    for (int i = 0; i < 10; ++i) {
        PB->odr = 1 << green_pin;
        delay(150000);
        PB->odr = 0 << green_pin;
        delay(150000); 
    }

    PB->odr = orig;

    EXTI->pr = 1;
}


static void exti_hack()
{
    // Try to point the vector table to RAM (hack)
    vtor[IRQ_EXTI0 + 16] = (uint32_t) irq_exti0;
    //vtor[IRQ_EXTI1 + 16] = (uint32_t) isr_hack;
    SCB->vtor = (uint32_t) vtor;

    // Enable input on PB0 and PB1 for buttons
    gpio_enable(PB, 0, 2, 0);
    gpio_enable(PB, 1, 2, 0);

    // Enable AFIO clock for EXTI
    RCC->apb2enr |= 1;

    // Set external interrupt configuration
    AFIO->exticr1 |= 0x0001;
    //AFIO->exticr1 |= 0x0006;

    // Set trigger selection (rising)
    EXTI->rtsr |= 1;
    //EXTI->rtsr |= 1 << 1;

    // Unmask EXTI interrupts
    EXTI->imr |= 1;
    //EXTI->imr |= 1 << 1;


    // Set interrupt priority (hack)
    NVIC->ipr[6] = 0x10;
    //NVIC->ipr[7] = 0x20;

    NVIC->iser[0] = 1 << 6;
    //NVIC->iser[0] = 1 << 7;
}


int main()
{

    // Enable port clocks (PA + PB + PC)
    RCC->apb2enr |= (1 << 4) | (1 << 3) | (1 << 2);

    // Enable ADC1 clock
    RCC->apb2enr |= 1 << 9;

    // Power on ADC by setting ADON
    ADC1->cr2 |= 1;

    // Enable input on PA0
    gpio_enable(PA, 0, 0, 0);

    // Enable LEDs
    gpio_enable(PB, red_pin, 0, 2);
    gpio_enable(PB, green_pin, 0, 2);
    gpio_enable(PC, 13, 0, 2);

    adc_calibrate(ADC1);

    exti_hack();

    uint16_t threshold = adc_read(ADC1, 0) >> 4;

    while (1) {
        int value = (1 << red_pin) | (1 << green_pin);

        uint16_t sample = adc_read(ADC1, 0) >> 4;
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
