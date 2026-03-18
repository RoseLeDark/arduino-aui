#include <Arduino.h>
#include "AUI/aui.h"

#define LED_PIN       13
#define BUTTON_PIN     2

#define UART_AUI_ID     10
#define LED_AUID_ID     LED_PIN

class led_switch_button : public aui_button <BUTTON_PIN, 250UL> {
public:
  virtual uint8_t on_click(const IElement* sender, aui_event* event ) override { 
 
    auisystem.send_massage<aui_idble_event>(this, MSG_GPIO_SWITCH, aui_idble_event::make(LED_AUID_ID));
    auisystem.send_massage<aui_uart_event>(this, MSG_UART_WRITE, aui_uart_event::make(UART_AUI_ID, "Button Click", AUI_LOG_LEVEL_INFOS) );

    return 0;
  }
};

class arduino_board : public aui_board<8> {
public:
    arduino_board() : m_serial() { 
      add_element(&m_button);
      add_element(&m_led);
      add_element(&m_serial);
    }
private:
    led_switch_button m_button;
    aui_led<LED_PIN> m_led;
    aui_serial<&Serial, UART_AUI_ID> m_serial;
};
arduino_board board;

void setup() {
  auisystem.set_handler([](const IElement* sender, const uint8_t msg, void* arg, const uint16_t size) {
        return board.handle_message(sender, msg, arg, size);
    });
  auisystem.on_setup();
}

void loop() {
  auisystem.on_loop(); 
}


/* OUTPUT
--- Terminal on COM4 | 9600 8-N-1
--- Available filters and text transformations: debug, default, direct, hexlify, log2file, nocontrol, printable, send_on_enter, time
--- More details at https://bit.ly/pio-monitor-filters
--- Quit: Ctrl+C | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H
[ 1248 ] info: Button Click
[ 2208 ] info: Button Click
[ 2832 ] info: Button Click
[ 3184 ] info: Button Click
[ 3488 ] info: Button Click
[ 3808 ] info: Button Click
[ 4288 ] info: Button Click
[ 4560 ] info: Button Click
....
*/