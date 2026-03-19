// SPDX-License-Identifier: EUPL-1.2
//
// This file is part of AUI.
// AUI is distributed under the terms of the European Union Public License v1.2.
// You may obtain a copy of the license at:
// https://joinup.ec.europa.eu/collection/eupl/eupl-text-eupl-12

#pragma once 

#include "aui_messages.h"
#include "aui_element.h"

using aui_tick_t = uint64_t;

namespace detail {
    aui_tick_t aui_global_get_ticks();
}

#define AUI_TICKS_NOW       (detail::aui_global_get_ticks())
#define AUI_BASE_MS         1000ULL
#define AUI_MS_PER_TICK(hz)  (AUI_BASE_MS / (hz))


/**
 * @class aui_system
 * @brief Central message dispatcher and lifecycle controller for the AUI framework.
 *
 * The AUI system is responsible for:
 *  - Broadcasting core lifecycle messages (MSG_ONSETUP, MSG_ONLOOP)
 *  - Forwarding messages to a user‑defined handler
 *  - Acting as the root element in the message hierarchy
 *
 * It does NOT store elements itself. Instead, boards or other composite
 * elements register and manage their own children. The system simply
 * orchestrates message flow.
 */
class aui_system : public IElement {
public:
    /**
     * @brief Function pointer type for the global message handler.
     *
     * The handler receives:
     *  - sender: The element that originated the message
     *  - msg:    Message ID
     *  - arg:    Optional payload
     *  - size:   Payload size
     *
     * The handler should return 0 if the message was processed.
     */
    using OnMessageHandle = uint8_t(*)(const IElement* , const uint8_t , void* , const uint16_t);


    /**
     * @brief Constructs the AUI system object.
     *
     * Initializes internal timing state. No setup is performed until
     * on_setup() is called.
     */
    aui_system() : IElement(),  m_lastUpdate(0), m_msPerTick(16) { }

     /**
     * @brief Executes one system loop iteration.
     *
     * This method should be called from the Arduino loop() function.
     * It sends MSG_ONLOOP to the global message handler.
     *
     * @return true if the loop executed successfully.
     */
    bool on_loop();

    /**
     * @brief Performs system initialization.
     *
     * Sends MSG_ONSETUP to the global message handler.
     * Should be called once from Arduino setup().
     *
     * @return Always true.
     */
    bool on_setup() { send_massage(this, MSG_ONSETUP, NULL, 0); return true; }

    /**
     * @brief Sends a message into the AUI message bus.
     *
     * This is the central dispatch function of the framework.
     * All messages pass through this method before reaching the
     * user‑defined message handler.
     *
     * @param sender Pointer to the element that originated the message.
     * @param msg    Message ID.
     * @param arg    Optional payload pointer.
     * @param size   Size of the payload.
     * @return true if the message was forwarded.
     */
    bool send_massage(IElement* sender, uint8_t msg, void* arg, uint16_t size);

    template <class TEVENT>
    bool send_massage(IElement* sender, uint8_t msg, TEVENT event) {
        return send_massage(sender, msg, &event, sizeof(TEVENT));
    }
    /**
     * @brief Registers the global message handler.
     *
     * The handler is responsible for routing messages to boards,
     * elements, or other subsystems.
     *
     * @param handler Function pointer to the message handler.
     */
    void set_handler(OnMessageHandle handler) { m_msgHandler = handler; }

    inline void set_hz(uint8_t hz) {  m_msPerTick = AUI_MS_PER_TICK(hz);  }

private:
    uint32_t m_lastUpdate;        ///< Timestamp of last loop execution.
    uint16_t m_msPerTick;
    OnMessageHandle m_msgHandler; ///< User‑defined message routing function.
};
/**
 * @brief Global AUI system instance.
 *
 * This object should be used throughout the application to send messages
 * and control the AUI lifecycle.
 */
extern aui_system auisystem;

