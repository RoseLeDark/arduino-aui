#pragma once

#include "aui_button.h"

template <uint8_t TPIN,  uint32_t TDOUBLE_CLICK_TIME = 250>
using aui_limit = aui_button<TPIN, TDOUBLE_CLICK_TIME>;

template <uint8_t TPIN,  uint32_t TDOUBLE_CLICK_TIME = 250>
using aui_reed = aui_button<TPIN, TDOUBLE_CLICK_TIME>;

template <uint8_t TPIN,  uint32_t TDOUBLE_CLICK_TIME = 250>
using aui_switch = aui_button<TPIN, TDOUBLE_CLICK_TIME>;

template <uint8_t TPIN,  uint32_t TDOUBLE_CLICK_TIME = 250>
using aui_endstop = aui_button<TPIN, TDOUBLE_CLICK_TIME>;

template <uint8_t TPIN,  uint32_t TDOUBLE_CLICK_TIME = 250>
using aui_limit_pullup = aui_button_pullup<TPIN, TDOUBLE_CLICK_TIME>;

template <uint8_t TPIN,  uint32_t TDOUBLE_CLICK_TIME = 250>
using aui_reed_pullup = aui_button_pullup<TPIN, TDOUBLE_CLICK_TIME>;

template <uint8_t TPIN,  uint32_t TDOUBLE_CLICK_TIME = 250>
using aui_switch_pullup = aui_button_pullup<TPIN, TDOUBLE_CLICK_TIME>;

template <uint8_t TPIN,  uint32_t TDOUBLE_CLICK_TIME = 250>
using aui_endstop_pullup = aui_button_pullup<TPIN, TDOUBLE_CLICK_TIME>;


template<uint8_t TPIN>
using aui_motion = aui_digital_input<TPIN>;      // PIR

template<uint8_t TPIN>
using aui_flame = aui_digital_input<TPIN>;

template<uint8_t TPIN>
using aui_rain = aui_digital_input<TPIN>;

template<uint8_t TPIN>
using aui_hall = aui_digital_input<TPIN>;

template<uint8_t TPIN>
using aui_lightgate = aui_digital_input<TPIN>;

template<uint8_t TPIN>
using aui_vibration = aui_digital_input<TPIN>;

template<uint8_t TPIN>
using aui_knock = aui_digital_input<TPIN>;

template<uint8_t TPIN>
using aui_gas = aui_digital_input<TPIN>;


template<uint8_t TPIN>
using aui_relay = aui_digital_output<TPIN>;

template<uint8_t TPIN>
using aui_ssr = aui_digital_output<TPIN>;

template<uint8_t TPIN>
using aui_mosfet = aui_digital_output<TPIN>;

template<uint8_t TPIN>
using aui_transistor = aui_digital_output<TPIN>;

template<uint8_t TPIN>
using aui_valve = aui_digital_output<TPIN>;

template<uint8_t TPIN>
using aui_pump = aui_digital_output<TPIN>;

template<uint8_t TPIN>
using aui_laser = aui_digital_output<TPIN>;

template<uint8_t TPIN>
using aui_trigger = aui_digital_output<TPIN>;

