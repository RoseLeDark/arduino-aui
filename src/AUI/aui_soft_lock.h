#pragma once

#include "aui_config.h"
#include "aui_system.h"
#include "aui_arch.h"

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


    auier_t try_lock(int16_t id, value_type ticks) ;

    // 1 = NICHT gesperrt - Fehler
    // 0 = gesperrt - OK
    aui_bool_t is_locked() const;

    // 0 = OK
    // 1 = Fehler (falscher Besitzer)
    // 2 = Wir sind im ISR modus. ISR darf nicht aufrufen
    auier_t unlock(int16_t id);

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
    ~aui_lock_guard() { if(m_lock.is_locked() == 0) m_lock.unlock(TID); }

    operator aui_bool_t () { return m_lock.is_locked(); }
private:
    lock_type& m_lock;
};

