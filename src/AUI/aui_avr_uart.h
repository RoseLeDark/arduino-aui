#pragma once
/* #include "aui_config.h"
#include "aui_arch.h"
#include "aui_types.h"
#include "aui_element.h"



enum class aui_uart_mode : uint8_t {
    async,
    sync_master,
    sync_slave  
};
enum class aui_uart_parity : uint8_t {
    none,
    even,
    odd
};

struct aui_uart_hal_register {
    volatile uint8_t*  UDR_REG;
    volatile uint8_t*  UCSRA_REG;
    volatile uint8_t*  UCSRB_REG;
    volatile uint8_t*  UCSRC_REG;
    volatile uint16_t*  UBRR_REG;
     uint8_t TXEN_BIT;
     uint8_t RXEN_BIT;
     uint8_t UDRE_BIT;
     uint8_t RXC_BIT;
     uint8_t UCSZ0_BIT;
     uint8_t UCSZ1_BIT;
     uint8_t UMSEL0_BIT;
     uint8_t UMSEL1_BIT;
     uint8_t UPM0_BIT;
     uint8_t UPM1_BIT;
     uint8_t USBS_BIT;
     uint8_t UCPOL_BIT;
     uint8_t U2X_BIT;
     uint8_t RXCIE_BIT;
     uint8_t TXCIE_BIT;
};


struct aui_uart_hal {
    

    aui_uart_hal() = default;

    aui_uart_hal(aui_uart_hal_register map) : reg(map) { }

    uint16_t write(const uint8_t* data, uint16_t size) {
            return 9;
    }
    uint16_t read(uint8_t* data, uint16_t size) {
        return 9;
    }
     uint8_t available() {
        return (*(reg.UCSRA_REG) & (1 << reg.RXC_BIT) ) ? 1 : 0;
    }
     auier_t on_begin(uint32_t TBAUD, uint8_t TDATA_BITS, aui_uart_parity TPARITY, uint8_t TSTOP_BITS, aui_uart_mode TMODE,
          uint8_t TCLOCK_POL, aui_bool_t TDOUBLE_SPEED) {
       
       if (TMODE == aui_uart_mode::async) {
            if (TDOUBLE_SPEED == AUI_TRUE) {
                *(reg.UBRR_REG) = (uint16_t)(F_CPU / 8 / TBAUD - 1);
                *(reg.UCSRA_REG) |= (uint8_t)(1 << reg.U2X_BIT);
            } else {
                *(reg.UBRR_REG) = (uint16_t)(F_CPU / 16 / TBAUD - 1);
                *(reg.UCSRA_REG) &= (uint8_t)~(1 << reg.U2X_BIT);
            }
        } else {
            *(reg.UBRR_REG) = (uint16_t)(F_CPU / 2 / TBAUD - 1);
        }
       // UBRR_REG = ubrr;<-- BÖSE
        
        // Frame-Format
        /// uint8_t ucsrc = 0;<-- BÖSE
        // Data bits
        *(reg.UCSRC_REG) &= (uint8_t)~((1 << reg.UCSZ0_BIT) | (1 << reg.UCSZ1_BIT));
        if (TDATA_BITS == 8) *(reg.UCSRC_REG) |= (uint8_t)((1 << reg.UCSZ0_BIT) | (1 << reg.UCSZ1_BIT));
        else if (TDATA_BITS == 7) *(reg.UCSRC_REG) |= (uint8_t)(1 << reg.UCSZ0_BIT);
        // Parity
        *(reg.UCSRC_REG) &= (uint8_t)~((1 << reg.UPM0_BIT) | (1 << reg.UPM1_BIT));
        if (TPARITY == aui_uart_parity::even) *(reg.UCSRC_REG) |= (uint8_t)(1 << reg.UPM1_BIT);
        else if (TPARITY == aui_uart_parity::odd) *(reg.UCSRC_REG) |= (uint8_t)((1 << reg.UPM1_BIT) | (1 << reg.UPM0_BIT));
        // Stop bits
        *(reg.UCSRC_REG) &= (uint8_t)~(1 << reg.USBS_BIT);
        if (TSTOP_BITS == 2) *(reg.UCSRC_REG) |= (uint8_t)(1 << reg.USBS_BIT);
        // Mode
        *(reg.UCSRC_REG) &= (uint8_t)~((1 << reg.UMSEL0_BIT) | (1 << reg.UMSEL1_BIT));
        if (TMODE == aui_uart_mode::sync_master) *(reg.UCSRC_REG) |= (uint8_t)(1 << reg.UMSEL0_BIT);
        else if (TMODE == aui_uart_mode::sync_slave) *(reg.UCSRC_REG) |= (uint8_t)((1 << reg.UMSEL0_BIT) | (1 << reg.UMSEL1_BIT));
        // Clock polarity
        *(reg.UCSRB_REG) |= (uint8_t)((1 << reg.TXEN_BIT) | (1 << reg.RXEN_BIT));
        
        /// UCSRC_REG = ucsrc;<-- BÖSE
        
        // Enable TX und RX
        *(reg.UCSRB_REG) |= (uint8_t)((1 << reg.TXEN_BIT) | (1 << reg.RXEN_BIT));
        
        // Bei Slave: Interrupt aktivieren
        // Slave: RX‑Interrupt
        if (TMODE == aui_uart_mode::sync_slave) {
            *(reg.UCSRB_REG) |= (uint8_t)(1 << reg.RXCIE_BIT);
        }
        
        return AUI_OK;
    }

};

#define HAS_UART_0   1

#ifdef HAS_UART_0
extern aui_uart_hal AUI_UART0_HAL;
#endif

#ifdef HAS_UART_1
extern aui_uart_hal AUI_UART1_HAL;
#endif

#ifdef HAS_UART_2
extern aui_uart_hal AUI_UART2_HAL;
#endif

#ifdef HAS_UART_3
extern aui_uart_hal AUI_UART3_HAL;
#endif


template< uint8_t TID, uint32_t TBAUD, uint8_t TDATA_BITS, aui_uart_parity TPARITY, uint8_t TSTOP_BITS, aui_uart_mode TMODE,        // async, sync master, sync slave
          uint8_t TCLOCK_POL, aui_bool_t TDOUBLE_SPEED>
class aui_uart : public IElementWithID <TID> {
public:
    using base_type = IElementWithID <TID>;


    virtual auier_t handle_message(const IElement* sender, const uint8_t msg, void* arg, const uint16_t size) override {
        if(base_type::handle_message(sender, msg, arg, size) == 0) return 0;
        
        if (msg == MSG_UART_WRITE && base_type::m_bEnable == 0) {
            
            return on_write(sender, static_cast<aui_event<aui_idble_payload>*>(arg));
        }
        if (msg == MSG_UART_READ && base_type::m_bEnable == 0) {
            return on_read_text(sender, static_cast<aui_event<aui_idble_payload>*>(arg));
        }
        return AUI_ERROR_NOTHANDLED;
    }

    static void on_serial_event() {
        // cheak aui_uart_hal buffer have data to send and read data 
    }
protected:
    auier_t on_begin(const IElement* sender, const aui_event<aui_idble_payload>* event) override {
        // Baudrate berechnen
        return m_hal.on_begin(TBAUD, TDATA_BITS, TPARITY, TSTOP_BITS, TMODE, TCLOCK_POL, TDOUBLE_SPEED);
    }
    virtual uint8_t on_write(const IElement* sender, const aui_event<aui_uart_payload>* event) {
        if(event->get_id() != TID) return 1;

         // TODO RINGBUFFER AND SERIAL TIMER On 
        return AUI_OK;;
    }

    virtual uint8_t on_read_text(const IElement* sender, aui_event<aui_uart_payload>* event) {
        if(event->get_id() != TID) return 1;
        
        // TODO RINGBUFFER AND SERIAL TIMER On 
        
         
        return AUI_OK;;
    }
};


 */