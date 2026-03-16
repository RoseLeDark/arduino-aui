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
template <HardwareSerial* TSerial, uint32_t TBAUD = 9600, uint32_t TConfig = SERIAL_8N1>
class aui_serial : public IElement {
public:
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
    uint8_t handle_message(IElement* sender, uint8_t msg, void* arg, uint16_t size) override;

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
     * @brief Callback for formatted UART text output.
     *
     * This method is invoked when the element receives:
     *  - MSG_UART_INFO_TEXT
     *  - MSG_UART_ERROR_TEXT
     *  - MSG_UART_DEBUG_TEXT
     *  - MSG_UART_VERBOSE_TEXT
     *
     * The default implementation prints a timestamped line to the UART.
     *
     * @param what  Category string ("info", "error", "debug", "verbose").
     * @param stext The text message to output.
     */
    virtual void on_text(String what, String stext);
};

// ---------------------------------------------------------------------------
// Implementation
// ---------------------------------------------------------------------------

template <HardwareSerial* TSerial, uint32_t TBAUD , uint32_t TConfig >
void aui_serial<TSerial, TBAUD, TConfig>::on_serial_event() {
        while (TSerial->available()) {
        char c = TSerial->read();
        auisystem.send_massage(nullptr, MSG_UART_RAW_RX, &c, 1);
    }
}

template <HardwareSerial* TSerial, uint32_t TBAUD , uint32_t TConfig >
void aui_serial<TSerial, TBAUD, TConfig>::on_text(String what, String stext) {
    String _text = "[ " + String(millis()) + " ] " + what + ": " + stext;
    TSerial->println(_text);
}

template <HardwareSerial* TSerial, uint32_t TBAUD , uint32_t TConfig >
uint8_t aui_serial<TSerial, TBAUD, TConfig>::handle_message(IElement* sender, uint8_t msg, void* arg, uint16_t size)  {

        if (msg == MSG_ONSETUP) {
            TSerial->begin(TBAUD, TConfig);
            return 0;
        }

        if (msg == MSG_UART_INFO_TEXT && AUI_LOGLEVEL <= AUI_LOG_LEVEL_INFOS) {
            const char* text = (const char*)arg;
            on_text("info", text);
            return 0;
        }

        if (msg == MSG_UART_ERROR_TEXT && AUI_LOGLEVEL <= AUI_LOG_LEVEL_ERRORS) {
            const char* text = (const char*)arg;
            on_text("error", text);
            return 0;
        }

         if (msg == MSG_UART_DEBUG_TEXT && AUI_LOGLEVEL <= AUI_LOG_LEVEL_DEBUG ) {
                const char* text = (const char*)arg;
                on_text("Debug", text);
            return 0;
        }

        if (msg == MSG_UART_VERBOSE_TEXT && AUI_LOGLEVEL <= AUI_LOG_LEVEL_VERBOSE) { 
            const char* text = (const char*)arg;
            on_text("Verbose", text);
            
            return 0;
        }

        return 1;
    }