#pragma once

#include "aui_button.h"
#include "aui_avr_uart.h"

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



/* 

// ============================================================
// ATmega328P / 168 (UNO, Nano, Pro Mini) - Nur UART0
// ============================================================
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)


// using aui_uart0_hal = aui_uart_hal<
//     UDR0, UCSR0A, UCSR0B, UCSR0C, UBRR0,
//     TXEN0, RXEN0, UDRE0, RXC0, UCSZ00, UCSZ01,
//     UMSEL00, UMSEL01, UPM00, UPM01, USBS0, UCPOL0, U2X0,
//     RXCIE0, TXCIE0
// >;

#endif


// ============================================================
// ATmega2560 / 1280 (MEGA) - UART0, UART1, UART2, UART3
// ============================================================
#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__)

using aui_uart0_hal = aui_uart_hal<
    UDR0, UCSR0A, UCSR0B, UCSR0C, UBRR0,
    TXEN0, RXEN0, UDRE0, RXC0, UCSZ00, UCSZ01,
    UMSEL00, UMSEL01, UPM00, UPM01, USBS0, UCPOL0, U2X0,
    RXCIE0, TXCIE0
>;

using aui_uart1_hal = aui_uart_hal<
    UDR1, UCSR1A, UCSR1B, UCSR1C, UBRR1,
    TXEN1, RXEN1, UDRE1, RXC1, UCSZ10, UCSZ11,
    UMSEL10, UMSEL11, UPM10, UPM11, USBS1, UCPOL1, U2X1,
    RXCIE1, TXCIE1
>;

using aui_uart2_hal = aui_uart_hal<
    UDR2, UCSR2A, UCSR2B, UCSR2C, UBRR2,
    TXEN2, RXEN2, UDRE2, RXC2, UCSZ20, UCSZ21,
    UMSEL20, UMSEL21, UPM20, UPM21, USBS2, UCPOL2, U2X2,
    RXCIE2, TXCIE2
>;

using aui_uart3_hal = aui_uart_hal<
    UDR3, UCSR3A, UCSR3B, UCSR3C, UBRR3,
    TXEN3, RXEN3, UDRE3, RXC3, UCSZ30, UCSZ31,
    UMSEL30, UMSEL31, UPM30, UPM31, USBS3, UCPOL3, U2X3,
    RXCIE3, TXCIE3
>;

template< uint8_t TID, uint32_t TBAUD, uint8_t TDATA_BITS, aui_uart_parity TPARITY, uint8_t TSTOP_BITS, aui_uart_mode TMODE, uint8_t TCLOCK_POL, aui_bool_t TDOUBLE_SPEED>
using aui_uart0 = aui_uart<TID, aui_uart0_hal, TBAUD, TDATA_BITS, TPARITY, TSTOP_BITS, TMODE, TCLOCK_POL, TDOUBLE_SPEED>;

template< uint8_t TID, uint32_t TBAUD, uint8_t TDATA_BITS, aui_uart_parity TPARITY, uint8_t TSTOP_BITS, aui_uart_mode TMODE, uint8_t TCLOCK_POL, aui_bool_t TDOUBLE_SPEED>
using aui_uart1 = aui_uart<TID, aui_uart1_hal, TBAUD, TDATA_BITS, TPARITY, TSTOP_BITS, TMODE, TCLOCK_POL, TDOUBLE_SPEED>;

template< uint8_t TID, uint32_t TBAUD, uint8_t TDATA_BITS, aui_uart_parity TPARITY, uint8_t TSTOP_BITS, aui_uart_mode TMODE, uint8_t TCLOCK_POL, aui_bool_t TDOUBLE_SPEED>
using aui_uart2 = aui_uart<TID, aui_uart2_hal, TBAUD, TDATA_BITS, TPARITY, TSTOP_BITS, TMODE, TCLOCK_POL, TDOUBLE_SPEED>;

template< uint8_t TID, uint32_t TBAUD, uint8_t TDATA_BITS, aui_uart_parity TPARITY, uint8_t TSTOP_BITS, aui_uart_mode TMODE, uint8_t TCLOCK_POL, aui_bool_t TDOUBLE_SPEED>
using aui_uart3 = aui_uart<TID, aui_uart3_hal, TBAUD, TDATA_BITS, TPARITY, TSTOP_BITS, TMODE, TCLOCK_POL, TDOUBLE_SPEED>;


#endif


// ============================================================
// ATmega32U4 (Leonardo, Micro) - UART1 (kein UART0)
// ============================================================
#if defined(__AVR_ATmega32U4__)

using aui_uart0_hal = aui_uart_hal<
    UDR1, UCSR1A, UCSR1B, UCSR1C, UBRR1,
    TXEN1, RXEN1, UDRE1, RXC1, UCSZ10, UCSZ11,
    UMSEL10, UMSEL11, UPM10, UPM11, USBS1, UCPOL1, U2X1,
    RXCIE1, TXCIE1
>;

template< uint8_t TID, uint32_t TBAUD, uint8_t TDATA_BITS, aui_uart_parity TPARITY, uint8_t TSTOP_BITS, aui_uart_mode TMODE, uint8_t TCLOCK_POL, aui_bool_t TDOUBLE_SPEED>
using aui_uart0 = aui_uart<TID, aui_uart0_hal, TBAUD, TDATA_BITS, TPARITY, TSTOP_BITS, TMODE, TCLOCK_POL, TDOUBLE_SPEED>;


#endif


// ============================================================
// ATmega4809 (UNO WiFi Rev2) - UART0
// ============================================================
#if defined(__AVR_ATmega4809__)

// Achtung: ATmega4809 hat moderne Register-Struktur!
// Die Register heißen anders: USART0.STATUS, USART0.CTRLB, etc.
// Hier müsste eine spezielle HAL für moderne AVRs gebaut werden

using aui_uart0_hal = aui_uart_hal<
    USART0.TXDATAL, USART0.STATUS, USART0.CTRLB, USART0.CTRLC, USART0.BAUD,
    USART_TXEN_bp, USART_RXEN_bp, USART_DREIF_bp, USART_RXCIF_bp, 
    USART_CHSIZE0_bp, USART_CHSIZE1_bp,
    0, 0, 0, 0, USART_CHSIZE_8BIT_gc, 0, 0,  // UMSEL, UPM, USBS, UCPOL, U2X
    USART_RXCIE_bm, USART_TXCIE_bm
>;

template< uint8_t TID, uint32_t TBAUD, uint8_t TDATA_BITS, aui_uart_parity TPARITY, uint8_t TSTOP_BITS, aui_uart_mode TMODE, uint8_t TCLOCK_POL, aui_bool_t TDOUBLE_SPEED>
using aui_uart0 = aui_uart<TID, aui_uart0_hal, TBAUD, TDATA_BITS, TPARITY, TSTOP_BITS, TMODE, TCLOCK_POL, TDOUBLE_SPEED>;


#endif


// ============================================================
// ATmega644 / 1284 (MightyCore) - UART0
// ============================================================
#if defined(__AVR_ATmega644__) || defined(__AVR_ATmega1284__)

using aui_uart0_hal = aui_uart_hal<
    UDR0, UCSR0A, UCSR0B, UCSR0C, UBRR0,
    TXEN0, RXEN0, UDRE0, RXC0, UCSZ00, UCSZ01,
    UMSEL00, UMSEL01, UPM00, UPM01, USBS0, UCPOL0, U2X0,
    RXCIE0, TXCIE0
>;

template< uint8_t TID, uint32_t TBAUD, uint8_t TDATA_BITS, aui_uart_parity TPARITY, uint8_t TSTOP_BITS, aui_uart_mode TMODE, uint8_t TCLOCK_POL, aui_bool_t TDOUBLE_SPEED>
using aui_uart0 = aui_uart<TID, aui_uart0_hal(), TBAUD, TDATA_BITS, TPARITY, TSTOP_BITS, TMODE, TCLOCK_POL, TDOUBLE_SPEED>;


#endif */