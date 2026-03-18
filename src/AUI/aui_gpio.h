// SPDX-License-Identifier: EUPL-1.2
//
// This file is part of AUI.
// AUI is distributed under the terms of the European Union Public License v1.2.
// You may obtain a copy of the license at:
// https://joinup.ec.europa.eu/collection/eupl/eupl-text-eupl-12


#pragma once 


#include "aui_element.h"
#include "aui_system.h"

/**
 * @enum aui_gpio_resolution
 * @brief Defines the numeric resolution used by analog GPIO elements.
 *
 * The numeric values correspond directly to the maximum representable output
 * value for the given resolution. These constants are used for clamping,
 * scaling, and selecting the appropriate value_type in analog GPIO classes.
 *
 * Values:
 *  - resolution_1bit  = 1      (digital-style on/off)
 *  - resolution_8bit  = 255    (0–255 PWM)
 *  - resolution_10bit = 1023   (0–1023 PWM/ADC)
 *  - resolution_12bit = 4095   (0–4095 DAC/ADC)
 */
enum class aui_gpio_resolution : uint16_t {
    resolution_1bit = 1,
    resolution_8bit = 255,
    resolution_10bit = 1023,
    resolution_12bit = 4095
};

/**
 * @namespace detail
 * @brief Internal utilities for GPIO type resolution.
 *
 * aui_gpio_value_type maps a compile-time resolution constant to the smallest
 * integer type capable of representing its full numeric range. This ensures
 * that analog GPIO elements use efficient and correct value types without
 * exposing implementation details to the public API.
 *
 * Mappings:
 *  - 1‑bit and 8‑bit resolutions  → uint8_t
 *  - 10‑bit and 12‑bit resolutions → uint16_t
 */
namespace detail {

    template <aui_gpio_resolution R>
    struct aui_gpio_value_type;

    /**
     * @brief Maps 1‑bit resolution to uint8_t.
     */
    template <>
    struct aui_gpio_value_type<aui_gpio_resolution::resolution_1bit> {
        using value_type = uint8_t;
    };
    /**
     * @brief Maps 8‑bit resolution to uint8_t.
     */
    template <>
    struct aui_gpio_value_type<aui_gpio_resolution::resolution_8bit> {
        using value_type = uint8_t;
    };
    /**
     * @brief Maps 10‑bit resolution to uint16_t.
     */
    template <>
    struct aui_gpio_value_type<aui_gpio_resolution::resolution_10bit> {
        using value_type = uint16_t;
    };
    /**
     * @brief Maps 12‑bit resolution to uint16_t.
     */
    template <>
    struct aui_gpio_value_type<aui_gpio_resolution::resolution_12bit> {
        using value_type = uint16_t;
    };

    void aui_gpio_pcint_register_pin(uint8_t pinmask, uint8_t pcint_id);

    template <uint8_t TPIN>
    class aui_base_pin  {
    public:
        void set_pcint_enable(uint8_t pcintID) {
            aui_gpio_pcint_register_pin( TPIN, pcintID);         
        }
    };

    #if AUI_CONFIG_EXT_INTERRUPT == 1
        extern "C" AUI_ISR_ATTR void aui_isr_handler_pin() ;
    #endif
    template <uint8_t TPIN>
    class aui_interrupt_pin {
    public:
        uint8_t attache_interrupt(uint8_t mode, void (* handler)() = nullptr ) {
            int irq = digitalPinToInterrupt(TPIN);
            if (irq == NOT_AN_INTERRUPT) return 1;

            ::attachInterrupt(irq, handler == nullptr ? aui_isr_handler_pin :  handler, mode);
            return 0;
        }

