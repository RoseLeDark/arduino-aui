#pragma once

#include <Arduino.h>

#include "aui_element.h"
#include "aui_config.h"

#define MSG_I2C_WRITE 150
#define MSG_I2C_READ  151
#define MSG_I2C_INTRT 152

#if AUI_PLATFORM_AVR  == AUI_CONFIG_USED

PreValue = 1/4/16/64
#define  aui_i2c_begin(freq, PreValue) \ 
    TWSR = (TWSR & 0xFC) | PreValue; \
    TWBR = ((F_CPU / freq) - 16) / 2;

#define aui_i2c_start() \
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); \
    while (!(TWCR & (1<<TWINT)));

#define aui_i2c_stop() \
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);

#define aui_i2c_write_byte(uint8_t data) \
     TWDR = data; \
    TWCR = (1<<TWINT)|(1<<TWEN); \
    while (!(TWCR & (1<<TWINT)));

#define aui_i2c_read_byte_ack(RET) \
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA); \
    while (!(TWCR & (1<<TWINT))); \
    RET = TWDR;

#define aui_i2c_read_byte_nack(RET) \
    TWCR = (1<<TWINT)|(1<<TWEN); \
    while (!(TWCR & (1<<TWINT))); \
    RET = TWDR;

#define aui_i2c_status(RET) ((TWSR & 0xF8) != RET)

#endif

template<uint8_t TADDR>
class aui_i2c_test_module {
public:
    void begin(uint32_t freq) {
        aui_i2c_begin(freq);
    }
    uint16_t write(const uint8_t* data, uint16_t size) { 
        aui_i2c_start();
        if (aui_i2c_status(0x08)) return 0;

        aui_i2c_write_byte((TADDR << 1) | 0); // Write
        if (aui_i2c_status(0x18)) return 0;

        uint16_t w = 0;
        for(; w<size; w++) {
            aui_i2c_write_byte(data[w]);
            if (aui_i2c_status(0x28)) 
                break;
        }
        aui_i2c_stop();
        return w; 
    }
    uint16_t read(uint8_t* data, uint16_t size) { 
        aui_i2c_start();
        if (aui_i2c_status(0x08)) return 0;

        aui_i2c_write_byte((TADDR << 1) | 1); // Read
        uint16_t r = 0;

        for (; r < size; r++) i<size; i++) {
            if (r < size - 1) {
                aui_i2c_read_byte_ack(data[r]);
                if (aui_i2c_status(0x50)) break;
            } else {
                aui_i2c_read_byte_nack(data[r]);
                if (aui_i2c_status(0x58)) break;
            }
        }
        aui_i2c_stop();
        return r;
    }
};

template <uint8_t TID, class TModule = aui_i2c_test_module<0x00> >
class aui_i2c : public IElementWithID<TID> {
public:
    using module_type = TModule;
    using base_type = IElementWithID<TID>;
    using value_type = typename TModule::value_type;

    uint16_t write(const uint8_t* data, uint16_t size) {
        return m_pModule.write(data, size);
    }
    uint16_t read(uint8_t* data, uint16_t size) {
        return m_pModule.read(data, size);
    }

    uint8_t handle_message(const IElement* sender, const uint8_t msg, void* arg, const uint16_t size) override { 
        if(base_type::handle_message(sender, msg, arg, size) == 0) return 0;

        if (msg == MSG_I2C_WRITE && base_type::m_bEnable == 0) { 
            return on_i2c_write(sender, &aui_idble_event::make(arg, size) );
        }
        if (msg == MSG_I2C_READ  && base_type::m_bEnable == 0) {
            return on_i2c_read(sender, &aui_idble_event::make(arg, size) );
        }
        return 1;
    }
protected:
    uint8_t on_i2c_write(const IElement* sender, aui_idble_event* event ) {
        if(event->get_id() != TID) return 1;

        uint16_t written = write(event->get_as<uint8_t*>(), event->get_size() );
        event->set_size(written);

        return 0;
    }

    uint8_t on_i2c_read(const IElement* sender, aui_idble_event* event ) {
        if(event->get_id() != TID) return 1;

        uint16_t readed = read(event->get_as<uint8_t*>(), event->get_size() );
        event->set_size(readed);

        return 0;
    }
protected:
    module_type m_pModule;
};

template<uint8_t TID, uint8_t TADDR, uint8_t TSIZE>
class aui_i2c_slave : public IElementWithID<TID> {
    // ...
protected:
    static uint8_t m_buffer[TSIZE];
    static uint8_t current ;
    static uint8_t ready_flag;
public:
    static void on_interrupt(uint8_t byte) { // IN ISR(TWI_vect)  on_interrupt(TWDR);
       // 
       if (ready_flag == 0) {
            m_buffer[current++] = byte;

            if (current == TSIZE) {
                ready_flag = 1;
                current = 0; // optional: zurücksetzen
            }
        }
    }
public:
    aui_i2c_slave(uint8_t dest) 
        : m_destAddrs(dest) { }

    uint8_t* get_buffer() { return aui_i2c_slave<TID, TADDR>::m_buffer; }

protected:
    /**
     * @brief Optional periodic update hook.
     *
     * Called automatically when the element receives MSG_ONLOOP.
     * Derived classes override this to implement polling or state updates.
     */
    uint8_t on_update(const IElement* sender, const uint64_t ticks) override { 
        if(aui_i2c_slave<TID, TADDR>::ready_flag == 1) {
            auisystem.send_massage<aui_idble_event>(this, MSG_I2C_INTRT, aui_idble_event::make(aui_i2c_slave<TID, TADDR>::m_buffer, TSIZE, m_destAddrs));
            aui_i2c_slave<TID, TADDR>::ready_flag = 0;
        }
        return 0; 
    }

    void set_dest_tid(uint8_t dest) {m_destAddrs = dest; }
protected:
    uint8_t m_destAddrs;
};

template<uint8_t TID, uint8_t TADDR, uint8_t TSIZE>
uint8_t aui_i2c_slave<TID, TADDR, TSIZE>::m_buffer[TSIZE];

template<uint8_t TID, uint8_t TADDR, uint8_t TSIZE>
uint8_t aui_i2c_slave<TID, TADDR, TSIZE>::current = 0;

template<uint8_t TID, uint8_t TADDR, uint8_t TSIZE>
uint8_t aui_i2c_slave<TID, TADDR, TSIZE>::ready_flag = 0;