#include "AUI/aui_soft_lock.h"

aui_soft_lock::aui_soft_lock()  : m_ticksUntil(0), m_id(-1) {}

// 0 = Lock gesetzt
// 1 = Lock NICHT gesetzt
uint8_t aui_soft_lock::try_lock(int16_t id, value_type ticks) {
        // Interrupts/Timer dürfen keinen Lock setzen
    if (aui_in_isr()) return 2;

    aui_tick_t now = detail::aui_global_get_ticks();

    // Kein Lock aktiv → setzen
    if (m_id < 0) {
        m_id = id;
        m_ticksUntil = now + ticks;
        return 0;
    }

    // Lock aktiv, aber abgelaufen → zurücksetzen + Fehler
    if (now >= m_ticksUntil) {
        m_id = -1;
        m_ticksUntil = 0;
        return 1;
    }

    // Lock aktiv und NICHT abgelaufen → Fehler
    return 1;
}

// 1 = NICHT gesperrt
// 0 = gesperrt
uint8_t aui_soft_lock::is_locked() const {
    if (m_id < 0) return 1;

    return (detail::aui_global_get_ticks() < m_ticksUntil) ? 0 : 1;
}

// 0 = OK
// 1 = Fehler (falscher Besitzer)
uint8_t aui_soft_lock::unlock(int16_t id) {
        // Interrupts/Timer dürfen keinen Lock aufheben
    if (aui_in_isr()) return 2;

    if (m_id != id) return 1; // falscher Besitzer

    m_id = -1;
    m_ticksUntil = 0;
    return 0;
}