        uint8_t detach_interrupt() {
            int irq = digitalPinToInterrupt(TPIN);
            if (irq == NOT_AN_INTERRUPT) return 1;

            ::detachInterrupt(irq);
            return 0;
        }
    private_static: 
    #if AUI_CONFIG_EXT_INTERRUPT == AUI_CONFIG_OFF
        static void aui_isr_handler_pin() {
            auisystem.send_massage<aui_idble_event>(&auisystem, MSG_INTERRUPT, aui_idble_event::make(TPIN));
        }
    #endif
    };
}



/**
 * @class aui_digital_output
 * @brief GPIO element representing a digital output pin.
 *
 * This class provides a deterministic, message-driven interface for controlling
 * a digital output pin. It supports switching, writing, and resetting the pin
 * state through AUI messages, and integrates with the visual element system
 * via IVisualElement<TPIN>.
 *
 * Template Parameter:
 *  - TPIN: The hardware pin number associated with this digital output.
 *
 * Behavior:
 *  - Maintains an internal boolean state (0 or 1).
 *  - Responds to MSG_GPIO_SWITCH, MSG_GPIO_WRITE, and MSG_GPIO_RESET.
 *  - Performs pinMode(TPIN, OUTPUT) during on_begin().
 *  - Exposes implicit conversion operators for convenience.
 */
template <uint8_t TPIN>
class aui_digital_output : public IVisualElement<TPIN>, 
                           public  detail::aui_base_pin<TPIN>,
                           public detail::aui_interrupt_pin<TPIN> {
public:
    /**
     * @brief Base class providing message dispatch and optional rendering hooks.
     */
    using base_type = IVisualElement<TPIN>;

    /**
     * @brief Value type for digital output (always uint8_t with range 0–1).
     */
    using value_type = typename detail::aui_gpio_value_type<aui_gpio_resolution::resolution_1bit>::value_type;

    /**
     * @brief Constructs a digital output element with an initial LOW state.
     */
    aui_digital_output() : m_value(0) { }

    /**
     * @brief Returns the minimum representable value (0).
     */
    value_type get_min() { return 0; }
    /**
     * @brief Returns the maximum representable value (1).
     */
    value_type get_max() { return 1; }

    /**
     * @brief Handles incoming AUI messages for this GPIO element.
     *
     * Supported messages:
     *  - MSG_GPIO_SWITCH → toggles the pin state.
     *  - MSG_GPIO_WRITE  → writes a specific value.
     *  - MSG_GPIO_RESET  → re-applies the current state to the hardware pin.
     *
     * @return 0 if handled, 1 if ignored.
     */
    virtual uint8_t handle_message(const IElement* e, const uint8_t msg, void* args, const uint16_t size) override {
         if(base_type::handle_message(e, msg, args, size) == 0 ) return 0;

        if(msg == MSG_GPIO_SWITCH) {
            return on_gpio_switch(e, static_cast<aui_idble_event*>(args));
        }
        if(msg == MSG_GPIO_WRITE) {
            return on_gpio_write(e, static_cast<aui_idble_event*>(args) );
        }
        if (msg == MSG_RESET ) { 
            return on_reset(e, static_cast<aui_idble_event*>(args)  ); 
        }
        return 1;
    }

    
    operator bool () { return m_value == 1; }
    operator value_type () {return m_value; }


protected:
    /**
     * @brief Initializes the hardware pin as OUTPUT.
     */
    uint8_t on_begin(const IElement* sender, aui_event* event ) override { 
        
        pinMode(TPIN, OUTPUT);
        return 0; 
    }
    /**
     * @brief Toggles the digital output state.
     */
    virtual uint8_t on_gpio_switch(const IElement* sender, aui_idble_event* event) { 
        m_value = !m_value;
        digitalWrite(TPIN, m_value);
        return 0; 
    }
    /**
     * @brief Writes a new value to the pin.
     */
    virtual uint8_t on_gpio_write(const IElement* sender, aui_idble_event* event)  { 
        if(event->get_id() != TPIN) return 1;
        m_value = *event->get_as<value_type>();

        digitalWrite(TPIN, m_value );
        return 0; 
    }
    /**
     * @brief Re-applies the current state to the hardware pin.
     */
    virtual uint8_t on_reset(const IElement* sender, aui_idble_event* event)  { 
        if(event->get_id() != TPIN) return 1; 
        digitalWrite(TPIN, m_value );
        return 0;
    }
protected:
    /**
     * @brief Cached digital output state (0 or 1).
     */
    value_type m_value;

private:

};



