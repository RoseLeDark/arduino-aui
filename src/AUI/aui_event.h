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

/**
 * @brief Generic event container used to pass raw argument data between elements.
 *
 * aui_event stores a raw pointer and its size. It provides typed access
 * through get_as<T>() and allows writing values into the payload via set_as<T>().
 * The event does not own the memory; the sender is responsible for providing
 * a valid buffer.
 */
class aui_event {
protected:
    void* m_pRaw;        ///< Pointer to raw payload data
    uint16_t m_tSize;    ///< Size of the payload in bytes

    /**
     * @brief Internal constructor used by factory methods.
     * @param args Pointer to raw payload data
     * @param size Size of the payload in bytes
     */
    aui_event(void* args, uint16_t size)
        : m_pRaw(args), m_tSize(size) { }
public:
    /**
     * @brief Constructs an empty event with no payload.
     */
    aui_event() : m_pRaw(nullptr), m_tSize(0) { }

    /**
     * @brief Returns the payload as a typed pointer.
     * @tparam U Type to interpret the payload as
     * @return Pointer to payload cast to U*
     */
    template <typename U>
    U* get_as() { return static_cast<U*>(m_pRaw); }

    /**
     * @brief Writes a typed value into the payload buffer.
     * @tparam U Type of the value to write
     * @param value Value to copy into the payload
     *
     * Copies sizeof(U) bytes into the raw buffer. The caller must ensure
     * that the buffer is large enough.
     */
    template <typename U>
    void set_as(U value) { memcpy(m_pRaw, &value, sizeof(U)); }

    /**
     * @brief Returns the size of the payload buffer.
     */
    uint16_t get_size() { return m_tSize; }

    /**
     * @brief Creates an event from a message handler.
     */
    static aui_event make(void* args, uint16_t size) {
        return aui_event(args, size);
    }

    /**
     * @brief Creates an event from SendMessage.
     */
    static aui_event make(void* args, uint16_t size, uint16_t) {
        return aui_event(args, size);
    }

    /**
     * @brief Creates an empty event with no payload.
     */
    static aui_event make() {
        return aui_event(nullptr, 0);
    }

    /**
     * @brief Returns true if the event has no payload.
     */
    operator bool () { return m_pRaw == nullptr; }
};

/**
 * @brief Universal ID-based event used throughout the AUI framework.
 *
 * aui_idble_event is the primary event type for all components in the
 * AUI (Arduino User Interface) system that are identified by a compile-time
 * ID. This includes all elements derived from IVisualElement<TID> as well as
 * any logical element that participates in the event system.
 *
 * The event may optionally carry a payload. The payload memory is not owned
 * by the event; the sender must provide a valid buffer for the duration of
 * dispatch. Typed access is provided through get_as<T>().
 */
class aui_idble_event {
protected:
    union {
        struct {
            uint16_t m_id;   ///< Identifier associated with the event
            void* m_rArgs;   ///< Pointer to argument payload
        };
        void* m_pRaw;         ///< Raw pointer used when receiving events
    };

    uint16_t m_pRawSize;   ///< Size of the raw event block
    uint16_t m_rArgsSize;  ///< Size of the argument payload

    /**
     * @brief Constructor used when receiving events (Message Handle).
     * @param args Pointer to raw event block
     * @param size Size of the raw block
     */
    aui_idble_event(void* args, uint16_t size)
        : m_pRaw(args), m_pRawSize(size), m_rArgsSize(sizeof(m_rArgs)) { }

    /**
     * @brief Constructor used when sending events (SendMessage).
     * @param id Identifier associated with the event
     * @param args Pointer to argument payload
     * @param size Size of the argument payload
     */
    aui_idble_event(uint16_t id, void* args, uint16_t size)
        : m_id(id), m_rArgs(args), m_pRawSize(sizeof(m_pRaw)), m_rArgsSize(size) { }

public:
    /**
     * @brief Returns the size of the argument payload.
     */
    constexpr uint16_t get_size() const { return m_rArgsSize; }

    /**
     * @brief Returns the event identifier.
     */
    constexpr uint16_t get_id() const { return m_id; }

    /**
     * @brief Returns the argument payload as a typed pointer.
     * @tparam U Type to interpret the payload as
     * @return Pointer to payload cast to U*
     */
    template <typename U>
    U* get_as() const { return static_cast<U*>(m_rArgs); }

