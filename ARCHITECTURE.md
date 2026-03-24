# 📘 ARCHITECTURE.md — AUI Framework Architecture

AUI (Abstract UI for Embedded Systems) is a deterministic, event‑driven hardware framework designed for microcontrollers.  
It provides a UI‑like architecture for hardware elements such as buttons, LEDs, sensors, buses, and communication modules.

AUI focuses on:
- compile‑time configuration (templates)
- deterministic state machines
- zero dynamic allocation
- reproducible behavior
- a unified message‑based interaction model

## 1. Core Concepts

### 1.1 Elements
Every component in AUI is an **Element**.  
Elements are lightweight objects that receive messages and react to them.

Base classes:
- `IElement` — minimal element with lifecycle and message handling
- `IElementWithID<TID>` — element with compile‑time ID
- `IVisualElement<TID>` — visual element with ID (for displays, UI widgets)

### 1.2 Message System
AUI uses a **central message dispatcher**.  
Messages are small numeric codes (uint8_t) that describe actions or events.

Global messages include:
- `MSG_ENABLE` — activate element (wake)
- `MSG_DISABLE` — deactivate element (sleep)
- `MSG_RESET` — reset internal state
- `MSG_UART_WRITE` — send text via UART
- `MSG_UART_READ` — read text via UART

Messages are delivered via:
```cpp
on_MESSAGE(IElement* sender, aui_event_ex<aui_idble_payload>* event)
```

### 1.3 Event Structures
Events carry additional data for messages.

Core event types:
- `aui_event` — generic event
- `aui_event_ex<aui_idble_payload>` — event with element ID
- `aui_uart_event` — UART read/write payload

## 2. Hardware Abstraction Layers

### 2.1 Digital I/O
- `aui_digital_input<pin>`  
- `aui_digital_output<pin>`

These provide deterministic GPIO access without dynamic memory.

### 2.2 Communication Buses
Planned and implemented:
- `aui_uart` (implemented)
- `aui_i2c` (planned for 0.4.0)
- `aui_spi` (planned for 0.4.0)
- `aui_i2s` (planned for 0.5.0)
- `aui_usb_host` (planned for 1.0)
- `aui_can` (planned for 1.0)

All bus modules follow the same message‑based pattern.

## 3. Button Architecture

### 3.1 Template‑Configurable Logic
`aui_basic_button<pin, TPRESS, TUPRESS, TDOUBLE_CLICK_TIME>`

Compile‑time parameters:
- `TPRESS` — logic level for "pressed"
- `TUPRESS` — logic level for "released"
- `TDOUBLE_CLICK_TIME` — double‑click window

### 3.2 State Machine
The button implements:
- Press (edge)
- Hold
- Release
- Click
- Double‑Click

All logic is deterministic and based on edge transitions.

## 4. Board Architecture

### 4.1 aui_board
A board is a container for elements:
- stores them in a static array
- dispatches messages
- manages lifecycle (`on_begin`, `on_loop`, `on_reset`)

Boards allow:
- grouping hardware modules
- deterministic update order
- reproducible behavior

## 5. Visual Elements (Future)
Visual elements will use:
- `IVisualElement<TID>`
- message‑based drawing
- deterministic rendering pipelines

Planned modules:
- `aui_oled_i2c` (0.4.5)

## 6. Sensor Architecture (Future)
Sensors follow a unified pattern:
- static configuration
- event‑based updates
- no dynamic allocation

Planned:
- `aui_dht`
- `aui_barometer_i2c`
- `aui_xyz_i2c`
- `aui_9achsen_i2c`
- `aui_temp_ao`

## 7. Communication Modules (Future)
Planned for 1.0:
- `aui_wifi_esp32` (UART‑based AT interface)
- `aui_bt_esp32`
- `aui_usb_host`

All follow the same event‑driven architecture.

## 8. Design Principles

### 8.1 Determinism
Every element behaves identically across runs.

### 8.2 Zero Dynamic Allocation
No `new`, no `malloc`, no fragmentation.

### 8.3 Compile‑Time Configuration
Templates define:
- IDs
- pins
- logic levels
- timing
- bus addresses

### 8.4 Message‑Driven
All interactions use messages, not direct calls.

### 8.5 Hardware‑Agnostic
Logic is independent of:
- pull‑up/pull‑down
- inverted logic
- bus implementation

## 9. Roadmap Overview
See `FUTURE.md` for detailed version plans.

