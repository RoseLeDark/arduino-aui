// SPDX-License-Identifier: EUPL-1.2
//
// This file is part of AUI.
// AUI is distributed under the terms of the European Union Public License v1.2.
// You may obtain a copy of the license at:
// https://joinup.ec.europa.eu/collection/eupl/eupl-text-eupl-12

#pragma once 

#include <stdint.h>
#include "aui_gpio.h"   
#include "aui_system.h"

/**
 * @class aui_led
 * @brief Simple digital LED output element for the AUI framework.
 *
 * This class represents a single LED connected to a digital output pin.
 * It reacts to AUI messages and toggles its state when instructed.
 *
 * Supported messages:
 *  - MSG_ONSETUP:   Initializes the pin as OUTPUT.
 *
 * The LED does not perform animations, blinking, or timing logic.
 * It is a minimal, deterministic hardware element.
 *
 * @tparam TPIN Digital output pin number.
 */
template <uint8_t TPIN>
class aui_led : public aui_digital_output<TPIN> {
public:
    using value_type = bool;
    using base_type = aui_digital_output<TPIN>;

    void send_on() {
        uint8_t value = 1;
        auisystem.send_massage<aui_idble_event>(this, MSG_GPIO_WRITE, aui_idble_event::make(&value, 1, TPIN) );
    }

    void send_off() {
        uint8_t value = 0;
        auisystem.send_massage<aui_idble_event>(this, MSG_GPIO_WRITE, aui_idble_event::make(&value, 1, TPIN) );
    }
    void send_switch() {
        auisystem.send_massage<aui_idble_event>(this, MSG_GPIO_SWITCH, aui_idble_event::make(TPIN) );
    }
};