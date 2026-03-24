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
        auisystem.send_massage<aui_event<aui_idble_payload>>(&auisystem, MSG_GPIO_WRITE, 
               aui_event<aui_idble_payload>(TPIN, &value ));
    }

    void send_off() {
        uint8_t value = 0;

        auisystem.send_massage<aui_event<aui_idble_payload>>(&auisystem, MSG_GPIO_WRITE, 
               aui_event<aui_idble_payload>(TPIN, &value ));

    }
    void send_switch() {
        auisystem.send_massage<aui_event<aui_idble_payload>>(&auisystem, MSG_GPIO_SWITCH, 
               aui_event<aui_idble_payload>(TPIN, nullptr ));
    }
};

template <uint8_t TPIN, aui_gpio_resolution TRESOLUTION = aui_gpio_resolution::resolution_8bit>
using aui_led_pwm = aui_pwm_output<TPIN, TRESOLUTION>;


struct aui_color_rgb {
    uint8_t r, g, b;

    aui_color_rgb() : r(0), g(0), b(0) { }
};

template <uint8_t TID, uint8_t TRPIN, uint8_t TGPIN, uint8_t TBPIN>
class aui_led_rgb : public IElementWithID<TID> {
public:
    using base_type = IElementWithID<TID>;
    using value_type = typename aui_led_pwm<TRPIN>::value_type;

    aui_led_rgb(aui_color_rgb base_color) 
        :  m_ledRed(base_color.r), m_ledGreen(base_color.g), m_ledBlue(base_color.b), m_prevColor(base_color)  { }

    void send_on()  { 
        send_color(m_prevColor);  
    }

    void send_off() { 
        m_prevColor.r = m_ledRed.get_value();
        m_prevColor.g = m_ledGreen.get_value();
        m_prevColor.b = m_ledBlue.get_value();

       send_color(aui_color_rgb() );  
    }

    void send_color(aui_color_rgb color) {
        auisystem.send_massage<aui_event<aui_idble_payload>>(&auisystem, MSG_GPIO_WRITE, aui_event<aui_idble_payload>(TRPIN, &color.r ));
        auisystem.send_massage<aui_event<aui_idble_payload>>(&auisystem, MSG_GPIO_WRITE, aui_event<aui_idble_payload>(TGPIN, &color.g ));
        auisystem.send_massage<aui_event<aui_idble_payload>>(&auisystem, MSG_GPIO_WRITE, aui_event<aui_idble_payload>(TBPIN, &color.b ));
    }

    void send_value(value_type value) {
        auisystem.send_massage<aui_event<aui_idble_payload>>(&auisystem, MSG_GPIO_WRITE, aui_event<aui_idble_payload>(TRPIN, &value ));
        auisystem.send_massage<aui_event<aui_idble_payload>>(&auisystem, MSG_GPIO_WRITE, aui_event<aui_idble_payload>(TGPIN, &value ));
        auisystem.send_massage<aui_event<aui_idble_payload>>(&auisystem, MSG_GPIO_WRITE, aui_event<aui_idble_payload>(TBPIN, &value ));
    }
    void send_switch() {

        auisystem.send_massage<aui_event<aui_idble_payload>>(&auisystem, MSG_GPIO_SWITCH,  aui_event<aui_idble_payload>(TRPIN, nullptr ));
        auisystem.send_massage<aui_event<aui_idble_payload>>(&auisystem, MSG_GPIO_SWITCH,  aui_event<aui_idble_payload>(TGPIN, nullptr ));
        auisystem.send_massage<aui_event<aui_idble_payload>>(&auisystem, MSG_GPIO_SWITCH,   aui_event<aui_idble_payload>(TBPIN, nullptr ));
    }
protected:
    virtual uint8_t on_disable(const IElement* sender, const uint8_t ID) override {
        if(ID == TID) {
            send_off();
            base_type::set_enable(1); 
            return AUI_OK;;
        }
        return 1; 
    }
    virtual uint8_t on_enable(const IElement* sender, const uint8_t ID) override { 
         if(ID == TID) {
            send_on();
            base_type::set_enable(0); 
            return AUI_OK;;
        }
        return 1;  
    }
protected:
    aui_led_pwm<TRPIN> m_ledRed;
    aui_led_pwm<TGPIN> m_ledGreen;
    aui_led_pwm<TBPIN> m_ledBlue;

    aui_color_rgb m_prevColor;
};