// SPDX-License-Identifier: EUPL-1.2
//
// This file is part of AUI.
// AUI is distributed under the terms of the European Union Public License v1.2.
// You may obtain a copy of the license at:
// https://joinup.ec.europa.eu/collection/eupl/eupl-text-eupl-12

#pragma once 

#include <stdint-gcc.h>
#include "aui_element.h"

/**
 * @class aui_led
 * @brief Simple digital LED output element for the AUI framework.
 *
 * This class represents a single LED connected to a digital output pin.
 * It reacts to AUI messages and toggles its state when instructed.
 *
 * Supported messages:
 *  - MSG_ONSETUP:   Initializes the pin as OUTPUT.
 *  - MSG_PAINT:     Writes the current LED state to the pin.
 *  - MSG_LED_SWITCH: Toggles the LED if the message's LED ID matches TID.
 *
 * The LED does not perform animations, blinking, or timing logic.
 * It is a minimal, deterministic hardware element.
 *
 * @tparam TPIN Digital output pin number.
 * @tparam TID  Logical LED identifier used for MSG_LED_SWITCH routing.
 */
template <uint8_t TPIN, uint8_t TID>
class aui_led : public IVisualElement {
public:
    /**
     * @brief Constructs the LED element with initial OFF state.
     */
    aui_led() : m_state(0) { }

    /**
     * @brief Handles incoming AUI messages.
     *
     * @param sender Pointer to the element that sent the message.
     * @param msg    Message ID.
     * @param arg    Optional payload (used for MSG_LED_SWITCH).
     * @param size   Payload size.
     * @return Always returns 0 for handled messages.
     */
    uint8_t handle_message(IElement* sender, uint8_t msg, void* arg, uint16_t size) {
        if(msg == MSG_ONSETUP) return on_begin();
        if(msg == MSG_PAINT) return on_paint();
        if(msg == MSG_LED_SWITCH) return on_led_switch(arg, size);

        return 0;
    }

protected:
     /**
     * @brief Initializes the LED pin.
     *
     * Called automatically on MSG_ONSETUP.
     *
     * @return Always 0.
     */
    virtual uint8_t on_begin() { pinMode(TPIN, OUTPUT); return 0; }
     /**
     * @brief Writes the current LED state to the pin.
     *
     * Called automatically on MSG_PAINT.
     *
     * @return Always 0.
     */
    virtual uint8_t on_paint() { digitalWrite(TPIN, m_state); return 0; }
     /**
     * @brief Handles LED toggle requests.
     *
     * If the payload contains a LED ID matching TID, the LED toggles its state.
     *
     * Payload:
     *  - arg: pointer to uint8_t containing the LED ID
     *  - size: must be >= 1
     *
     * @param arg  Pointer to LED ID.
     * @param size Payload size.
     * @return 0 on success, 1 if payload is invalid.
     */
    virtual uint8_t on_led_switch(void* arg, uint16_t size) {
        uint8_t *LED = static_cast<uint8_t*>(arg);
        if(LED == NULL) return 1;
        
        if(*LED == TID) m_state = m_state == 0 ? 1 : 0;

        digitalWrite(TPIN, m_state); return 0;

        return 0;
    }
private:
     uint8_t m_state; ///< Current LED state (0 = OFF, 1 = ON)
};