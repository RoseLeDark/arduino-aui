
// SPDX-License-Identifier: EUPL-1.2
//
// This file is part of AUI.
// AUI is distributed under the terms of the European Union Public License v1.2.
// You may obtain a copy of the license at:
// https://joinup.ec.europa.eu/collection/eupl/eupl-text-eupl-12

#include <Arduino.h>
#include "AUI/aui_system.h"

aui_system auisystem;


bool aui_system::on_loop() {
    unsigned long now = millis();
    if (now - m_lastUpdate < 16)
        return true;

    m_lastUpdate = now;

    if(m_msgHandler != 0) {
        return (m_msgHandler( (IElement*)this, MSG_ONLOOP, NULL, 0 ) == 0);
    }
    return false;
}

bool aui_system::send_massage(IElement* sender, uint8_t msg, void* arg, uint16_t size) {
    if(m_msgHandler != 0) {
        return (m_msgHandler( sender, msg, arg, size ) == 0);
    }
    return false;
}