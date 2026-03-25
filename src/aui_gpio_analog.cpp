#include <Arduino.h>

#include "AUI/aui_arch.h"
#include "AUI/aui_gpio.h"
#include "AUI/aui_avr_uart.h"

#ifdef HAS_UART_0
aui_uart_hal AUI_UART0_HAL = {
    {
        &UDR0,
        &UCSR0A,
        &UCSR0B,
        &UCSR0C,
        &UBRR0,
        TXEN0,
        RXEN0,
        UDRE0,
        RXC0,
        UCSZ00,
        UCSZ01,
        UMSEL00,
        UMSEL01,
        UPM00,
        UPM01,
        USBS0,
        UCPOL0,
        U2X0,
        RXCIE0,
        TXCIE0
    }
};

#endif

#ifdef HAS_UART_1
aui_uart_hal AUI_UART1_HAL;
#endif

#ifdef HAS_UART_2
aui_uart_hal AUI_UART2_HAL;
#endif

#ifdef HAS_UART_3
aui_uart_hal AUI_UART3_HAL;
#endif

namespace detail {
    uint8_t aui_gpio_analog_disable(uint8_t pin, uint8_t mode) {
        // ============================================================
        // ATmega328P / 168  (UNO, Nano, Pro Mini)
        // ============================================================
    #if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)

        switch(pin) {
            case A0: if (mode != 0) DIDR0 |= (1<<ADC0D); else DIDR0 &= ~(1<<ADC0D); return 0;
            case A1: if (mode != 0) DIDR0 |= (1<<ADC1D); else DIDR0 &= ~(1<<ADC1D); return 0;
            case A2: if (mode != 0) DIDR0 |= (1<<ADC2D); else DIDR0 &= ~(1<<ADC2D); return 0;
            case A3: if (mode != 0) DIDR0 |= (1<<ADC3D); else DIDR0 &= ~(1<<ADC3D); return 0;
            case A4: if (mode != 0) DIDR0 |= (1<<ADC4D); else DIDR0 &= ~(1<<ADC4D); return 0;
            case A5: if (mode != 0) DIDR0 |= (1<<ADC5D); else DIDR0 &= ~(1<<ADC5D); return 0;

            // Komparator
            case 6:  if (mode != 0) DIDR1 |= (1<<AIN0D); else DIDR1 &= ~(1<<AIN0D); return 0;
            case 7:  if (mode != 0) DIDR1 |= (1<<AIN1D); else DIDR1 &= ~(1<<AIN1D); return 0;
        }

    #endif


        // ============================================================
        // ATmega2560 / 1280 (MEGA)
        // ============================================================
    #if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__)

        if (pin >= A0 && pin <= A15) {
            uint8_t bit = pin - A0;
            if (mode != 0) DIDR0 |=  (1 << bit);
            else    DIDR0 &= ~(1 << bit);
            return 0;
        }

    #endif


        // ============================================================
        // ATmega32U4 (Leonardo, Micro)
        // ============================================================
    #if defined(__AVR_ATmega32U4__)

        if (pin >= A0 && pin <= A12) {
            uint8_t bit = pin - A0;
            if (mode != 0) DIDR0 |=  (1 << bit);
            else    DIDR0 &= ~(1 << bit);
            return 0;
        }

    #endif


        // ============================================================
        // ATtiny85 / 45
        // ============================================================
    #if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny45__)

        if (pin >= A0 && pin <= A3) {
            uint8_t bit = pin - A0;
            if (mode != 0) DIDR0 |=  (1 << bit);
            else    DIDR0 &= ~(1 << bit);
            return 0;
        }

    #endif


        // ============================================================
        // ATtiny84 / 44
        // ============================================================
    #if defined(__AVR_ATtiny84__) || defined(__AVR_ATtiny44__)

        if (pin >= A0 && pin <= A7) {
            uint8_t bit = pin - A0;
            if (mode != 0) DIDR0 |=  (1 << bit);
            else    DIDR0 &= ~(1 << bit);
            return 0;
        }

    #endif


        // ============================================================
        // ATtiny861 / 461 / 261
        // ============================================================
    #if defined(__AVR_ATtiny861__) || defined(__AVR_ATtiny461__) || defined(__AVR_ATtiny261__)

        if (pin >= A0 && pin <= A7) {
            uint8_t bit = pin - A0;
            if (mode != 0) DIDR0 |=  (1 << bit);
            else    DIDR0 &= ~(1 << bit);
            return 0;
        }

    #endif


        // ============================================================
        // ATmega4809 (UNO WiFi Rev2)
        // ============================================================
    #if defined(__AVR_ATmega4809__)

        if (pin >= A0 && pin <= A15) {
            uint8_t bit = pin - A0;
            if (mode != 0) ADC0.DIDR0 |=  (1 << bit);
            else    ADC0.DIDR0 &= ~(1 << bit);
            return 0;
        }

    #endif


        // ============================================================
        // ATmega644 / 1284 (MightyCore)
        // ============================================================
    #if defined(__AVR_ATmega644__) || defined(__AVR_ATmega1284__)

        if (pin >= A0 && pin <= A7) {
            uint8_t bit = pin - A0;
            if (mode != 0) DIDR0 |=  (1 << bit);
            else    DIDR0 &= ~(1 << bit);
            return 0;
        }

    #endif

        return 1;
    }
}