template <uint8_t TPIN, uint8_t TID = TPIN>
class aui_digital_input : public IElementWithID<TID>, 
                          public  detail::aui_base_pin<TPIN>,
                          public detail::aui_interrupt_pin<TPIN>  {
public:
    /**
     * @brief Base class providing message dispatcher .
     */
    using base_type = IElementWithID<TID> ;

    /**
     * @brief Value type for digital output (always uint8_t with range 0–1).
     */
    using value_type = typename detail::aui_gpio_value_type<aui_gpio_resolution::resolution_1bit>::value_type;

    /**
     * @brief Constructs a digital output element with an initial LOW state.
     */
    aui_digital_input() : m_value(0) { }

    /**
     * @brief Returns the minimum representable value (0).
     */
    value_type get_min() { return 0; }
    /**
     * @brief Returns the maximum representable value (1).
     */
    value_type get_max() { return 1; }

    /**
     * @brief Handles incoming AUI messages for this GPIO element.
     *
     * Supported messages:
     *  - MSG_GPIO_READ  → read a specific digital pin.
     *
     * @return 0 if handled, 1 if ignored.
     */
    virtual uint8_t handle_message(const IElement* e, const uint8_t msg, void* args, const uint16_t size) override {
        if(base_type::handle_message(e, msg, args, size) == 0 ) return 0;

        if(msg == MSG_GPIO_READ) {
            return on_gpio_read(e, static_cast<aui_idble_event*>(args) );
        }
        return 1;
    }

    operator bool () { return m_value == 1; }
    operator value_type () {return m_value; }
protected:
    /**
     * @brief Initializes the hardware pin as INPUT.
     */
    virtual uint8_t on_begin(const IElement* sender, aui_event* event ) override { 
        pinMode(TPIN, INPUT);
        m_value = digitalRead(TPIN);
        return 0; 
    }
    /**
     * @brief read the digital input state.
     */
    virtual uint8_t on_gpio_read(const IElement* sender, aui_idble_event* event) { 
        if(event->get_id() != TID ) return 1;

        m_value = digitalRead(TPIN);
        event->set_as<value_type>(m_value);
        return 0; 
    }
protected:
    /**
     * @brief Cached digital input state (0 or 1).
     */
    value_type m_value;
};


