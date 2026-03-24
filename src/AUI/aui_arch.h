#pragma once

// Keine C++17‑Abhängigkeit
#define AUI_LOCKED(LOCK, ID, TICKS) \
    aui_lock_guard<aui_soft_lock, ID>  AUI_guard(LOCK, TICKS); \
    if( (aui_bool_t)(AUI_guard) == AUI_TRUE )
    
#if AUI_PLATFORM_AVR
    // AVR: I-Bit in SREG zeigt an, ob wir im ISR sind
    #define aui_in_isr() ((SREG & (1 << 7)) ? 0 : 1)

#elif AUI_PLATFORM_ESP32
    // ESP32: FreeRTOS liefert die Info direkt
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #define aui_in_isr() (xPortInIsrContext() ? 1 : 0)

#elif AUI_PLATFORM_STM32 || AUI_PLATFORM_SAMD
    // ARM Cortex-M: VECTACTIVE != 0 bedeutet ISR aktiv
    #define aui_in_isr() ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) ? 1 : 0)

#else
    // Fallback: kein ISR bekannt → immer Loop
    #define aui_in_isr() 0
#endif
