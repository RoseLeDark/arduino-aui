
<h1> AUI — Arduino User Interface Framework</h1>
<table>
<tr>
<td width="200" ><img align="left" src="logo.svg"  /> </td>
<td>
<p>AUI is a lightweight, deterministic, event‑driven UI and hardware framework for Arduino.  
It provides a modular message‑based architecture that unifies buttons, LEDs, displays, UART interfaces, and custom components under a single, elegant system.

AUI does **not** rely on the traditional `loop()`‑driven Arduino style. Instead, it uses a **central message bus**, a **clean element hierarchy**, and **predictable lifecycle events**.</p></td>
</tr>
</table>

## ✨ Features

- **Event‑driven architecture**  
  Every component receives messages through a unified `handle_message()` interface.

- **Deterministic lifecycle**  
  - `MSG_ONSETUP` for initialization  
  - `MSG_ONLOOP` for periodic updates  
  - `MSG_PAINT` for rendering  
  - Hardware‑specific messages (UART, LED, etc.)

- **Modular element system**  
  - `IElement` — base class for all components  
  - `IVisualElement` — UI elements with paint support  
  - `aui_board` — composite container  
  - `aui_button` — click + double‑click detection  
  - `aui_led` — simple digital LED widget  
  - `aui_serial` — UART integration with log levels

- **Zero dynamic allocation**  
  All elements are statically allocated and stored in fixed arrays.

- **Hardware abstraction**  
  Components like UART, LEDs, buttons, displays, etc. behave like UI elements.

## 📦 Installation

Clone the repository into your Arduino `libraries` folder:

```bash
git clone https://github.com/kschattenfeld/arduino-aui.git
```

Or install via PlatformIO using `library.json`.

## 🚀 Quickstart

### 1. Include AUI

```cpp
#include <AUI/aui.h>
```

### 2. Create a board

```cpp

class led_switch_button : public aui_button <2, 250UL> {
public:
  virtual void on_click() override { 
    uint8_t led = 1;
    char* buf = "Button Click";

    auisystem.send_massage(this, MSG_LED_SWITCH, &led, sizeof(led));
  }
};

class my_board : public aui_board<8> {
public:
    void on_create() override {
      add_element(&m_button);
      add_element(&m_led);
    }
private:
    led_switch_button m_button;
    aui_led<13, 1> m_led;
};
```

### 3. Setup and loop

```cpp
my_board board;

void setup() {
  auisystem.set_handler([](IElement* sender, uint8_t msg, void* arg, uint16_t size) {
        return board.handle_message(sender, msg, arg, size);
    });
  auisystem.on_setup();
}

void loop() {
  auisystem.on_loop();
}
```

## 🧩 Architecture Overview

```
+----------------------+
|      aui_system      |
|  (message dispatcher)|
+----------+-----------+
           |
           v
+----------------------+
|      aui_board       |
| (composite element)  |
+----+-----------+-----+
     |           |
     v           v
+--------+   +--------+
| button |   |   led  |
+--------+   +--------+
```

AUI is intentionally simple:  
**Messages flow downward, events bubble upward.**


## 📝 License

This project is licensed under the **European Union Public License 1.2 (EUPL‑1.2)**.  
See the `LICENSE.md` file for details.

## 📚 Status

AUI is currently in early development (v0.1.0).  
The API is stable, but additional widgets and hardware modules will be added over time.

Contributions, ideas, and discussions are welcome.
