#include "irq.h"
#include "adc.h"
#include "gpio.h"
#include "usart.h"
#include "clock.h"
#include <stddef.h>
#include <stdint.h>

static int red_pin = 12;
static int green_pin = 13;
static volatile uint16_t threshold; // Potentiometer threshold value



static void uart_send(volatile struct usart* usart, const char* str)
{
    usart->cr1 |= 0 << 3;
    usart->cr1 |= 1 << 3;

    const char* p = str;
    while (*p != '\0') {
        while (!(usart->sr & (1 << 7)));
        usart->dr = *p;
        ++p;
    }

    while (!(usart->sr & (1 << 6)));
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
    for (uint32_t i = 0; i < ms; ++i) {
        while (!(systick.ctrl & (1 << 16)));
    }
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
    int tmp = green_pin;
    green_pin = red_pin;
    red_pin = tmp;
    exti.pr |= 1;

    uart_send(&usart1, "swap\r\n");
    flash_alternate(6, 100);
}


static void button_reset()
{
    threshold = adc_read(&adc1, 0);
    exti.pr |= 2;

    uart_send(&usart1, "reset\r\n");
    flash_both(6, 100);
}


void systick_handler(void)
{
    static int ms = 0;

    if (++ms == 1000) {
        uart_send(&usart1, "second\r\n");
        ms = 0;
    }
}


int main()
{
    int clk_speed = rcc_sysclk(SYSCLK_HSE_9);

    // Enable SysTick interrupts
    // TODO: TIM5 interrupts for delay/counter
    // TODO: events instead of interrupts (delaying in irq routine doesn't work becaus irqs block)
    irq_set_handler(IRQ_SysTick, systick_handler);
    // TODO: use systick for preemptive scheduling
    systick.load = clk_speed / 8 / 1000;
    systick.ctrl |= 2 | 1;
    //systick.ctrl |= 1;

    // Enable port clocks (PA + PB + PC)
    rcc.apb2enr |= (1 << 4) | (1 << 3) | (1 << 2);

    // Enable ADC1 clock
    rcc.apb2enr |= 1 << 9;

    // Power on ADC by setting ADON
    adc1.cr2 |= 1;

    // Enable AFIO clock for EXTI interrupts
    rcc.apb2enr |= 1;

    // Enable input on PA0
    gpio_cfg(&gpioa, 0, GPIO_ANALOG, GPIO_INPUT);

    // Enable LEDs
    gpio_cfg(&gpiob, red_pin, GPIO_PUSHPULL, GPIO_2MHZ);
    gpio_cfg(&gpiob, green_pin, GPIO_PUSHPULL, GPIO_2MHZ);
    gpio_cfg(&gpioc, 13, GPIO_PUSHPULL, GPIO_2MHZ);

    // After a reset, the ADC requires calibration
    adc_calibrate(&adc1);

    // Set up custom EXTI interrupt vectors
    irq_set_handler(IRQ_EXTI0, button_reset);
    irq_set_handler(IRQ_EXTI1, button_swap);

    // Set interrupt priorities (I have no idea what I'm doing here)
    irq_set_priority(IRQ_EXTI0, 2);
    irq_set_priority(IRQ_EXTI1, 3);

    // Set up EXTI line interrupts for pins
    exti_enable(&gpiob, 0, EXTI_TRIGGER_RISING);
    exti_enable(&gpiob, 1, EXTI_TRIGGER_RISING);

    // Take initial sample
    threshold = adc_read(&adc1, 0);

    // Enable interrupts
    irq_enable(IRQ_EXTI0);
    irq_enable(IRQ_EXTI1);

    // Enable USART2 clock
    //rcc.apb1enr |= 1 << 17;
    rcc.apb2enr |= 1 << 14;

    // Set PA9 to TX and PA10 to RX
    // Ref section 9.1.11
    //gpio_enable(&gpioa, 10, 2, 0);
    gpio_cfg(&gpioa, 9, GPIO_AFIO | GPIO_PUSHPULL, GPIO_50MHZ);

    // Test UART transmit
    usart1.cr1 |= 1 << 13; // USART enable
    usart1.cr1 |= 0 << 12; // 8 data bits
    usart1.cr1 |= 0 << 9;  // Even parity
    usart1.cr2 |= 2 << 12; // 2 stop bits
    usart1.brr = clk_speed / 115200;    // Baud rate 115200

    flash_alternate(5, 100);

    while (1) {
        int value = (1 << red_pin) | (1 << green_pin);

        uint16_t sample = adc_read(&adc1, 0);
        if (sample < threshold) {
            value = 1 << red_pin;
        } else if (sample > threshold) {
            value = 1 << green_pin;
        }

        out_b = value;

        delay(250);
        toggle_led();
    }
}
