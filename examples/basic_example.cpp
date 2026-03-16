#include <Arduino.h>
#include "AUI/aui.h"

class led_switch_button : public aui_button <2, 250UL> {
public:
  virtual void on_click() override { 
    uint8_t led = 1;
    char* buf = "Button Click";

    auisystem.send_massage(this, MSG_LED_SWITCH, &led, sizeof(led));

    auisystem.send_massage(this, MSG_UART_INFO_TEXT, buf, strlen(buf));
  }
};

class seed_board : public aui_board<8> {
public:
    seed_board() : m_serial() { }
    void on_create() override {
      add_element(&m_button);
      add_element(&m_led);
      add_element(&m_serial);
    }
private:
    led_switch_button m_button;
    aui_led<13, 1> m_led;
    aui_serial<&Serial> m_serial;
};
seed_board board;

void setup() {
  auisystem.set_handler([](IElement* sender, uint8_t msg, void* arg, uint16_t size) {
        return board.handle_message(sender, msg, arg, size);
    });
  auisystem.on_setup();
}

void loop() {
  auisystem.on_loop();
}
