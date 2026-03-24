#include <avr/io.h>
#include <avr/interrupt.h>

#include <Arduino.h>
#include "aui/aui_config.h"
#include "aui/aui_system.h"

namespace detail {

// --- Konfiguration / Globals ---
static volatile uint8_t __last_state[3] = {0,0,0}; // last state per PCINT group
static volatile uint8_t __mask_group[3] = {0,0,0}; // mask per group (PCMSKx)
static uint8_t __id_map[24] = { 0 };
static uint8_t __group_inited[3] = {0,0,0}; // per-group init flag

// --- Implementation using Arduino core macros when available ---
#if AUI_PLATFORM_AVR == AUI_CONFIG_USED
  #include <Arduino.h>
  static int8_t tpin_to_pcint_index(uint8_t tpin) {
      // digitalPinToPCICRbit returns the PCIE bit index (0..2) or NOT_A_PIN
      // digitalPinToPCMSKbit returns the bit inside PCMSKx
      // digitalPinToPCMSK returns pointer to PCMSKx register (we don't need pointer here)
      int8_t pcicrbit = digitalPinToPCICRbit(tpin);
      if (pcicrbit == NOT_AN_INTERRUPT) return -1;
      // compute global pcint index: group base + bit
      uint8_t bit = digitalPinToPCMSKbit(tpin);
      if (pcicrbit == 0) return (int8_t)bit;           // PCINT0..7
      if (pcicrbit == 1) return (int8_t)(8 + bit);     // PCINT8..15
      return (int8_t)(16 + bit);                       // PCINT16..23
  }
#else
// --- Fallback: explicit mapping for ATmega328P (Uno/Nano) ---
// D0..D7 -> PCINT16..23, D8..D13 -> PCINT0..5, A0..A5 -> PCINT8..13
static const int8_t tpin_to_pcint_table[] = {
  16,17,18,19,20,21,22,23,  0,1,2,3,4,5,  8,9,10,11,12,13
};
static int8_t tpin_to_pcint(uint8_t tpin) {
  if (tpin < (sizeof(tpin_to_pcint_table)/sizeof(tpin_to_pcint_table[0])))
      return tpin_to_pcint_table[tpin];
  return -1;
}
#endif


static inline uint8_t pcint_to_group(uint8_t pcint) {
    if (pcint <= 7) return 0;       // PCINT0..7  -> group 0 (PORTB)
    if (pcint <= 14) return 1;      // PCINT8..14 -> group 1 (PORTC)
    return 2;                       // PCINT16..23 -> group 2 (PORTD)
}

static inline uint8_t pcint_to_bitmask(uint8_t pcint) {
    if (pcint <= 7)       return (1 << pcint);         // PORTB bit
    else if (pcint <= 14) return (1 << (pcint - 8));   // PORTC bit
    else                  return (1 << (pcint - 16));  // PORTD bit
}

static inline uint8_t read_port_for_group(uint8_t group) {
    if (group == 0) return PINB;
    if (group == 1) return PINC;
    return PIND;
}

static inline void set_pcmsk_for_group(uint8_t group, uint8_t mask) {
    if (group == 0) PCMSK0 = mask;
    else if (group == 1) PCMSK1 = mask;
    else PCMSK2 = mask;
}

static inline void enable_pcint_group_irq(uint8_t group) {
    if (group == 0) PCICR |= (1 << PCIE0);
    else if (group == 1) PCICR |= (1 << PCIE1);
    else PCICR |= (1 << PCIE2);
}

// --- Interrupt handler für eine Gruppe ---
static void aui_gpio_pcint_handle_group(uint8_t group) {
    uint8_t current = read_port_for_group(group);
    uint8_t changed = (current ^ __last_state[group]) & __mask_group[group];
    __last_state[group] = current;

    if (!changed) return;

    // Iteriere über Bits der Gruppe (max 8)
    for (uint8_t bit = 0; bit < 8; ++bit) {
        if (changed & (1 << bit)) {
            // Bestimme globalen PCINT index
            uint8_t pcint_index;
            if (group == 0) pcint_index = bit;            // PCINT0..7
            else if (group == 1) pcint_index = 8 + bit;   // PCINT8..15 (we use up to 14)
            else pcint_index = 16 + bit;                  // PCINT16..23

            if (pcint_index >= 24) continue; // safety

            uint8_t id = __id_map[pcint_index];
            if (id == 0xFF) continue; // nicht registriert

            // Event an AUI senden; auisystem ist angenommen vorhanden
            auisystem.send_massage<aui_event_ex<aui_pcint_payload>>(&auisystem, MSG_PCINT_INTERRUPT, 
               aui_event_ex<aui_pcint_payload>(id, (current >> bit) & 1 ));




            
        }
    }
}


// --- Public Funktion: registriert TPIN und ID, initialisiert Gruppe falls nötig ---
void aui_gpio_pcint_register_pin(uint8_t tpin, uint8_t pcint_id) {
    int8_t pcint = tpin_to_pcint_index(tpin);
    if (pcint < 0) return; // TPIN hat keinen PCINT

    uint8_t group = pcint_to_group((uint8_t)pcint);
    uint8_t bitmask = pcint_to_bitmask((uint8_t)pcint);

    // Gruppe initialisieren beim ersten Registrieren
    if (!__group_inited[group]) {
        __last_state[group] = read_port_for_group(group);
        __mask_group[group] = 0;
        set_pcmsk_for_group(group, 0);
        enable_pcint_group_irq(group);
        __group_inited[group] = 1;
    }

    // ID speichern und Maske setzen
    __id_map[pcint] = pcint_id;
    __mask_group[group] |= bitmask;
    // PCMSKx aktualisieren
    if (group == 0) PCMSK0 = __mask_group[group];
    else if (group == 1) PCMSK1 = __mask_group[group];
    else PCMSK2 = __mask_group[group];
}
}

// --- ISR Handlers für alle drei Gruppen ---
ISR(PCINT0_vect) { detail::aui_gpio_pcint_handle_group(0); }
ISR(PCINT1_vect) { detail::aui_gpio_pcint_handle_group(1); }
ISR(PCINT2_vect) { detail::aui_gpio_pcint_handle_group(2); }
