#ifndef __STM32_F103C8_SYSTEM_H__
#define __STM32_F103C8_SYSTEM_H__

#include <stdint.h>
#include "register.h"


/*
 * Nested vectored interrupt controller (NVIC)
 * Section 4.3 in STM32F10xxx Cortex-M3 programming manual.
 */
REGISTER(nvic, struct {
    uint32_t iser[3];       // Interrupt set-enable registers
    uint32_t reserved0[29]; // Reserved
    uint32_t icer[3];       // Interrupt clear-enable registers (offset 0x080)
    uint32_t reserved1[29]; // Reserved
    uint32_t ispr[3];       // Interrupt set-pending registers (offset 0x100)
    uint32_t reserved2[29]; // Reserved
    uint32_t icpr[3];       // Interrupt clear-pending registers (offset 0x180)
    uint32_t reserved3[29]; // Reserved
    uint32_t iabr[3];       // Interrupt active bit registers (offset 0x200)
    uint32_t reserved4[62]; // Reserved
    uint8_t  ipr[84];       // Interrupt priority registers (offset 0x300)
    uint32_t pad[2732];     // Padding (for SCB registers)
    uint32_t stir;          // Software trigger interrupt register
});

#define NVIC    ((nvic_t) 0xe000e100)


/*
 * System control block (SCB)
 * Section 4.4 in STM32F10xxx Cortex-M3 programming manual.
 */
REGISTER(scb, struct {
    uint32_t cpuid;         // CPUID base register
    uint32_t icsr;          // Interrupt control and state 
    uint32_t vtor;          // Vector table offset
    uint32_t aircr;         // Application interrupt and reset control
    uint32_t scr;           // System control register
    uint32_t ccr;           // Configuration and control register
    uint32_t shpr[3];       // System handler priority 
    uint32_t shcsr;         // System handler control and state
    uint32_t cfsr;          // Configurable fault status
    uint32_t hfsr;          // Hard fault status
    uint32_t mmfar;         // Memory management fault address
    uint32_t bfar;          // Bus fault address
});

#define SCB     ((scb_t) 0xe000ed00)



/*
 * SysTick timer (STK)
 * Section 4.5 in STM32F10xxx Cortex-M3 programming manual.
 */
REGISTER(stk, struct {
    uint32_t ctrl;          // Control and status
    uint32_t load;          // Reload value
    uint32_t val;           // Current value
    uint32_t calib;         // Calibration value
});

#define SYSTICK ((stk_t) 0xe000e010)


/*
 * Exception/interrupt numbers
 * Defined in 2.3.2 in STM32F10xxx Cortex-M3 programming manual,
 * and in 10.1.2 in RM0008.
 *
 * irq no < 0 = exception
 * irq no >= 0 interrupt (offset 0x40 in vector table)
 */
typedef enum {
    NMI                     = -14,  // Non-maskable interrupt
    HardFault               = -13,  // Hard fault
    MemManage               = -12,  // Memory access fault
    BusFault                = -11,  // Bus fault
    UsageFault              = -10,  // Usage fault
    SVCall                  = -5,   // Supervisor call
    DebugMonitor            = -4,   // Debug monitor
    PendSV                  = -2,   // PendSV
    SysTick                 = -1,   // System timer reaches zero
    WWDG                    = 0,    // Window watchdog 
    PVD                     = 1,    // PVD through EXTI Line detection 
    TAMPER                  = 2,    // Tamper interrupt
    RTC                     = 3,    // RTC global interrupt
    FLASH                   = 4,    // Flash global interrupt
    RCC                     = 5,    // RCC global interrupt
    EXTI0                   = 6,    // EXTI line 0
    EXTI1                   = 7,   
    EXTI2                   = 8,    
    EXTI3                   = 9,    
    EXTI4                   = 10,   
    DMA1_Channel1           = 11,   // DMA1 Channel1 interrupt
    DMA1_Channel2           = 12,
    DMA1_Channel3           = 13,
    DMA1_Channel4           = 14,
    DMA1_Channel5           = 15,
    DMA1_Channel6           = 16,
    DMA1_Channel7           = 17,
    ADC1_2                  = 18,   // ADC1 and ADC2 global interrupt
    USB_HP_CAN_TX           = 19,   // USB high priority or CAN TX
    USB_LP_CAN_RX0          = 20,   // USB low priority or CAN RX0
    CAN_RX1                 = 21,   
    CAN_SCE                 = 22,  
    EXTI9_5                 = 23,   // EXTI line 9-5
    TIM1_BRK                = 24,   // TIM1 break interrupt
    TIM1_UP                 = 25,   // TIM1 update interrupt
    TIM1_TRG_COM            = 26,   // TIM1 trigger and commutation
    TIM1_CC                 = 27,   // TIM1 capture compare interrupt
    TIM2                    = 28,   // TIM2 global interrupt
    TIM3                    = 29,   // TIM3 global interrupt
    TIM4                    = 30,   // TIM24global interrupt
    I2C1_EV                 = 31,   // I2C1 event
    I2C1_ER                 = 32,   // I2C1 error
    I2C2_EV                 = 33,   // I2C2 event
    I2C2_ER                 = 34,   // I2C2 error
    SPI1                    = 35,   // SPI1 global interrupt
    SPI2                    = 36,   // SPI2 global interrupt
    USART1                  = 37,   // USART1 global interrupt
    USART2                  = 38,   // USART2 global interrupt
    USART3                  = 39,   // USART3 global interrupt
    EXTI15_10               = 40,   // EXTI line 15-10
    RTCAlarm                = 41,   // RTC alarm through EXTI line
    USBWakeup               = 42,   // USB wakeup from suspend through EXTI line
    TIM8_BRK                = 43,   // TIM8 break interrupt
    TIM8_UP                 = 44,   // TIM8 update interrupt
    TIM8_TRG_COM            = 45,   // TIM8 trigger and commutation
    TIM8_CC                 = 46,   // TIM8 capture compare
    ADC3                    = 47,   // ADC3 global interrupt
    FSMC                    = 48,   
    SDIO                    = 49,
    TIM5                    = 50,   // TIM5 global interrupt
    SPI3                    = 51,   // SPI3 global interrupt
    UART4                   = 52,   // UART4 global interrupt
    UART5                   = 53,   // UART5 global interrupt
    TIM6                    = 54, 
    TIM7                    = 55,
    DMA2_Channel1           = 56,   // DMA2 channel 1 interrupt
    DMA2_Channel2           = 57,
    DMA2_Channel3           = 58,
    DMA2_Channel4_5         = 59,
} irqno_t;


#endif
