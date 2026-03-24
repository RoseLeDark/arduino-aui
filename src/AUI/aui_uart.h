// SPDX-License-Identifier: EUPL-1.2
//
// This file is part of AUI.
// AUI is distributed under the terms of the European Union Public License v1.2.
// You may obtain a copy of the license at:
// https://joinup.ec.europa.eu/collection/eupl/eupl-text-eupl-12


#pragma once

#include "aui_element.h"
#include "Arduino.h"

/**
 * @brief Logging level constants for aui_serial.
 *
 * These levels control which UART text messages are processed and printed.
 */
#define AUI_LOG_LEVEL_NOOUTPUT 0 
#define AUI_LOG_LEVEL_ERRORS 1
#define AUI_LOG_LEVEL_INFOS 2
#define AUI_LOG_LEVEL_DEBUG 3
#define AUI_LOG_LEVEL_VERBOSE 4

/**
 * @brief Global compile‑time log level for all aui_serial instances.
 */
#define AUI_LOGLEVEL AUI_LOG_LEVEL_INFOS 

/**
 * @class aui_serial
 * @brief Generic UART interface element for the AUI system.
 *
 * This class integrates a HardwareSerial instance into the AUI message bus.
 * It does NOT implement buffering, parsing, or protocol handling.
 * Instead, it provides a minimal and deterministic interface:
 *
 *  - Initializes UART on MSG_ONSETUP
 *  - Emits raw RX characters via MSG_UART_RAW_RX
 *  - Handles basic text messages (info, error, debug, verbose)
 *  - Provides a virtual on_text() callback for formatted output
 *
 * @tparam TSerial Pointer to a HardwareSerial instance (e.g. &Serial)
 * @tparam TBAUD   UART baud rate (default: 9600)
 * @tparam TConfig UART configuration (default: SERIAL_8N1)
 */
template <HardwareSerial* TSerial, uint8_t TID, uint32_t TBAUD = 9600, uint32_t TConfig = SERIAL_8N1>
class aui_serial : public IElementWithID<TID> {
public:
    using base_type = IElementWithID<TID>;

    /**
     * @brief Handles incoming AUI messages.
     *
     * Supported messages:
     *  - MSG_ONSETUP: Initializes the UART interface.
     *  - MSG_UART_INFO_TEXT: Prints an info message if log level allows it.
     *  - MSG_UART_ERROR_TEXT: Prints an error message if log level allows it.
     *  - MSG_UART_DEBUG_TEXT: Prints a debug message if log level allows it.
     *  - MSG_UART_VERBOSE_TEXT: Prints a verbose message if log level allows it.
     *
     * @param sender Pointer to the element that sent the message.
     * @param msg    Message ID.
     * @param arg    Optional message payload (usually a char*).
     * @param size   Size of the payload.
     * @return Always returns 0 for handled messages, 1 otherwise.
     */
    auier_t handle_message(const IElement* sender, const uint8_t msg, void* arg, const uint16_t size) override;

    /**
     * @brief Reads all available UART bytes and forwards them as raw events.
     *
     * This method should be called from the global serialEvent() hook.
     * Each received character is forwarded as:
     *  - MSG_UART_RAW_RX
     *
     * @note This function does not buffer or parse data.
     */
    void on_serial_event();
protected:
    /**
     * @brief Initializes the hardware pin as OUTPUT.
     */
    auier_t on_begin(const IElement* sender, const aui_event_ex<aui_idble_payload>* event ) override {  
        TSerial->begin(TBAUD, TConfig);
        return AUI_OK;; 
    }

    virtual uint8_t on_write(const IElement* sender, const aui_event_ex<aui_uart_payload>* event);

    virtual uint8_t on_read_text(const IElement* sender, aui_event_ex<aui_uart_payload>* event) {
        if(event->get_id() != TID) return 1;

        uint16_t readle = 0;
        String read = "";
        char r ;


        while(event->get_payload().len < readle) {
            while(Serial.available() > 0);

            r = (char)Serial.read();
            if (r == '\n' || r == '\t' || r == '\r') {
                break;
            } else {
                read += r;
            }
            readle++;
        }

        strcpy(event->get_payload().m_chText, read.begin());

        return AUI_OK;;
    }
};

// ---------------------------------------------------------------------------
// Implementation
// ---------------------------------------------------------------------------

template <HardwareSerial* TSerial, uint8_t TID, uint32_t TBAUD , uint32_t TConfig >
void aui_serial<TSerial, TID, TBAUD, TConfig>::on_serial_event() {
        while (TSerial->available()) {
        char c = TSerial->read();
        auisystem.send_massage(nullptr, MSG_UART_RAW_RX, &c, 1);
    }
}

template <HardwareSerial* TSerial, uint8_t TID, uint32_t TBAUD , uint32_t TConfig >
uint8_t aui_serial<TSerial, TID,TBAUD, TConfig>::on_write(const IElement* sender, const aui_event_ex<aui_uart_payload>* event) {
    if(event->get_id() != TID) return 1;

    if ( event->get_payload().loLevel == AUI_LOG_LEVEL_ERRORS && AUI_LOGLEVEL <= AUI_LOG_LEVEL_ERRORS) {
        TSerial->println("[ " + String(millis()) + " ] error: " + event->get_payload().m_chText );
    } else if ( event->get_payload().loLevel == AUI_LOG_LEVEL_INFOS  && AUI_LOGLEVEL <= AUI_LOG_LEVEL_INFOS) {
        TSerial->println("[ " + String(millis()) + " ] info: " + event->get_payload().m_chText );
    } else if ( event->get_payload().loLevel == AUI_LOG_LEVEL_DEBUG  && AUI_LOGLEVEL <= AUI_LOG_LEVEL_DEBUG) {
        TSerial->println("[ " + String(millis()) + " ] debug: " + event->get_payload().m_chText );
    } else if ( event->get_payload().loLevel == AUI_LOG_LEVEL_VERBOSE  && AUI_LOGLEVEL <= AUI_LOG_LEVEL_VERBOSE) {
        TSerial->println("[ " + String(millis()) + " ] verbose: " + event->get_payload().m_chText );
    }

    return AUI_OK;;
}

template <HardwareSerial* TSerial, uint8_t TID, uint32_t TBAUD , uint32_t TConfig >
uint8_t aui_serial<TSerial, TID,TBAUD, TConfig>::handle_message(const IElement* sender, const uint8_t msg, void* arg, const uint16_t size)  {
    if(base_type::handle_message(sender, msg, arg, size) == 0 ) return AUI_OK;;

    if (msg == MSG_UART_WRITE) { 
        return on_write(sender, static_cast<aui_event_ex<aui_uart_payload>* >(arg) );
    }
    if(msg == MSG_UART_READ) {
        return on_read_text(sender, static_cast<aui_event_ex<aui_uart_payload>* >(arg) );
    }

    return 1;
}