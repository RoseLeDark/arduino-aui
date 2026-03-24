// SPDX-License-Identifier: EUPL-1.2
//
// This file is part of AUI.
// AUI is distributed under the terms of the European Union Public License v1.2.
// You may obtain a copy of the license at:
// https://joinup.ec.europa.eu/collection/eupl/eupl-text-eupl-12

#pragma once

#include <stdint-gcc.h>
#include <Arduino.h>
#include "aui_element.h"
#include "aui_gpio.h"

/**
 * @class aui_basic_button
 * @brief Digital input button element with click and double‑click detection.
 *
 * This class implements a deterministic, polling‑based button handler that
 * integrates into the AUI message system. It detects:
 *
 *  - Rising edges (button press)
 *  - Single clicks
 *  - Double clicks within a configurable time window
 *
 * The button does not debounce in software; it relies on stable hardware input
 * or external debouncing. All timing is based on millis().
 *
 * @tparam pin                 Digital input pin number.
 * @tparam TDOUBLE_CLICK_TIME  Maximum time window (in ms) between two presses
 *                             to be considered a double‑click.
 */
template <uint8_t pin, uint8_t TPRESS = HIGH, uint8_t TUPRESS = LOW, uint32_t TDOUBLE_CLICK_TIME = 250>
class aui_basic_button : public aui_digital_input<pin> {
public:
    using base_type = aui_digital_input<pin> ;
    /**
     * @struct button_state
     * @brief Stores the current or previous button state.
     *
     * This structure tracks:
     *  - The logical pin state (HIGH or LOW)
     *  - The timestamp of the last state change
     */
    struct button_state {
        uint8_t state;              ///< Current digital state (HIGH or LOW)
        unsigned long timestamp;    ///< Timestamp of last state change (millis)

        /**
         * @brief Updates the state and records the timestamp.
         * @param s New digital state.
         */
        void set_state(uint8_t s) { state = s; timestamp = millis(); }
    };
    /**
     * @struct double_button_state
     * @brief Tracks timing information for double‑click detection.
     */
    struct double_button_state {
        unsigned long lastClickTime = 0;
        uint8_t       clickCount = 0;
    };
    /**
     * @brief Constructs a new button instance.
     *
     * The pin is not configured until MSG_ONSETUP is received.
     */
    aui_basic_button() {}
protected:
     /**
     * @brief Called when a single click is detected.
     *
     * Override this in derived classes to implement custom behavior.
     */
    virtual auier_t on_click(const IElement* sender, const uint64_t tick)  { return AUI_OK; }
    /**
     * @brief Called when a double click is detected.
     *
     * Override this in derived classes to implement custom behavior.
     */
    virtual auier_t on_double_click(const IElement* sender, const uint64_t tick)  { return AUI_OK; }

    virtual auier_t on_pressed(const IElement* sender, const uint64_t tick)  { return AUI_OK; }

    virtual auier_t on_released(const IElement* sender, const uint64_t tick)  { return AUI_OK; }

    /**
     * @brief Initializes the button pin and internal state.
     *
     * Called automatically on MSG_ONSETUP.
     */
    auier_t on_begin(const IElement* sender, const aui_event_ex<void*>* event ) override;

    /**
     * @brief Polls the pin and updates internal state.
     *
     * Called automatically on MSG_ONLOOP.
     */
    auier_t on_update(const IElement* sender, const uint64_t ticks) override;


    /**
     * @brief Checks whether the button was just pressed (rising edge).
     * @return true if previous state was LOW and current state is HIGH.
     */
    bool just_pressed() const;

    /**
     * @brief Processes click and double‑click timing logic.
     *
     * This method:
     *  - Increments click count
     *  - Checks double‑click window
     *  - Calls OnClick() or OnDoubleClick()
     */
    auier_t handle_click_logic(const IElement* sender, const uint64_t ticks);

private:
    button_state        m_state;   ///< Current button state
    button_state        m_prev;    ///< Previous button state
    double_button_state m_double;  ///< Double‑click timing state
};

// ---------------------------------------------------------------------------
// Implementation
// ---------------------------------------------------------------------------

template <uint8_t pin, uint8_t TPRESS , uint8_t TUPRESS, uint32_t TDOUBLE_CLICK_TIME >
auier_t aui_basic_button<pin, TPRESS, TUPRESS, TDOUBLE_CLICK_TIME>::on_begin(const IElement* sender, const aui_event_ex<void*>* event ) {
    base_type::on_begin(sender,  event );
    m_state.set_state(base_type::m_value);
    m_prev = m_state;
    return AUI_OK;
}

template <uint8_t pin, uint8_t TPRESS , uint8_t TUPRESS, uint32_t TDOUBLE_CLICK_TIME >
auier_t aui_basic_button<pin, TPRESS, TUPRESS, TDOUBLE_CLICK_TIME>::on_update(const IElement* sender, const uint64_t ticks) {
    uint8_t newState = digitalRead(pin);
    m_prev = m_state;

    if (newState != m_state.state) {
        m_state.set_state(newState);

        if (m_prev.state == TUPRESS && m_state.state == TPRESS) {
            return handle_click_logic(sender, ticks);
        }

        if(m_prev.state == TPRESS && m_state.state == TPRESS) {
           return on_pressed(sender, ticks);
        }

        if(m_prev.state == TPRESS && m_state.state == TUPRESS) {
           return on_released(sender, ticks);
        }

    }
    return AUI_ERROR_NOTHANDLED;
}

template <uint8_t pin, uint8_t TPRESS , uint8_t TUPRESS, uint32_t TDOUBLE_CLICK_TIME >
auier_t aui_basic_button<pin, TPRESS, TUPRESS, TDOUBLE_CLICK_TIME>::handle_click_logic(const IElement* sender, const uint64_t ticks) {
    unsigned long now = millis();
    uint8_t _ret = 1;

    if (now - m_double.lastClickTime <= TDOUBLE_CLICK_TIME) {
        m_double.clickCount++;

        if (m_double.clickCount == 2) {
            _ret = on_double_click( sender,  ticks);
            m_double.clickCount = 0;
            m_double.lastClickTime = 0;
        }
    } else {
        m_double.clickCount = 1;
        _ret = AUI_OK;
    }

    m_double.lastClickTime = now;

    // Einzelklick nur auslösen, wenn kein DoubleClick folgt
    if (m_double.clickCount == 1) {
        _ret =  on_click( sender,  ticks);
    }
    return _ret;
}
template <uint8_t TPIN,  uint32_t TDOUBLE_CLICK_TIME = 250>
using aui_button = aui_basic_button<TPIN, HIGH, LOW, TDOUBLE_CLICK_TIME>;

template <uint8_t TPIN,  uint32_t TDOUBLE_CLICK_TIME = 250>
using aui_button_pullup = aui_basic_button<TPIN, LOW, HIGH, TDOUBLE_CLICK_TIME>;
