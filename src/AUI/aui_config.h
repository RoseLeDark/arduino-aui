#pragma once

#include <Arduino.h>


#define AUI_CONFIG_ON                       1
#define AUI_CONFIG_OFF                      0
#define AUI_CONFIG_USED                     1

#ifndef AUI_SYSTEM_MAX_UI_ELEMENTS
#define AUI_SYSTEM_MAX_UI_ELEMENTS          8
#endif

#define AUI_CONFIG_TIMER_1000HZ     AUI_CONFIG_OFF
#define AUI_CONFIG_TIMER_500HZ      AUI_CONFIG_OFF
#define AUI_CONFIG_TIMER_100HZ      AUI_CONFIG_USED
#define AUI_CONFIG_TIMER_50HZ       AUI_CONFIG_OFF
#define AUI_CONFIG_TIMER_20HZ       AUI_CONFIG_OFF

#ifndef AUI_CONFIG_HIDE_LOOP
#define AUI_CONFIG_HIDE_LOOP        AUI_CONFIG_OFF
#endif

/// ------------------------------- DO NO EDIT AFTER THIS LINE -------------------------------
#if defined(__AVR__)
    #define AUI_PLATFORM_AVR AUI_CONFIG_USED
    #define AUI_ISR_ATTR
#elif defined(ARDUINO_ARCH_SAMD)
    #define AUI_PLATFORM_SAMD AUI_CONFIG_USED
    #define AUI_ISR_ATTR
#elif defined(ARDUINO_ARCH_STM32)
    #define AUI_PLATFORM_STM32 AUI_CONFIG_USED
    #define AUI_ISR_ATTR __attribute__((used))
#elif defined(ESP32)
    #define AUI_PLATFORM_ESP32 AUI_CONFIG_USED
    #define AUI_ISR_ATTR ARDUINO_ISR_ATTR
#else
    #define AUI_ISR_ATTR
#endif

#if AUI_PLATFORM_SAMD == 1 | AUI_PLATFORM_STM32 == 1 | AUI_PLATFORM_ESP32 == 1
#define AUI_CONFIG_EXT_INTERRUPT        AUI_CONFIG_ON
#define AUI_CONFIG_ANALOG_INTERRUPT     AUI_CONFIG_ON
#else 
#define AUI_CONFIG_EXT_INTERRUPT        AUI_CONFIG_OFF
#define AUI_CONFIG_ANALOG_INTERRUPT     AUI_CONFIG_OFF
#endif


#if AUI_CONFIG_TIMER_1000HZ == AUI_CONFIG_USED
    #define AUI_CONFIG_TIMER_HZ aui_timer_hz::Hz1000
#elif AUI_CONFIG_TIMER_500HZ == AUI_CONFIG_USED
    #define AUI_CONFIG_TIMER_HZ aui_timer_hz::Hz500
#elif AUI_CONFIG_TIMER_100HZ == AUI_CONFIG_USED
    #define AUI_CONFIG_TIMER_HZ aui_timer_hz::Hz100
#elif AUI_CONFIG_TIMER_50HZ == AUI_CONFIG_USED
    #define AUI_CONFIG_TIMER_HZ aui_timer_hz::Hz50
#elif AUI_CONFIG_TIMER_20HZ == AUI_CONFIG_USED
    #define AUI_CONFIG_TIMER_HZ aui_timer_hz::Hz20
#else
    #define AUI_CONFIG_NOTIMER_T AUI_CONFIG_ON
#endif

#define AUI_WEAK __attribute__((weak))

#define AUI_VERSION_10          1
#define AUI_VERSION_15          0