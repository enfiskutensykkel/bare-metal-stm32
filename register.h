#ifndef __STM32_F103C8_REGISTER_H__
#define __STM32_F103C8_REGISTER_H__

/* Convenience macro for creating register types */
#define REGISTER(name, decl) \
    typedef volatile decl * const name##_t 

#endif
