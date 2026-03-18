
# AUI — Arduino User Interface Framework

<table border="0" cellspacing="0" cellpadding="0" style="border:none !important; ">
<tr style="border:none !important;">
<td width="200" style="border:none !important;"><img align="left" src="https://github.com/RoseLeDark/arduino-aui/raw/main/logo.svg"  /> </td>
<td style="border:none!important;">
<p>AUI is a deterministic, event‑driven hardware framework for microcontrollers.
It provides a modular, message‑based architecture that unifies digital I/O, sensors, buses, displays, and communication modules under a single, consistent system.</p></td>
</tr>
</table>
Unlike traditional Arduino development, AUI does not rely on .ino files, implicit globals, or the loop() pattern.
Instead, it introduces:

- a central message dispatcher,
- a strict element lifecycle,
- compile‑time configuration,
- and a predictable, reproducible execution model.

AUI is designed for professional embedded developers, industrial prototyping, and deterministic hardware architectures where clarity, modularity, and reproducibility matter.

## Key Features

### Deterministic, Event‑Driven Architecture
All components communicate through a unified message interface:

- `MSG_ONSETUP` — initialization  
- `MSG_ONLOOP` — periodic update  
- `MSG_RESET` — reset internal state  
- Hardware‑specific messages (UART, GPIO, I²C, SPI, etc.)

No hidden magic, no implicit Arduino behavior.

### Modular Element System
AUI provides a clean, extensible hierarchy:

- `IElement` — base class for all hardware modules  
- `IElementWithID<TID>` — compile‑time element identity  
- `IVisualElement<TID>` — display/UI elements  
- `aui_board` — deterministic composite container  
- `aui_basic_button` — press/hold/release/click/double‑click  
- `aui_led` — digital output abstraction  
- `aui_uart` — UART interface with event structures  

All elements are **statically allocated** and stored in fixed arrays.

### Hardware Abstraction Without Runtime Overhead
AUI treats hardware modules like UI widgets:

- Buttons behave like UI buttons  
- LEDs behave like UI indicators  
- UART behaves like a UI text stream  
- Sensors behave like UI data sources  

Everything follows the same message‑driven pattern.

### Zero Dynamic Allocation
No `new`, no `malloc`, no fragmentation.  
All memory is known at compile time.

### Reproducible Behavior
AUI is designed for:

- deterministic state machines  
- predictable timing  
- reproducible hardware behavior  
- static analysis and debugging  

## Installation

### Installation (PlatformIO Only)

AUI is currently available exclusively through PlatformIO.
Arduino IDE, Arduino CLI, and CMake‑based toolchains are not supported at this time.

#### Install via PlatformIO Registry
Add AUI to your platformio.ini:

```ini
[env:my_build_env]
platform = infineonxmc
framework = arduino

lib_deps =
    pba3h11aso/AUI ^ 0.3.13
```

**Install a specific version**
```ini

lib_deps =
    pba3h11aso/AUI @0.3.13
```

**Install directly from GitHub (development version)**
```ini

lib_deps =
    https://github.com/RoseLeDark/arduino-aui.git
```
AUI includes a complete library.json manifest for seamless PlatformIO integration.

## Quickstart (Professional Style)

### 1. Include AUI

```cpp
#include <AUI/aui.h>
```

### 2. Define your board and elements

```cpp
class led_switch_button 
    : public aui_basic_button<2, HIGH, LOW, 250> 
{
public:
    uint8_t on_click(const IElement* sender, aui_event* event) override {
        uint8_t led = 1;
        auisystem.send_message(this, MSG_LED_SWITCH, &led, sizeof(led));
        return 0;
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
    aui_led<13, HIGH> m_led;
};
```

### 3. Setup and loop

```cpp
my_board board;

void setup() {
    auisystem.set_handler(
        [](IElement* sender, uint8_t msg, void* arg, uint16_t size) {
            return board.handle_message(sender, msg, arg, size);
        }
    );
    auisystem.on_setup();
}

void loop() {
    auisystem.on_loop();
}
```
## Target Audience

AUI is designed for:

- Embedded developers  
- Industrial prototyping  
- Deterministic hardware systems  
- Teams that avoid `.ino` and prefer clean C++ architectures  
- Projects requiring reproducible behavior and modularity  

Not for:

- beginners  
- Arduino‑style sketches  
- dynamic memory heavy systems  

## Status

AUI is under active development.  
The API is stable, and new modules (I²C, SPI, sensors, displays, I²S, CAN, ESP32 WiFi/BT) are planned.

See [FUTURE.md](https://github.com/RoseLeDark/arduino-aui/blob/main/FUTURE.md) for the roadmap.

## Changelog

### [0.3.14] – 2026‑03‑18

#### Added
- PCINT support for AVR‑based GPIO elements  
  - automatic TPIN → PCINT mapping  
  - per‑group mask handling (PCMSK0/1/2)  
  - centralized registration via `aui_gpio_pcint_register_pin()`  
  - unified interrupt dispatch into the AUI event system  
- `aui_pcint_event`
  - introduced the new event type aui_pcint_event, carrying id and state  
  - supports both raw‑pointer message construction and typed payload creation
  - integrates seamlessly with the AUI message bus and interrupt dispatch
- `aui_digital_input_pullup<TPIN>`
  - variant of the digital input element using `INPUT_PULLUP`  
  - optional PCINT activation via `set_pcint_enable()`  

#### **Improved**
- Consolidated GPIO infrastructure through `aui_base_pin<TPIN>`  
  - provides explicit `set_pcint_enable()`  
  - no automatic activation; each class opts in as needed  

### [0.3.13] – 2026‑03‑17
#### Added
- New template parameters for `aui_basic_button`:
  - `TPRESS` and `TUPRESS` to define active logic levels (HIGH/LOW)
  - Fully hardware‑agnostic press/hold/release detection
- Press/Hold/Release callbacks (`on_pressed`, `on_released`)
- Extended click state machine:
  - deterministic edge detection
  - universal double‑click logic
- Global messages:
  - `MSG_ENABLE` (wake/activate element)
  - `MSG_DISABLE` (sleep/deactivate element)
  - `MSG_RESET` (reset internal state machine)
  - `MSG_UART_WRITE` (send text to `aui_uart` via `aui_uart_event`)
  - `MSG_UART_READ` (read text from `aui_uart` via `aui_uart_event`)
- Added `aui_uart_event` structure

#### Changed
- Fully refactored button logic:
  - clear separation between edge, hold, and release states
  - click detection now only triggered on `TUPRESS → TPRESS`
- Renamed `aui_button` to `aui_basic_button` to simplify specialization
- Stabilized GPIO handling (no implicit assumptions about pull‑up/pull‑down)

#### Fixed
- Incorrect press detection when using inverted logic
- Uninitialized timestamps in the double‑click state machine

See [FUTURE.md](https://github.com/RoseLeDark/arduino-aui/blob/main/CHANGELOG.md) for more.

##  License

Licensed under the **European Union Public License 1.2 (EUPL‑1.2)**. See [LICENSE.md](https://github.com/RoseLeDark/arduino-aui/blob/main/LICENSE.md)
