#include <Arduino.h>
#include "aui/aui_timer.h"

// Deklaration des originalen Arduino-Timer0-Overflow-Vektors
extern "C" void __vector_16(void) __attribute__((signal));
extern "C" volatile unsigned long timer0_millis;

volatile uint64_t __global_millis = 0;

// Weak: kann vom Nutzer überschrieben werden
extern "C" void AUI_WEAK aui_timer0_callback(unsigned long ms)
{
    // Default: nichts tun
}
extern "C" void AUI_WEAK aui_timer1_callback(unsigned long ms) {
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

unsigned long ___timer1_ticks = 0;
volatile unsigned long ___timer1_millis;

#if AUI_CONFIG_TIMER_1000HZ == AUI_CONFIG_USED
    #define AUI_TIMER1_MS_PER_TICK 1
#elif AUI_CONFIG_TIMER_500HZ == AUI_CONFIG_USED
    #define AUI_TIMER1_MS_PER_TICK 2
#elif AUI_CONFIG_TIMER_100HZ == AUI_CONFIG_USED
    #define AUI_TIMER1_MS_PER_TICK 10
#elif AUI_CONFIG_TIMER_50HZ == AUI_CONFIG_USED
    #define AUI_TIMER1_MS_PER_TICK 20
#elif AUI_CONFIG_TIMER_20HZ == AUI_CONFIG_USED
    #define AUI_TIMER1_MS_PER_TICK 50
#endif

ISR(TIMER1_COMPA_vect) { 
    ___timer1_ticks++;
    ___timer1_millis = (uint64_t)___timer1_ticks * AUI_TIMER1_MS_PER_TICK;

    aui_timer1_callback(___timer1_millis);
}

uint64_t aui_get_timer1_millis() {
    return ___timer1_millis;
}

#endif



uint64_t aui_get_loop_millis() {
    return __global_millis;
}

//-------------------------------------------------------

uint64_t aui_get_timer0_millis() {
    return timer0_millis;
}


