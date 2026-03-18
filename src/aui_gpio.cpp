#include <avr/io.h>
#include <avr/interrupt.h>
#include "aui/aui_system.h"
#include <Arduino.h>

namespace detail {

#if AUI_CONFIG_EXT_INTERRUPT == 1
    extern "C" AUI_ISR_ATTR void aui_isr_handler_pin() {
        auisystem.send_massage<aui_idble_event>(&auisystem, MSG_INTERRUPT, aui_idble_event::make(TPIN));
    }
#endif

}