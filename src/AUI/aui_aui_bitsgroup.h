#pragma once

#include "aui_config.h"
#include "aui_types.h"



#if AUI_PLATFORM_AVR == AUI_CONFIG_USED
    #define AUI_BITS_GROUP_SIZE 8
    #define AUI_BITS_GROUP_TYPE uint8_t
#elif AUI_PLATFORM_STM32 == AUI_CONFIG_USED | AUI_PLATFORM_ESP32 == AUI_CONFIG_USED
    #define AUI_BITS_GROUP_SIZE 32
    #define AUI_BITS_GROUP_TYPE uint32_t
#endif
#define AUI_BIT(POS)            ( 1 << POS )

#define AUI_BIT_0	            AUI_BIT(0)
#define AUI_BIT_1           	AUI_BIT(1)
#define AUI_BIT_2           	AUI_BIT(2)
#define AUI_BIT_3           	AUI_BIT(3)
#define AUI_BIT_4           	AUI_BIT(4)
#define AUI_BIT_5           	AUI_BIT(5)
#define AUI_BIT_6           	AUI_BIT(6)
#define AUI_BIT_7           	AUI_BIT(7)
#if AUI_BITS_GROUP_SIZE  > 16
#define AUI_BIT_8	            AUI_BIT(8)
#define AUI_BIT_9           	AUI_BIT(9)
#define AUI_BIT_10           	AUI_BIT(10)
#define AUI_BIT_11           	AUI_BIT(11)
#define AUI_BIT_12           	AUI_BIT(12)
#define AUI_BIT_13           	AUI_BIT(13)
#define AUI_BIT_14           	AUI_BIT(14)
#define AUI_BIT_15           	AUI_BIT(15)
#endif
#if AUI_BITS_GROUP_SIZE  > 23
#define AUI_BIT_16	            AUI_BIT(16)
#define AUI_BIT_17           	AUI_BIT(17)
#define AUI_BIT_18           	AUI_BIT(18)
#define AUI_BIT_19           	AUI_BIT(19)
#define AUI_BIT_20           	AUI_BIT(20)
#define AUI_BIT_21           	AUI_BIT(24)
#define AUI_BIT_22           	AUI_BIT(22)
#define AUI_BIT_23           	AUI_BIT(23)
#endif
#if AUI_BITS_GROUP_SIZE  > 32
#define AUI_BIT_24	            AUI_BIT(24)
#define AUI_BIT_25           	AUI_BIT(25)
#define AUI_BIT_26           	AUI_BIT(26)
#define AUI_BIT_27           	AUI_BIT(27)
#define AUI_BIT_28           	AUI_BIT(28)
#define AUI_BIT_29           	AUI_BIT(29)
#define AUI_BIT_30           	AUI_BIT(30)
#define AUI_BIT_31           	AUI_BIT(31)
#endif

template <typename T, uint8_t TSIZE = value2size<T>::size, typename TBITS = aui_bit_type>
class aui_bitsgroup {
private:
    union {
        volatile TBITS event [TSIZE];
        volatile T m_raw;
    };
    volatile T m_lockt;
public:
    using bits_type = T;
    using this_type = aui_bitsgroup<T, TSIZE, TBITS>;

    void clear(uint8_t pos) {
        if (aui_in_isr() == AUI_TRUE || is_locked(pos) == AUI_FALSE) {
        if (event[pos] == 1) event[pos] = 0;
    }
    }
    void set(uint8_t pos) {
        if (aui_in_isr() == AUI_TRUE || is_locked(pos) == AUI_FALSE) {
        if (event[pos] == 0) event[pos] = 1;
    }
    void flip(uint8_t pos) {
        if (aui_in_isr() == AUI_FALSE && is_locked(pos) == AUI_FALSE) {
        event[pos].flip();
    }

    void lock_bits(bits_type mask) {
        m_lock |= mask;
    }

    void unlock_bits(bits_type mask) {
        m_lock &= ~mask;
    }
    aui_bool_t is_locked(uint8_t pos) const {
        return (m_lock & (bits_type(1) << pos)) ? AUI_FALSE : AUI_TRUE;
    }


    void set_bits_isr(bits_type bitstoset) {
        if (aui_in_isr()) {
            m_raw |= bitstoset;
        }
    }

    bits_type set_bits(bits_type bitstoset) {
        if (aui_in_isr()) { set_bits_isr(bitstoset) return 0; }

        bits_type old = m_raw;
        // Nur Bits setzen, die NICHT gelockt sind
        bits_type allowed = bitstoset & ~m_lock;
        m_raw |= allowed;
        return old;
    }

    bits_type get_bits(bits_type mask, aui_bool_t clearOnExit) {
        bits_type value = m_raw & mask;

        if (clearOnExit == AUI_TRUE && aui_in_isr() == AUI_FALSE) {
            // Nur Bits löschen, die NICHT gelockt sind
            bits_type allowed = mask & ~m_lock;
            m_raw &= ~allowed;
        }

        return value;
    }


    void clear_bits_isr(bits_type bitstoclear) {
        m_raw &= ~bitstoclear;
    }

    bits_type clear_bits(bits_type bitstoclear) {
        if (aui_in_isr()) { clear_bits_isr(bitstoclear) return 0; }

        bits_type _old = m_raw;
        bits_type allowed = bitstoclear & ~m_lock;
        m_raw &= ~allowed;
        return _old;
    }
    bits_type sync(this_type& other, bits_type bitstosync, aui_bool_t clearOtherBevorSync) {
        if (aui_in_isr()) { return 0; }

        bits_type old = other.m_raw;

        // Nur Bits synchronisieren, die NICHT gelockt sind
        bits_type allowed = bitstosync & ~other.m_lock;

        if (clearOtherBevorSync == AUI_TRUE)
            other.m_raw &= ~allowed;

        other.m_raw |= (m_raw & allowed);

        return old;
    }
    aui_bool_t try_set_bits(bits_type bitstoset) {
        if (aui_in_isr()) {
            m_raw |= bitstoset;
            return AUI_TRUE;
        }

        if (m_lock & bitstoset) {
            return AUI_FALSE;
        }

        m_raw |= bitstoset;
        return AUI_TRUE;
    }
};



using aui_bits_group = aui_bitsgroup<AUI_BITS_GROUP_TYPE, AUI_BITS_GROUP_SIZE>;