template <uint8_t TPIN, uint8_t TID = TPIN>
class aui_digital_input_pullup : public IElementWithID<TID>, 
                                 public detail::aui_base_pin<TPIN>,
                                 public detail::aui_interrupt_pin<TPIN>  {
public:
    /**
     * @brief Base class providing message dispatcher .
     */
    using base_type = IElementWithID<TID> ;

    /**
     * @brief Value type for digital output (always uint8_t with range 0–1).
     */
    using value_type = typename detail::aui_gpio_value_type<aui_gpio_resolution::resolution_1bit>::value_type;

    /**
     * @brief Constructs a digital output element with an initial LOW state.
     */
    aui_digital_input_pullup() : m_value(0) { }

    /**
     * @brief Returns the minimum representable value (0).
     */
    value_type get_min() { return 0; }
    /**
     * @brief Returns the maximum representable value (1).
     */
    value_type get_max() { return 1; }

    /**
     * @brief Handles incoming AUI messages for this GPIO element.
     *
     * Supported messages:
     *  - MSG_GPIO_READ  → read a specific digital pin.
     *
     * @return 0 if handled, 1 if ignored.
     */
    virtual uint8_t handle_message(const IElement* e, const uint8_t msg, void* args, const uint16_t size) override {
        if(base_type::handle_message(e, msg, args, size) == 0 ) return 0;

        if(msg == MSG_GPIO_READ) {
            return on_gpio_read(e, static_cast<aui_idble_event*>(args) );
        }
        return 1;
    }

    operator bool () { return m_value == 1; }
    operator value_type () {return m_value; }
protected:
    /**
     * @brief Initializes the hardware pin as INPUT.
     */
    virtual uint8_t on_begin(const IElement* sender, aui_event* event ) override { 
        pinMode(TPIN, INPUT_PULLUP);
        m_value = digitalRead(TPIN);
        return 0; 
    }
    /**
     * @brief read the digital input state.
     */
    virtual uint8_t on_gpio_read(const IElement* sender, aui_idble_event* event) { 
        if(event->get_id() != TID ) return 1;

        m_value = digitalRead(TPIN);
        event->set_as<value_type>(m_value);
        return 0; 
    }
protected:
    /**
     * @brief Cached digital input state (0 or 1).
     */
    value_type m_value;
};
/**
 * @class aui_analog_output
 * @brief GPIO element representing an analog/PWM output pin.
 *
 * This class provides a resolution-aware analog output interface. It supports
 * writing, switching, and resetting the output value through AUI messages.
 *
 * Template Parameters:
 *  - TPIN:        Hardware pin number.
 *  - TRESOLUTION: Compile-time analog resolution (8/10/12 bit).
 *
 * Behavior:
 *  - Uses the smallest suitable integer type for the selected resolution.
 *  - Clamps values to the valid range before writing.
 *  - Calls analogWrite(TPIN, value) for output.
 */
template <uint8_t TPIN, aui_gpio_resolution TRESOLUTION>
class aui_analog_output : public IVisualElement<TPIN>, 
                          public  detail::aui_base_pin<TPIN>
                #if AUI_CONFIG_ANALOG_INTERRUPT == 1
                         , public detail::aui_interrupt_pin<TPIN>
                #endif
{
    static_assert(
        TRESOLUTION != aui_gpio_resolution::resolution_1bit,
        "aui_analog_output does not support 1-bit resolution. Use aui_digital_output for digital (1-bit) GPIO modes." );

public:
    /**
     * @brief Base class providing message dispatch and optional rendering hooks.
     */
    using base_type = IVisualElement<TPIN>;
    /**
     * @brief Value type determined by the selected resolution.
     */
    using value_type = typename detail::aui_gpio_value_type<TRESOLUTION>::type;

    /**
     * @brief Returns the minimum representable value (0).
     */
    value_type get_min() { return false; }
    /**
     * @brief Returns the maximum representable value for the resolution.
     */
    value_type get_max() { return (value_type)TRESOLUTION; }

    /**
     * @brief Handles analog GPIO messages (switch, write, reset).
     */
    virtual uint8_t handle_message(const IElement* e, const uint8_t msg, void* args, const uint16_t size) override {
         if(base_type::handle_message(e, msg, args, size) == 0 ) return 0;

        if(msg == MSG_GPIO_SWITCH) {
            return on_gpio_switch(e, static_cast<aui_idble_event*>(args));
        }
        if(msg == MSG_GPIO_WRITE) {
            return on_gpio_write(e, static_cast<aui_idble_event*>(args) );
        }
        if (msg == MSG_RESET ) { 
            return on_reset(e, static_cast<aui_idble_event*>(args)  ); 
        }
        return 1;
    }

    operator bool () { return m_value == (value_type)TRESOLUTION; }
    operator value_type () {return m_value; }
protected:
    /**
     * @brief Initializes the hardware pin as OUTPUT.
     */
    uint8_t on_begin(const IElement* sender, aui_event* event ) override {  
        pinMode(TPIN, OUTPUT);
        return 0; 
    }
    /**
     * @brief Placeholder for switching behavior (not yet implemented).
     */
    virtual uint8_t on_gpio_switch(const IElement* sender, aui_idble_event* event) {
        if (event->get_id() != TPIN)
            return 1;

        const value_type max = get_max();   // z.B. 255, 1023, 4095

        // Analoge Spiegelung: neuer Wert = max - alter Wert
        m_value = max - m_value;

        analogWrite(TPIN, m_value);
        return 0;
    }

    /**
     * @brief Writes a new analog value to the pin, with clamping.
     */
    virtual uint8_t on_gpio_write(const IElement* sender, aui_idble_event* event)  { 
        if(event->get_id() != TPIN) return 1;
        m_value = *event->get_as<value_type>();

        if (m_value > (value_type)TRESOLUTION)
            m_value = (value_type)TRESOLUTION; 
        analogWrite(TPIN, m_value); 
        return 0; 
    }
    /**
     * @brief Re-applies the current analog value to the hardware pin.
     */
    virtual uint8_t on_reset(const IElement* sender, aui_idble_event* event)  { 
        if(event->get_id() != TPIN) return 1; 
        if (m_value > (value_type)TRESOLUTION)
            m_value = (value_type)TRESOLUTION; 
        analogWrite(TPIN, m_value); 

        return 0;
    }
    
protected:
    /**
     * @brief Cached analog output value.
     */
    value_type m_value;
};

