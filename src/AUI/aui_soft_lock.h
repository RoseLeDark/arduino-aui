#pragma once

#include "aui_config.h"
#include "aui_system.h"

// Keine C++17‑Abhängigkeit
#define AUI_LOCKED(LOCK, ID, TICKS) \
    aui_lock_guard<aui_soft_lock, ID>  AUI_guard(LOCK, TICKS); \
    if( (uint8_t)(AUI_guard) == 0 )
    
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

class aui_soft_lock {
public:
#if AUI_PLATFORM_SAMD == 1 || AUI_PLATFORM_STM32 == 1 || AUI_PLATFORM_ESP32 == 1
    using value_type = uint16_t;
#else
    using value_type = uint8_t;
#endif
    aui_soft_lock();
    aui_soft_lock(const aui_soft_lock&) = delete;
    aui_soft_lock(aui_soft_lock&&) = delete;

    // 0 = Lock gesetzt
    // 1 = Lock NICHT gesetzt
    // 2 = Wir sind im ISR modus. ISR darf nicht setzen
    uint8_t try_lock(int16_t id, value_type ticks) ;

    // 1 = NICHT gesperrt - Fehler
    // 0 = gesperrt - OK
    uint8_t is_locked() const;

    // 0 = OK
    // 1 = Fehler (falscher Besitzer)
    // 2 = Wir sind im ISR modus. ISR darf nicht aufrufen
    uint8_t unlock(int16_t id);

    int16_t owner() const { return m_id; }

    aui_soft_lock& operator=(aui_soft_lock&&) = delete;
    aui_soft_lock& operator=(const aui_soft_lock&) = delete;

    
private:
    aui_tick_t m_ticksUntil;
    int16_t    m_id;
};

template <class TLOCK, int16_t TID>
class aui_lock_guard {
public:
    using lock_type = TLOCK;
    using value_type = typename lock_type::value_type;

    aui_lock_guard(lock_type& lock, value_type ticks)
        : m_lock(lock) { m_lock.try_lock(TID, ticks); }
    ~aui_lock_guard() { if(is_locked() == 0) m_lock.unlock(TID); }

     // 1 = NICHT gesperrt, 0 = gesperrt
    operator uint8_t () { return m_lock.is_locked(); }
private:
    lock_type& m_lock;
};

