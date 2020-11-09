#include "gpio.h"
#include "rcc.h"

static int red_pin = 12;
static int green_pin = 13;

static void gpio_enable(gpio_t port, int pin, int cnf, int mode)
{
    int y = pin;
    
    if (pin >= 8) {
        y = pin - 8;
        port->crh &= ~(0xf << (y * 4));
        port->crh |= (cnf << (y * 4 + 2)) | (mode << (y * 4));
    } else {
        port->crl &= ~(0xf << (y * 4));
        port->crl |= (cnf << (y * 4 + 2)) | (mode << (y * 4));
    }
}


int main()
{
    RCC->apb2enr |= (1 << 4) | (1 << 3) | (1 << 2);

    gpio_enable(PB, red_pin, 0, 2);
    gpio_enable(PB, green_pin, 0, 2);
    PB->odr = 1 << red_pin;
    PB->odr |= 1 << green_pin;

    
    return 0;
}
