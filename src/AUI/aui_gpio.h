// SPDX-License-Identifier: EUPL-1.2
//
// This file is part of AUI.
// AUI is distributed under the terms of the European Union Public License v1.2.
// You may obtain a copy of the license at:
// https://joinup.ec.europa.eu/collection/eupl/eupl-text-eupl-12


#pragma once 

#include "aui_element.h"

#ifndef AUI_DEFAULT_ANALOG_RES

#if defined(ARDUINO_AVR_UNO)
    #define AUI_DEFAULT_ANALOG_RES aui_gpio_resolution::resolution_8bit
#elif defined(ARDUINO_ESP32_DEV)
    #define AUI_DEFAULT_ANALOG_RES aui_gpio_resolution::resolution_10Bit
#else
    #define AUI_DEFAULT_ANALOG_RES aui_gpio_resolution::resolution_12Bit
#endif

#endif

enum class aui_gpio_type : uint8_t {
    digital_output,
    analog_output,
    digital_input,
    digital_input_pullup
};

enum class aui_gpio_resolution : uint16_t {
    resolution_8bit = 255,
    resolution_10bit = 1023,
    resolution_12bit = 4095
};

/**
 * @class aui_gpio
 * @brief Simple gpio pin element for the AUI framework.
 *
 * This class represents a single gpio pin.
 * It reacts to AUI messages and toggles its state when instructed.
 *
 * Supported messages:
 *  - MSG_ONSETUP:    Initializes the pin as given State.
 *  - MSG_GPIO_WRITE: Writes the current state to the pin.
 *
 *
 * @tparam TPIN Digital output pin number.
 */
template <uint8_t TPIN, aui_gpio_type TTYPE, typename... TARGS>
class aui_gpio : public IElement {
public:
    using value_type = bool;

    aui_gpio() = default;
    uint8_t handle_message(IElement*, uint8_t, void*, uint16_t) override {
        // Unsupported type → compile-time error if instantiated
        static_assert(TTYPE != TTYPE, "Unsupported GPIO type");
        return 1;
    }

    value_type min() { return false; }
    value_type max() { return true; }
    value_type current() { return false; }
};

// DIGITAL OUTPUT
template <uint8_t TPIN>
class aui_gpio<TPIN, aui_gpio_type::digital_output> : public IElement {
public:
    using value_type = bool;

    uint8_t handle_message(IElement*, uint8_t msg, void* arg, uint16_t size) override {
        if (msg == MSG_ONSETUP) {
            pinMode(TPIN, OUTPUT);
            return 0;
        }

        if (msg == MSG_GPIO_WRITE) {
            if (!arg || size < 1) return 1;
            uint8_t v = *static_cast<uint8_t*>(arg);
            digitalWrite(TPIN, v ? HIGH : LOW);
            return 0;
        }

        return 0;
    }

    value_type min() { return false; }
    value_type max() { return true; }
    value_type current() { return digitalRead(TPIN) == HIGH; }
};

namespace detail {
    template <aui_gpio_resolution R>
    struct aui_gpio_value_type;

    template <>
    struct aui_gpio_value_type<aui_gpio_resolution::resolution_8bit> {
        using type = uint8_t;
    };

    template <>
    struct aui_gpio_value_type<aui_gpio_resolution::resolution_10bit> {
        using type = uint16_t;
    };

    template <>
    struct aui_gpio_value_type<aui_gpio_resolution::resolution_12bit> {
        using type = uint16_t;
    };

}


template <uint8_t TPIN, aui_gpio_resolution TRESOLUTION = AUI_DEFAULT_ANALOG_RES>
class aui_gpio<TPIN, aui_gpio_type::analog_output> : public IElement {
public:
    using value_type = typename detail::aui_gpio_value_type<TRESOLUTION>::type;

    uint8_t handle_message(IElement*, uint8_t msg, void* arg, uint16_t size) override {
        if (msg == MSG_ONSETUP) {
            pinMode(TPIN, OUTPUT);
            return 0;
        }

        if (msg == MSG_GPIO_WRITE) {
            if (!arg || size < sizeof(value_type)) return 1;

            m_value = *static_cast<value_type*>(arg);
            if (m_value > (value_type)TRESOLUTION)
                m_value = (value_type)TRESOLUTION;

            analogWrite(TPIN, m_value);
            return 0;
        }
        return 0;
    }

    value_type min() { return 0; }
    value_type max() { return (value_type)TRESOLUTION; }
    value_type current() { return (value_type)m_value; }
private:
     value_type m_value = 0;
};

template <uint8_t TPIN>
class aui_gpio<TPIN, aui_gpio_type::digital_input_pullup> : public IElement {
public:
    using value_type = bool;

    uint8_t handle_message(IElement*, uint8_t msg, void* arg, uint16_t size) override {
        if (msg == MSG_ONSETUP) {
            pinMode(TPIN, INPUT_PULLUP);
            return 0;
        }

        if (msg == MSG_GPIO_READ) {
            if (!arg || size < 1) return 1;
            *static_cast<uint8_t*>(arg) = digitalRead(TPIN);
            return 0;
        }

        return 0;
    }

    value_type min() { return false; }
    value_type max() { return true; }
    value_type current() { return digitalRead(TPIN) == HIGH; }
};

template <uint8_t TPIN>
class aui_gpio<TPIN, aui_gpio_type::digital_input> : public IElement {
public:
    using value_type = bool;

    aui_gpio() = default;

    uint8_t handle_message(IElement*, uint8_t msg, void* arg, uint16_t size) override {
        if (msg == MSG_ONSETUP) {
            pinMode(TPIN, INPUT);
            return 0;
        }

        if (msg == MSG_GPIO_READ) {
            if (!arg || size < 1) return 1;
            *static_cast<uint8_t*>(arg) = digitalRead(TPIN);
            return 0;
        }

        return 0;
    }

    value_type min() { return false; }
    value_type max() { return true; }
    value_type current() { return digitalRead(TPIN) == HIGH; }
};

template <uint8_t TPIN>
using aui_gpio_aout8 = aui_gpio<TPIN, aui_gpio_type::analog_output, aui_gpio_resolution::resolution_8bit>;

template <uint8_t TPIN>
using aui_gpio_aout10 = aui_gpio<TPIN, aui_gpio_type::analog_output, aui_gpio_resolution::resolution_10bit>;

template <uint8_t TPIN>
using aui_gpio_aout12 = aui_gpio<TPIN, aui_gpio_type::analog_output, aui_gpio_resolution::resolution_12bit>;

template <uint8_t TPIN>
using aui_gpio_aout = aui_gpio<TPIN, aui_gpio_type::analog_output, AUI_DEFAULT_ANALOG_RES>;

template <uint8_t TPIN>
using aui_gpio_dout =  aui_gpio<TPIN, aui_gpio_type::digital_output>;

template <uint8_t TPIN>
using aui_gpio_din =  aui_gpio<TPIN, aui_gpio_type::digital_input>;

template <uint8_t TPIN>
using aui_gpio_din_pullup =  aui_gpio<TPIN, aui_gpio_type::digital_input_pullup>;