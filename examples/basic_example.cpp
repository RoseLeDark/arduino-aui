#include <Arduino.h>
#include "AUI/aui.h"
#include "AUI/aui_devices.h"

#define LED_PIN       13
#define BUTTON_PIN     2
#define LED_AUID_ID     LED_PIN

class led_switch_button : public aui_button <BUTTON_PIN, 250UL> {
public:
  virtual uint8_t on_click(const IElement* sender, const uint64_t tick ) override { 
 
    sAuiSendEvent(aui_idble_payload, this, MSG_GPIO_SWITCH, LED_AUID_ID );
    return 0;
  }
};

class arduino_board : public aui_board<8> {
public:
    arduino_board()  { 
      add_element(&m_button);
      add_element(&m_led);
    }
private:
    led_switch_button m_button;
    aui_led<LED_PIN> m_led;
};
arduino_board board;

void setup() {
  auisystem.set_handler([](const IElement* sender, const uint8_t msg, void* arg, const uint16_t size) {
        
    });
  auisystem.on_setup();
}

void loop() {
  auisystem.on_loop(); 
}
