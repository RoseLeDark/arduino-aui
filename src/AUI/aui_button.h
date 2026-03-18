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
    virtual uint8_t on_click(const IElement* sender, aui_event* event)  { return 0; }
    /**
     * @brief Called when a double click is detected.
     *
     * Override this in derived classes to implement custom behavior.
     */
    virtual uint8_t on_double_click(const IElement* sender, aui_event* event)  { return 0; }

    virtual uint8_t on_pressed(const IElement* sender, aui_event* event)  { return 0; }

    virtual uint8_t on_released(const IElement* sender, aui_event* event)  { return 0; }

    /**
     * @brief Initializes the button pin and internal state.
     *
     * Called automatically on MSG_ONSETUP.
     */
    uint8_t on_begin(const IElement* sender, aui_event* event ) override;

    /**
     * @brief Polls the pin and updates internal state.
     *
     * Called automatically on MSG_ONLOOP.
     */
    uint8_t on_update(const IElement* sender, aui_event* event) override;


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
    uint8_t handle_click_logic(const IElement* sender, aui_event* event);

private:
    button_state        m_state;   ///< Current button state
    button_state        m_prev;    ///< Previous button state
    double_button_state m_double;  ///< Double‑click timing state
};

// ---------------------------------------------------------------------------
// Implementation
// ---------------------------------------------------------------------------

template <uint8_t pin, uint8_t TPRESS , uint8_t TUPRESS, uint32_t TDOUBLE_CLICK_TIME >
uint8_t aui_basic_button<pin, TPRESS, TUPRESS, TDOUBLE_CLICK_TIME>::on_begin(const IElement* sender, aui_event* event ) {
    base_type::on_begin(sender,  event );
    m_state.set_state(base_type::m_value);
    m_prev = m_state;
    return 0;
}

template <uint8_t pin, uint8_t TPRESS , uint8_t TUPRESS, uint32_t TDOUBLE_CLICK_TIME >
uint8_t aui_basic_button<pin, TPRESS, TUPRESS, TDOUBLE_CLICK_TIME>::on_update(const IElement* sender, aui_event* event) {
    uint8_t newState = digitalRead(pin);
    m_prev = m_state;

    if (newState != m_state.state) {
        m_state.set_state(newState);

        if (m_prev.state == TUPRESS && m_state.state == TPRESS) {
            return handle_click_logic(sender, event);
        }

        if(m_prev.state == TPRESS && m_state.state == TPRESS) {
           return on_pressed(sender, event);
        }

        if(m_prev.state == TPRESS && m_state.state == TUPRESS) {
           return on_released(sender, event);
        }

    }
    return 1;
}

template <uint8_t pin, uint8_t TPRESS , uint8_t TUPRESS, uint32_t TDOUBLE_CLICK_TIME >
uint8_t aui_basic_button<pin, TPRESS, TUPRESS, TDOUBLE_CLICK_TIME>::handle_click_logic(const IElement* sender, aui_event* event) {
    unsigned long now = millis();
    uint8_t _ret = 1;

    if (now - m_double.lastClickTime <= TDOUBLE_CLICK_TIME) {
        m_double.clickCount++;

        if (m_double.clickCount == 2) {
            _ret = on_double_click( sender,  event);
            m_double.clickCount = 0;
            m_double.lastClickTime = 0;
        }
    } else {
        m_double.clickCount = 1;
        _ret = 0;
    }

    m_double.lastClickTime = now;

    // Einzelklick nur auslösen, wenn kein DoubleClick folgt
    if (m_double.clickCount == 1) {
        _ret =  on_click( sender,  event);
    }
    return _ret;
}


template <uint8_t pin,  uint32_t TDOUBLE_CLICK_TIME = 250>
using aui_button = aui_basic_button<pin, HIGH, LOW, TDOUBLE_CLICK_TIME>;

template <uint8_t pin,  uint32_t TDOUBLE_CLICK_TIME = 250>
using aui_button_pullup = aui_basic_button<pin, LOW, HIGH, TDOUBLE_CLICK_TIME>;