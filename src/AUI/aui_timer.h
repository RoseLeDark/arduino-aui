// SPDX-License-Identifier: EUPL-1.2
//
// This file is part of AUI.
// AUI is distributed under the terms of the European Union Public License v1.2.
// You may obtain a copy of the license at:
// https://joinup.ec.europa.eu/collection/eupl/eupl-text-eupl-12

#pragma once 

#include "aui_messages.h"
#include "aui_element.h"
#include "aui_system.h"

extern "C" void AUI_WEAK aui_timer0_callback(unsigned long now);

#if AUI_CONFIG_TIMER_OFF != AUI_CONFIG_ON
extern "C" void AUI_WEAK aui_timer_callback();
#endif


#define AUI_TIMER16_PRESCALER_8     0b010
#define AUI_TIMER16_PRESCALER_64    0b011
#define AUI_TIMER16_PRESCALER_256   0b100
#define AUI_TIMER16_PRESCALER_1024  0b101


enum class aui_timer_hz : uint8_t {
    Hz1000,   // 1 kHz
    Hz500,    // 500 Hz
    Hz100,    // 100 Hz
    Hz50,     // 50 Hz
    Hz20      // 20 Hz
};



template <uint8_t TID, aui_timer_hz THZ, uint16_t TTICKS>
class aui_timer : public IElementWithID<TID> {
protected:
    static uint8_t m_intrPrtFlag;
public:
    using base_type = IElementWithID<TID>;
    using this_type = aui_timer<TID, THZ, TTICKS>;

    aui_timer() {
        switch (THZ) {
            case aui_timer_hz::Hz1000:
                m_uiPrescaler = AUI_TIMER16_PRESCALER_8; // (1 << CS11); // 8
                m_uiOcr = 1999;               // 1 kHz
                break;

            case aui_timer_hz::Hz500:
                m_uiPrescaler = AUI_TIMER16_PRESCALER_8; // (1 << CS11);   // Prescaler 
                m_uiOcr = 3999;         // wrap, aber ok
                break;

            case aui_timer_hz::Hz100:
                m_uiPrescaler =  AUI_TIMER16_PRESCALER_64; //(1 << CS11) | (1 << CS10); // Prescaler 64
                m_uiOcr = 2499;
                break;

            case aui_timer_hz::Hz50:
                m_uiPrescaler = AUI_TIMER16_PRESCALER_64; //(1 << CS11) | (1 << CS10); // Prescaler 64
                m_uiOcr = 4999;
                break;

            case aui_timer_hz::Hz20:
                m_uiPrescaler = AUI_TIMER16_PRESCALER_256; //(1 << CS12); // Prescaler 256
                m_uiOcr = 3124;
                break;
        }
    }
    void end() {
        cli();
        TIMSK1 &= ~(1 << OCIE1A);
        sei();
    }

    // Nur für OneShot relevant
    void reset() {
        cli();
        TIMSK1 |= (1 << OCIE1A);    // Interrupt wieder aktivieren
        sei();
    }

    // Wird von der ISR aufgerufen
    static void isr_handler() {
        if (TTICKS > 0){
            // Timer deaktivieren
            TIMSK1 &= ~(1 << OCIE1A);
        }
        if(this_type::m_intrPrtFlag == 0)
            this_type::m_intrPrtFlag = 1;
    }
protected:
    /**
     * @brief initialization hook.
     *
     * Called automatically when the element receives MSG_ONSETUP.
     */
    uint8_t on_begin(const IElement* sender, const aui_event* event) override {
        cli();
        TCCR1A = 0;
        TCCR1B = (1 << WGM12);

        if  (TTICKS > 0) OCR1A  = TTICKS;
        else OCR1A   = m_uiOcr;

        TCCR1B |= m_uiPrescaler;
        TIMSK1 |= (1 << OCIE1A);
        sei();

         return 0;
    }

    uint8_t on_disable(const IElement* sender, const uint8_t ID)  {
        base_type::on_disable(sender, ID);
        if(ID == TID) {
            cli();
            TIMSK1 &= ~(1 << OCIE1A);
            TCCR1B  &= ~0x07;
            sei();
        }
        return 0; 
    }
    uint8_t on_enable(const IElement* sender, const uint8_t ID)  { 
        base_type::on_enable(sender, ID);
        if(ID == TID) {
            cli();
            TCCR1B  |= m_uiPrescaler;   // Timer wieder starten
            TIMSK1 |= (1 << OCIE1A);   // Interrupt wieder aktivieren
            sei();
        }
        return 0;  
    }
    uint8_t on_update(const IElement* sender, const uint64_t ticks) { 
        if(this_type::m_intrPrtFlag == 1) {
            this_type::m_intrPrtFlag = 0;

            auisystem.send_massage<aui_idble_event>(this, MSG_TIMER_INTER, aui_idble_event(TID, 1));
            return 0;
        }
        return 1;
    }
protected:
    uint8_t m_uiPrescaler = 0;
    uint16_t  m_uiOcr = 0;
};

#if AUI_CONFIG_TIMER_OFF != AUI_CONFIG_ON
    template <uint8_t TID, uint16_t TTICKS>
    using aui_timer_t = aui_timer<TID, AUI_CONFIG_TIMER_HZ, TTICKS>;
#endif