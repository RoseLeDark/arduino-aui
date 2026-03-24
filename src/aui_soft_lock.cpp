#include "AUI/aui_soft_lock.h"

aui_soft_lock::aui_soft_lock()  : m_ticksUntil(0), m_id(-1) {}

// 0 = Lock gesetzt
// 1 = Lock NICHT gesetzt
auier_t aui_soft_lock::try_lock(int16_t id, value_type ticks) {
    if (aui_in_isr()) return AUI_ERROR_ISISR;
    if(id < 0 || ticks == 0) return AUI_ERROR_PARAM;
    if(m_id != id && m_id < 0) return AUI_ERROR_NOTALLOW;

    auier_t _hr = AUI_OK;
    aui_tick_t now = detail::aui_global_get_ticks();

    // Lock aktiv, aber abgelaufen DANN zurücksetzen + Fehler
    if (now >= m_ticksUntil) {
        m_id = -1;
        m_ticksUntil = 0;
        return AUI_ERROR_TIMEOUT;
    } else {
        m_id = id;
        m_ticksUntil = now + ticks;
    }

    return _hr;
}

aui_bool_t aui_soft_lock::is_locked() const {
    if (m_id < 0) return AUI_FALSE;

    return (detail::aui_global_get_ticks() < m_ticksUntil) ? AUI_TRUE : AUI_FALSE;
}

// 0 = OK
// 1 = Fehler (falscher Besitzer)
auier_t aui_soft_lock::unlock(int16_t id) {
    if (aui_in_isr()) return AUI_ERROR_ISISR;
    if(id < 0) return AUI_ERROR_PARAM;
    if(m_id != id && m_id < 0) return AUI_ERROR_NOTALLOW;

    m_id = -1;
    m_ticksUntil = 0;
    return AUI_OK;
}