    template <typename U>
    void set_as(U value) { memcpy(m_rArgs, &value, sizeof(U)); }

    /**
     * @brief Creates an event from a message handler.
     */
    static aui_idble_event make(void* args, uint16_t size) {
        return aui_idble_event(args, size);
    }

    /**
     * @brief Creates an event from SendMessage with payload.
     */
    static aui_idble_event make(void* args, uint16_t size, uint16_t id) {
        return aui_idble_event(id, args, size);
    }

    /**
     * @brief Creates an event with ID but no payload.
     */
    static aui_idble_event make(uint16_t id) {
        return aui_idble_event(id, nullptr, 0);
    }
};



class aui_uart_event {
protected:
    union {
        struct {
            uint16_t m_id;   ///< Identifier associated with the event
            uint8_t m_loLevel; ///< Log Level to written to UART
            char m_chText[32];   ///< Text to writte to UART
        };
        void* m_pRaw;         ///< Raw pointer used when receiving events
    };

    uint16_t m_pRawSize;   ///< Size of the raw event block
    uint16_t m_rStringLen;  ///< Size of the string payload

    /**
     * @brief Constructor used when receiving events (Message Handle).
     * @param args Pointer to raw event block
     * @param size Size of the raw block
     */
    aui_uart_event(void* args, uint16_t size)
        : m_pRaw(args), m_pRawSize(size), m_rStringLen(strlen(m_chText)) { }

    /**
     * @brief Constructor used when sending events (SendMessage).
     */
    aui_uart_event(uint16_t id, const char* string, uint8_t loLevel)
        : m_id(id), m_loLevel(loLevel) { strcpy(m_chText, string); m_rStringLen = strlen(m_chText); m_pRawSize = sizeof(m_pRaw); }

    aui_uart_event(uint16_t id, uint16_t size)
        : m_id(id), m_loLevel(0), m_rStringLen(size) { m_pRawSize = sizeof(m_pRaw);  }
public:
    /**
     * @brief Returns the lenght of the argument string.
     */
    constexpr uint16_t get_string_lenght() const { return m_rStringLen; }

    /**
     * @brief Returns the event identifier.
     */
    constexpr uint16_t get_id() const { return m_id; }

    constexpr uint8_t get_loglevel() const { return m_loLevel; }
    /**
     * @brief Returns the argument payload as a typed pointer.
     * @tparam U Type to interpret the payload as
     * @return Pointer to payload cast to U*
     */
    template <typename U>
    const char* get_as() const { return m_chText; }

    void set(const char* chr, uint16_t  len) {
        strcpy(m_chText, chr);
       m_rStringLen = len;
    }
    /**
     * @brief Creates an event from a message handler.
     */
    static aui_uart_event make(void* args, uint16_t size) {
        return aui_uart_event(args, size);
    }

    /**
     * @brief Creates an event from SendMessage with payload.
     */
    static aui_uart_event make(uint16_t id, const char* string, uint8_t loLevel) {
        return aui_uart_event(id, string, loLevel);
    }

    /**
     * @brief Creates an event with ID but no payload.
     */
    static aui_uart_event make(uint16_t id, uint16_t lenghtToRRead) {
        return aui_uart_event(id, lenghtToRRead);
    }
};


class aui_pcint_event {
protected:
    union {
        struct {
            uint8_t m_id;  
            uint8_t m_state;   
        };
        void* m_pRaw;         ///< Raw pointer used when receiving events
    };
    uint16_t m_iSize;

    aui_pcint_event(void* args, uint16_t size) 
        : m_pRaw(args), m_iSize(size) { }

    aui_pcint_event(uint16_t id, uint8_t state) 
        : m_id(id), m_state(state), m_iSize(sizeof(m_pRaw))  { }
public:
    constexpr uint8_t get_id() const  { return m_id; }
    constexpr uint8_t get_state() const  { return m_state; }

    /**
     * @brief Creates an event from a message handler.
     */
    static aui_pcint_event make(void* args, uint16_t size) {
        return aui_pcint_event(args, size);
    }

    /**
     * @brief Creates an event from SendMessage with payload.
     */
    static aui_pcint_event make(uint16_t id, uint8_t state) {
        return aui_pcint_event(id, state);
    }
};