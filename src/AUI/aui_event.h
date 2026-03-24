// SPDX-License-Identifier: EUPL-1.2
//
// This file is part of AUI.
// AUI is distributed under the terms of the European Union Public License v1.2.
// You may obtain a copy of the license at:
// https://joinup.ec.europa.eu/collection/eupl/eupl-text-eupl-12

#pragma once 

#include <Arduino.h>
#include <string.h>
#include "aui_messages.h"

template <typename TPAYLOAD>
class aui_event {
private:
    union {
        struct {
            uint16_t m_id;   ///< Identifier associated with the event
            TPAYLOAD m_rArgs;   ///< Pointer to argument payload
        };
        void* m_pRaw;         ///< Raw pointer used when receiving events
    };
    uint16_t m_sizeRaw;
public:
    using value_type = TPAYLOAD;
    using reference = TPAYLOAD&;

    aui_event(value_type payload) 
        : m_id(0), m_rArgs (payload)  { }

    aui_event(uint8_t id, void* args, uint16_t size )
        :  m_id(id), m_pRaw(args), m_sizeRaw(size) { }

    aui_event(uint8_t id, value_type payload) 
        : m_id(id), m_rArgs (payload) { }

    
    reference get_payload() { return *m_rArgs; }
    reference get_payload() const { return *m_rArgs; }

    void set(TPAYLOAD arg) { m_rArgs = arg; }

    uint16_t get_id() { return m_id; }
    uint16_t get_id() const { return m_id; }
};

template <>
class aui_event<void*> {
private:
    struct {
        uint16_t m_id;   ///< Identifier associated with the event
        void* m_rArgs;   ///< Pointer to argument payload
    };
    uint16_t m_sizeRaw;
public:
    using value_type = void*;
    using reference = void*&;

    aui_event(value_type payload) 
        : m_id(0), m_rArgs (payload)  { }

    aui_event(uint8_t id, void* args, uint16_t size )
        :  m_id(id), m_sizeRaw(size) { }

    aui_event(uint8_t id, value_type payload) 
        : m_id(id), m_rArgs (payload) { }

    
    reference get_payload() { return m_rArgs; }

    void set(void* arg) { m_rArgs = arg; }

    template<typename U>
    void set_as(U* value) { memcpy(m_rArgs, &value, sizeof(U)); }

    template <typename U>
    U* get_as() const { return static_cast<U*>(m_rArgs); }

    uint16_t get_id() { return m_id; }
    uint16_t get_id() const { return m_id; }
};



using  aui_pcint_payload = uint16_t;
using  aui_idble_payload = void*;

struct aui_uart_payload {
    uint8_t loLevel; ///< Log Level to written to UART
    char m_chText[32];   ///< Text to writte to UART
    const uint16_t len = 32;
};