template <uint8_t TPIN, aui_gpio_resolution TRESOLUTION, uint8_t TID = TPIN>
class aui_analog_input : public IVisualElement<TPIN>, 
                         public   detail::aui_base_pin<TPIN>
                #if AUI_CONFIG_ANALOG_INTERRUPT == 1
                         , public detail::aui_interrupt_pin<TPIN>
                #endif
{
    static_assert(
        TRESOLUTION != aui_gpio_resolution::resolution_1bit,
        "aui_analog_input does not support 1-bit resolution. Use aui_digital_output for digital (1-bit) GPIO modes." );

public:
    /**
     * @brief Base class providing message dispatch and optional rendering hooks.
     */
    using base_type = IVisualElement<TPIN>;
    /**
     * @brief Value type determined by the selected resolution.
     */
    using value_type = typename detail::aui_gpio_value_type<TRESOLUTION>::type;

    /**
     * @brief Returns the minimum representable value (0).
     */
    value_type get_min() { return 0; }
    /**
     * @brief Returns the maximum representable value for the resolution.
     */
    value_type get_max() { return (value_type)TRESOLUTION; }

    /**
     * @brief Handles analog GPIO messages (switch, write, reset).
     */
    virtual uint8_t handle_message(const IElement* e, const uint8_t msg, void* args, const uint16_t size) override {
         if(base_type::handle_message(e, msg, args, size) == 0 ) return 0;

        if(msg == MSG_GPIO_READ) {
            return on_gpio_read(e, static_cast<aui_idble_event*>(args) );
        }

        return 1;
    }

    operator bool () { return m_value == (value_type)TRESOLUTION; }
    operator value_type () {return m_value; }
protected:
    /**
     * @brief Initializes the hardware pin as OUTPUT.
     */
    uint8_t on_begin(const IElement* sender, aui_event* event ) override {  
        pinMode(TPIN, INPUT);
        m_value = analogRead(TPIN);

        if (m_value > (value_type)TRESOLUTION)
            m_value = (value_type)TRESOLUTION;
        
        return 0; 
    }


    /**
     * @brief Writes a new analog value to the pin, with clamping.
     */
    virtual uint8_t on_gpio_read(const IElement* sender, aui_idble_event* event)  { 
        if(event->get_id() != TPIN) return 1;

        m_value = analogRead(TPIN);

        if (m_value > (value_type)TRESOLUTION)
            m_value = (value_type)TRESOLUTION;

        event->set_as<value_type>(m_value);
        return 0; 
    }

    
protected:
    /**
     * @brief Cached analog output value.
     */
    value_type m_value;
};