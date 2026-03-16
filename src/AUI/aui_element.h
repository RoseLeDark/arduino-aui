// SPDX-License-Identifier: EUPL-1.2
//
// This file is part of AUI.
// AUI is distributed under the terms of the European Union Public License v1.2.
// You may obtain a copy of the license at:
// https://joinup.ec.europa.eu/collection/eupl/eupl-text-eupl-12

#pragma once 

#include "Arduino.h"
#include <string.h>
#include "aui_messages.h"
/**
 * @class IElement
 * @brief Base interface for all AUI elements.
 *
 * This is the fundamental abstraction in the AUI framework. Every component,
 * whether UI‑related or hardware‑related, derives from IElement.
 *
 * Responsibilities:
 *  - Receives messages from the AUI system
 *  - Optionally reacts to setup and loop events
 *  - Provides a minimal lifecycle interface (begin/update)
 *
 * The default implementation does nothing. Derived classes override only what
 * they need, keeping the system lightweight and deterministic.
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
    virtual uint8_t handle_message(IElement* sender, uint8_t msg, void* arg, uint16_t size) { return 0; }

protected:

    /**
     * @brief Optional initialization hook.
     *
     * Called automatically when the element receives MSG_ONSETUP.
     * Derived classes override this to perform hardware or state initialization.
     */
    virtual void begin() { }

    /**
     * @brief Optional periodic update hook.
     *
     * Called automatically when the element receives MSG_ONLOOP.
     * Derived classes override this to implement polling or state updates.
     */
    virtual void update() { }
};


/**
 * @class IVisualElement
 * @brief Base class for elements that have a visual representation.
 *
 * Extends IElement by adding a paint callback. This is used by display‑based
 * widgets such as labels, buttons, sliders, or custom UI components.
 *
 * Rendering is not performed automatically; the AUI system or a display driver
 * must explicitly trigger paint events.
 */
class IVisualElement : public IElement {
protected:

    /**
     * @brief Optional paint callback.
     *
     * Called when the element should render itself. The exact timing and
     * triggering of paint events depends on the display backend.
     *
     * Derived classes override this to draw their visual representation.
     */
    virtual uint8_t on_paint() { return 0; }
};
