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

/**
 * @class aui_button
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
template <uint8_t pin, uint32_t TDOUBLE_CLICK_TIME = 250 /*ms*/>
class aui_button : public IVisualElement {
public:
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
    aui_button() {}
    /**
     * @brief Handles AUI messages for setup and periodic updates.
     *
     * Supported messages:
     *  - MSG_ONSETUP: Initializes the pin and internal state.
     *  - MSG_ONLOOP:  Polls the pin and processes click logic.
     *
     * @param sender Pointer to the element that sent the message.
     * @param msg    Message ID.
     * @param arg    Optional payload (unused).
     * @param size   Payload size (unused).
     * @return Always returns 0.
     */
    uint8_t handle_message(IElement* sender, uint8_t msg, void* arg, uint16_t size);

     /**
     * @brief Called when a single click is detected.
     *
     * Override this in derived classes to implement custom behavior.
     */
    virtual void on_click()  { }
    /**
     * @brief Called when a double click is detected.
     *
     * Override this in derived classes to implement custom behavior.
     */
    virtual void on_double_click()  { }

private:
    /**
     * @brief Initializes the button pin and internal state.
     *
     * Called automatically on MSG_ONSETUP.
     */
    void begin();

    /**
     * @brief Polls the pin and updates internal state.
     *
     * Called automatically on MSG_ONLOOP.
     */
    void update();

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
    void handle_click_logic();

private:
    button_state        m_state;   ///< Current button state
    button_state        m_prev;    ///< Previous button state
    double_button_state m_double;  ///< Double‑click timing state
};

// ---------------------------------------------------------------------------
// Implementation
// ---------------------------------------------------------------------------

template <uint8_t pin, uint32_t TDOUBLE_CLICK_TIME>
void aui_button<pin, TDOUBLE_CLICK_TIME>::begin() {
    pinMode(pin, INPUT);
    uint8_t s = digitalRead(pin);
    m_state.set_state(s);
    m_prev = m_state;
}

template <uint8_t pin, uint32_t TDOUBLE_CLICK_TIME>
void aui_button<pin, TDOUBLE_CLICK_TIME>::update() {
    uint8_t newState = digitalRead(pin);
    m_prev = m_state;

    if (newState != m_state.state) {
        m_state.set_state(newState);

        if (just_pressed()) {
            handle_click_logic();
        }
    }
}

template <uint8_t pin, uint32_t TDOUBLE_CLICK_TIME>
bool aui_button<pin, TDOUBLE_CLICK_TIME>::just_pressed() const {
    return m_prev.state == LOW && m_state.state == HIGH;
}

template <uint8_t pin, uint32_t TDOUBLE_CLICK_TIME>
void aui_button<pin, TDOUBLE_CLICK_TIME>::handle_click_logic() {
    unsigned long now = millis();

    if (now - m_double.lastClickTime <= TDOUBLE_CLICK_TIME) {
        m_double.clickCount++;

        if (m_double.clickCount == 2) {
            on_double_click();
            m_double.clickCount = 0;
            m_double.lastClickTime = 0;
            return;
        }
    } else {
        m_double.clickCount = 1;
    }

    m_double.lastClickTime = now;

    // Einzelklick nur auslösen, wenn kein DoubleClick folgt
    if (m_double.clickCount == 1) {
        on_click();
    }
}

template <uint8_t pin, uint32_t TDOUBLE_CLICK_TIME>
uint8_t aui_button<pin, TDOUBLE_CLICK_TIME>::handle_message(IElement* sender, uint8_t msg, void* arg, uint16_t size)  {
        switch (msg) {
        case MSG_ONSETUP:
            begin();
            break;

        case MSG_ONLOOP:
            update();
            break;
        }
        return 0;
    }