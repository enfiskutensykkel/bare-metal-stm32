#ifndef __STM32F103C8_PRIVATE_PERIPHERAL_BUS_H__
#define __STM32F103C8_PRIVATE_PERIPHERAL_BUS_H__

#include <stdint.h>


/*
 * Nested vectored interrupt controller (NVIC)
 * Section 4.3 in STM32F10xxx Cortex-M3 programming manual.
 */
struct nvic {
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
};

extern volatile struct nvic NVIC;


/*
 * System control block (SCB)
 * Section 4.4 in STM32F10xxx Cortex-M3 programming manual.
 */
struct scb {
    const uint32_t cpuid;   // CPUID base register
    uint32_t icsr;          // Interrupt control and state 
    const uint32_t vtor;    // Vector table offset
    uint32_t aircr;         // Application interrupt and reset control
    uint32_t scr;           // System control register
    uint32_t ccr;           // Configuration and control register
    uint32_t shpr[3];       // System handler priority 
    uint32_t shcsr;         // System handler control and state
    uint32_t cfsr;          // Configurable fault status
    uint32_t hfsr;          // Hard fault status
    const uint32_t mmfar;   // Memory management fault address
    const uint32_t bfar;    // Bus fault address
};

extern volatile struct scb SCB;



/*
 * SysTick timer (STK)
 * Section 4.5 in STM32F10xxx Cortex-M3 programming manual.
 */
struct stk {
    uint32_t ctrl;          // Control and status
    uint32_t load;          // Reload value
    uint32_t val;           // Current value
    uint32_t calib;         // Calibration value
};

extern volatile struct stk STK;



#endif
