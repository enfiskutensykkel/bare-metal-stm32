#ifndef __STM32F103C8_SYSTEM_INTERNAL_H__
#define __STM32F103C8_SYSTEM_INTERNAL_H__

#include <stdint.h>


/*
 * System control block (SCB)
 * Section 4.4 in STM32F10xxx Cortex-M3 programming manual.
 */
struct scb 
{
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

extern volatile struct scb scb;



/*
 * SysTick timer (STK)
 * Section 4.5 in STM32F10xxx Cortex-M3 programming manual.
 */
struct stk 
{
    uint32_t ctrl;          // Control and status
    uint32_t load;          // Reload value
    uint32_t val;           // Current value
    uint32_t calib;         // Calibration value
};

extern volatile struct stk systick;



#endif
