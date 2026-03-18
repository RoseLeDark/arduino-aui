// SPDX-License-Identifier: EUPL-1.2
//
// This file is part of AUI.
// AUI is distributed under the terms of the European Union Public License v1.2.
// You may obtain a copy of the license at:
// https://joinup.ec.europa.eu/collection/eupl/eupl-text-eupl-12

#pragma once 

#include "aui_element.h"

/**
 * @class aui_board
 * @brief A composite container element that groups multiple AUI elements.
 *
 * The board acts as a lightweight parent widget. It does not implement
 * rendering or layout logic by itself. Instead, it forwards all incoming
 * messages to its child elements.
 *
 * Typical usage:
 *  - Derive a custom board class
 *  - Override on_create() to register child elements
 *  - Add elements using add_element()
 *  - The board automatically forwards all messages to its children
 *
 * @tparam TMAX_ELEMENT Maximum number of child elements this board can hold.
 */
template <uint8_t TMAX_ELEMENT>
class aui_board : public IElement {
public:
     /**
     * @brief Constructs an empty board with zero registered elements.
     */
    aui_board ();

    /**
     * @brief Adds an element to the board.
     *
     * The element is stored in the internal static array. No dynamic memory
     * allocation is used. If the board is full, the element is not added.
     *
     * @param e Pointer to the element to add.
     * @return The new element count (1..TMAX_ELEMENT), or 0 if the board is full.
     */
    uint8_t add_element(IElement* e);
    /**
     * @brief Handles incoming AUI messages and forwards them to all children.
     *
     * Special behavior:
     *  - On MSG_ONSETUP, the board calls on_create() before forwarding.
     *
     * All other messages are forwarded unchanged to each registered element.
     *
     * @param sender Pointer to the element that sent the message.
     * @param msg    Message ID.
     * @param arg    Optional message payload.
     * @param size   Size of the payload.
     * @return Always returns 0.
     */
    uint8_t handle_message(const IElement* sender, const uint8_t msg, void* arg, const uint16_t size);
protected:
    
    IElement* m_elements[TMAX_ELEMENT]; ///< Static array of child element pointers.
    uint8_t   m_count;                 ///< Number of registered elements.
};

// ---------------------------------------------------------------------------
// Implementation
// ---------------------------------------------------------------------------

template <uint8_t TMAX_ELEMENT>
aui_board<TMAX_ELEMENT>::aui_board () {
    m_count = 0;
}

template <uint8_t TMAX_ELEMENT>
uint8_t aui_board<TMAX_ELEMENT>::add_element(IElement* e) {
    uint8_t _ret = 0;

    if (m_count < MAX_ELEMENTS) {
        m_elements[m_count++] = e;
        _ret = m_count;
    }
    return _ret;
}

template <uint8_t TMAX_ELEMENT>
uint8_t aui_board<TMAX_ELEMENT>::handle_message(const IElement* sender, const uint8_t msg, void* arg, const uint16_t size) {
    for (uint8_t i = 0; i < m_count; i++)
        m_elements[i]->handle_message(sender, msg, arg, size);

    return 0;
}
