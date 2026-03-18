
## [0.3.14] – 2026‑03‑18

### Added
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

### **Improved**
- Consolidated GPIO infrastructure through `aui_base_pin<TPIN>`  
  - provides explicit `set_pcint_enable()`  
  - no automatic activation; each class opts in as needed  

## [0.3.13] – 2026‑03‑17
### Added
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

### Changed
- Fully refactored button logic:
  - clear separation between edge, hold, and release states
  - click detection now only triggered on `TUPRESS → TPRESS`
- Renamed `aui_button` to `aui_basic_button` to simplify specialization
- Stabilized GPIO handling (no implicit assumptions about pull‑up/pull‑down)

### Fixed
- Incorrect press detection when using inverted logic
- Uninitialized timestamps in the double‑click state machine

## [0.3.0]
### Added
- `template<uint8_t TID> IElementWithID<TID>`  
  Base class for non‑visual elements requiring an ID (e.g., buttons)
- GPIO abstractions (`aui_digital_input`, `aui_digital_output`)

### Modified
- LED element (`aui_led`) now inherits from `aui_digital_output`
- UART element (`aui_basic_uart`) now inherits from `aui_digital_input`
- Board container (`aui_board`) now inherits from  
  `template<uint8_t TID> IElementWithID<TID>`
- Added new event types to the central message system  
  (`aui_idble_event`, `aui_event`) and updated message handler to  
  `on_MESSAGE(IElement* e, aui_event* event)`
- Updated `template<uint8_t TID> IVisualElement<TID>`  
  (added template parameter TID for visual elements with IDs)

## [0.1.1]
### Added
- Central message system (`IElement`, `aui_system`)
- Basic implementations of `aui_basic_button` and `aui_led` for clarity

## [0.1.0] – Initial Release
### Added
- First functional version of the AUI framework
- Static element array and central message dispatcher
- Foundational UI‑like architecture for hardware elements
