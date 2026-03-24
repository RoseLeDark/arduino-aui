// SPDX-License-Identifier: EUPL-1.2
//
// This file is part of AUI.
// AUI is distributed under the terms of the European Union Public License v1.2.
// You may obtain a copy of the license at:
// https://joinup.ec.europa.eu/collection/eupl/eupl-text-eupl-12

#pragma once 

#include <Arduino.h>
#include <string.h>
#include "aui_error.h"
#include "aui_types.h"
#include "aui_config.h"
#include "aui_messages.h"
#include "aui_event.h"

#define private_static private
/**
 * @class IElement
 * @brief Base class for all AUI components.
 *
 * IElement defines the core message‑driven execution model of the AUI framework.
 * Every component—whether hardware‑related, logical, or part of the UI—derives
 * from this class and participates in the unified event dispatch system.
 *
 * Responsibilities:
 *  - Receives all messages routed through the AUI system.
 *  - Provides lifecycle hooks for initialization (on_begin) and periodic updates (on_update).
 *  - Offers a minimal, deterministic interface that derived classes can extend.
 *
 * Message Handling:
 *  - handle_message() is the single entry point for all events.
 *  - MSG_ONSETUP triggers on_begin(), called exactly once during system startup.
 *  - MSG_ONLOOP triggers on_update(), called periodically by the AUI runtime.
 *
 * Derived classes override only the callbacks they need, keeping the system
 * lightweight, predictable, and easy to reason about.
 */

class IElement {
public:
    /**
     * @brief Handles an incoming AUI message.
     *
     * This is the central communication mechanism of the AUI framework.
     * All elements receive all messages unless filtered by the board or system.
     *
     * @param sender Pointer to the element that sent the message .
     * @param msg    Message ID.
     * @param arg    Optional message payload.
     * @param size   Size of the payload.
     * @return 0 if handled, non‑zero if ignored.
     */
    virtual uint8_t handle_message(const IElement* sender, const uint8_t msg, void* arg, const uint16_t size) {
        if (msg == MSG_ONSETUP) { 
            return on_begin(sender, static_cast<aui_event_ex<aui_idble_payload>*>(arg) );  
        }
        if (msg == MSG_ONLOOP && m_bEnable == 0) { 
            return on_update(sender, *static_cast<uint64_t*>(arg)  ); 
        }
        return 1;
    }

    inline uint8_t is_enable() const { return m_bEnable; }
    inline void set_enable(uint8_t value) { m_bEnable = value; }
protected:

    /**
     * @brief Optional initialization hook.
     *
     * Called automatically when the element receives MSG_ONSETUP.
     * Derived classes override this to perform hardware or state initialization.
     */
    virtual auier_t on_begin(const IElement* sender, const aui_event_ex<aui_idble_payload>* event) { return 0; }

    /**
     * @brief Optional periodic update hook.
     *
     * Called automatically when the element receives MSG_ONLOOP.
     * Derived classes override this to implement polling or state updates.
     */
    virtual auier_t on_update(const IElement* sender, const uint64_t ticks) { return 0; }
protected:
    uint8_t m_bEnable = 0 ; // 1 = false, 0 = true
};

template <uint8_t TID>
class IElementWithID : public IElement {
public:
    using base_type = IElement;

    virtual uint8_t handle_message(const IElement* sender, const uint8_t msg, void* arg, const uint16_t size) { 
        if(base_type::handle_message(sender, msg, arg, size) == 0) return 0;

        if (msg == MSG_DISABLE && base_type::m_bEnable == 0) {
            return on_disable(sender, *static_cast<uint8_t*>(arg));
        }
        if (msg == MSG_ENABLE) {
            return on_enable(sender, *static_cast<uint8_t*>(arg));
        }
        return 1;
    }

    uint8_t getID() {return TID; }
protected:
    virtual auier_t on_disable(const IElement* sender, const uint8_t ID)  {
        if(ID == TID) base_type::set_enable(1); 
        return 0; 
    }
    virtual auier_t on_enable(const IElement* sender, const uint8_t ID)  { 
        if(ID == TID) base_type::set_enable(0);
        return 0;  
    }
};

/**
 * @class IVisualElement
 * @brief Base class for elements with a visual representation.
 *
 * IVisualElement extends IElement by adding support for rendering events.
 * Components that draw to a display—such as labels, buttons, sliders, or
 * custom widgets—inherit from this class and implement on_paint().
 *
 * Rendering Model:
 *  - Visual updates are triggered explicitly via MSG_PAINT.
 *  - The AUI system or a display backend is responsible for issuing paint events.
 *  - on_paint() receives the same event structure as other callbacks, ensuring
 *    a consistent message‑driven design.
 *
 * This separation keeps rendering deterministic and avoids hidden refresh logic.
 * Visual elements remain passive until the system requests a repaint.
 */
template <uint16_t TID>
class IVisualElement : public IElementWithID<TID> {
public:
    using base_type = IElementWithID<TID>;

    virtual auier_t handle_message(const IElement* sender, const uint8_t msg, void* arg, const uint16_t size) { 
        if(base_type::handle_message(sender, msg, arg, size) == AUI_OK) return AUI_OK;

        if(msg == MSG_PAINT && base_type::m_bEnable == 0) {
            return on_paint(sender, static_cast<aui_event_ex<aui_idble_payload>*>(arg) );
        }
        return AUI_ERROR_NOTHANDLED;
    }
    
protected:
    /**
     * @brief Optional initialization hook.
     *
     * Called automatically when the element receives MSG_ONSETUP.
     * Derived classes override this to perform hardware or state initialization.
     */
    virtual auier_t on_paint(const IElement* sender, aui_event_ex<aui_idble_payload>* event) { return 0; }

    
};
