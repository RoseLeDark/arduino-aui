#include <Arduino.h>
#include "aui/aui_timer.h"

// Deklaration des originalen Arduino-Timer0-Overflow-Vektors
extern "C" void __vector_16(void) __attribute__((signal));
extern "C" volatile unsigned long timer0_millis;


// Weak: kann vom Nutzer überschrieben werden
extern "C" void AUI_WEAK aui_timer0_callback(unsigned long now)
{
    // Default: nichts tun
}
extern "C" void AUI_WEAK aui_timer_callback() {
    // Default: nichts tun
}

ISR(TIMER0_OVF_vect)
{
    // 1. Arduino-Original-ISR
    __vector_16();

    // 2. Dein Hook
     aui_timer0_callback(timer0_millis);
}

#if AUI_CONFIG_TIMER_OFF != AUI_CONFIG_ON
ISR(TIMER1_COMPA_vect) { 
    aui_timer_callback();
}
#endif

//-------------------------------------------------------


