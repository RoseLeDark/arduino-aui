#include <Arduino.h>

#define AUI_CONFIG_HIDE_LOOP  AUI_CONFIG_ON
#include "AUI/aui.h"
#include "AUI/aui_devices.h"

#define LED_PIN         13
#define BUTTON_PIN      2
#define LED_AUID_ID     LED_PIN

using basic_board = aui_board<8>;

class led_switch_button : public aui_button <BUTTON_PIN, 250UL> {
public:
  virtual uint8_t on_click(const IElement* sender, const uint64_t tick ) override { 
 
    sAuiSendEvent(aui_idble_payload, this, MSG_GPIO_SWITCH, LED_AUID_ID );
    return 0;
  }

  void register_elements(basic_board* board) {
    board->add_element(this);
    board->add_element(&m_led);
  }
private:
  aui_led<LED_PIN> m_led;
};

class arduino_board : public basic_board {
public:
    arduino_board()  { 
      m_button.register_elements(this);
    }
private:
    led_switch_button m_button;
    
};
arduino_board board;

auier_t messages(const IElement* sender, const uint8_t msg, void* arg, const uint16_t size) {
    return board.handle_message(sender, msg, arg, size);
}