## [0.3.85] – 2026‑03‑24

### Added
- Added simple/extended event macros  
  - `sAuiSendEvent` (simple): send ID‑only events without payload  
  - `eAuiSendEvent` (extended): send typed payload events  
  - both macros generate zero‑overhead `aui_event<T>` instances  
  - replaces multiple legacy factory functions (`make()`) from old event classes

- Added `aui_arch.h`
  - foundation for future platform‑specific auto‑configuration  
  - prepares unified architecture handling for AVR, ESP32, SAMD, Teensy, and ARM  
  - integrates with existing `aui_config.h` platform detection

- Added `aui_error.h`
  - central definition of all AUI error codes (`AUI_ERROR_*`)
  - provides a unified error namespace for all modules

- Added `aui_types.h`
  - introduced `aui_bool_t` (alias for `uint8_t`)
    - replaces native `bool`, which is 4 bytes on several platforms (e.g. ESP32, ARM)
    - ensures deterministic 1‑byte boolean type across all architectures
  - introduced `auier_t` (alias for `uint8_t`)
    - dedicated error type used by AUI modules  
    - type definition only — error codes remain in `aui_error.h`


### Modified
- Reworked the entire event system (`aui_event (new)`)
  - replaced multiple specialized event classes (`aui_event (old)`, `aui_idble_event`,  
    `aui_uart_event`, `aui_pcint_event`) with a unified template‑based design  
  - introduced `aui_event<TPAYLOAD>` as the new generic event container  
    - supports typed payloads with zero‑overhead access  
    - unified ID‑based routing for all event types  
  - added specialization `aui_event<void*>` for raw pointer payloads  
    - replaces previous raw‑event handling logic  
    - provides `get_as<T>()` and `set_as<T>()` for typed reinterpretation  
  - significantly reduced code size by removing redundant event classes  
  - deterministic memory layout across all event types  
  - simplified event creation in `send_message()` and message handlers



## [0.3.80] – 2026‑03‑19

### Added
- Added set_pcint_enable() to aui_base_pin<TPIN>
  - provides a unified API for enabling PCINT on any GPIO‑based element
  - internally forwards to aui_gpio_pcint_register_pin(TPIN, pcintID)
  - removes duplicated PCINT logic from individual GPIO classes
  - ensures consistent PCINT activation across all digital input types

- Cross‑platform soft‑lock system (`aui_soft_lock`)
  - supports AVR, ESP32, SAMD, and STM32  
  - unified ISR‑detection via `aui_in_isr()`  
  - platform‑specific implementations:  
    - AVR: checks I‑bit in SREG  
    - ESP32: uses FreeRTOS `xPortInIsrContext()`  
    - ARM Cortex‑M: checks `SCB->ICSR` VECTACTIVE  
  - fallback implementation for non‑RTOS/non‑MCU environments  

- RAII lock guard (`aui_lock_guard<TLOCK, TID>`)  
  - deterministic lock/unlock behavior  
  - explicit owner‑ID validation  
  - zero‑overhead in optimized builds  

- `AUI_LOCKED()` macro  
  - concise lock‑scope syntax without requiring C++17  
  - integrates cleanly with the soft‑lock system  

- Introduced `aui_devices.h` as a unified alias layer for common hardware elements  

  > **Note:** All device aliases in `aui_devices.h` are zero‑overhead type aliases only.  
  > They do not implement any behavior. Users must derive their own classes and  
  > override event handlers (e.g., `on_click`, `on_change`) to define device logic.

  - semantic aliases for button‑based sensors:  
    - `aui_limit`, `aui_reed`, `aui_switch`, `aui_endstop`  
    - pull‑up variants: `aui_limit_pullup`, `aui_reed_pullup`, `aui_switch_pullup`, `aui_endstop_pullup`  
  - digital‑input aliases for typical sensor modules:  
    - `aui_motion` (PIR), `aui_flame`, `aui_rain`, `aui_hall`,  
      `aui_lightgate`, `aui_vibration`, `aui_knock`, `aui_gas`  
  - digital‑output aliases for actuator modules:  
    - `aui_relay`, `aui_ssr`, `aui_mosfet`, `aui_transistor`,  
      `aui_valve`, `aui_pump`, `aui_laser`, `aui_trigger`  

- Unified enable/disable handling across all GPIO‑based elements  
  (`aui_digital_input`, `aui_digital_input_pullup`,  
   `aui_digital_output`, `aui_analog_input`, `aui_analog_output`)
  - GPIO elements now ignore `MSG_GPIO_SWITCH` and `MSG_GPIO_WRITE` when disabled  
  - prevents unintended state changes while inactive  
  - ensures consistent behavior across all input/output types  

### I²C Framework
- Introduced `aui_i2c<TID, TModule>` as a new I²C master element  
  - fully message‑driven (`MSG_I2C_WRITE`, `MSG_I2C_READ`)  
  - supports arbitrary I²C modules via template parameter  
  - zero‑overhead integration with the AUI message bus  

- Added `aui_i2c_test_module<TADDR>` as a reference implementation  
  - basic read/write operations for AVR TWI hardware  
  - template for device‑specific modules (e.g., MPU6050, OLED, sensors)  

- Added `aui_i2c_slave<TID, TADDR, TSIZE>`  
  - interrupt‑driven I²C slave receiver  
  - static buffer with ready‑flag mechanism  
  - dispatches `MSG_I2C_INTRT` when a full packet is received  
  - integrates cleanly with `on_update()` for periodic polling  

- Added AVR‑specific low‑level I²C macros  
  - `aui_i2c_begin()`, `aui_i2c_start()`, `aui_i2c_stop()`  
  - `aui_i2c_write_byte()`, `aui_i2c_read_byte_ack()`, `aui_i2c_read_byte_nack()`  
  - `aui_i2c_status()` for TWI status checking  
  - deterministic, blocking I²C operations without overhead  

### Modified
- Reworked `aui_system::on_loop()`  
  - replaced `NULL` loop argument with a global tick counter (`__global_ticks`)  
  - deterministic tick progression independent of message handler  
  - added `detail::aui_global_get_ticks()` for unified tick access  

- Updated message dispatch for `MSG_ONLOOP`  
  - now forwards the current tick value  
  - enables time‑aware components without boilerplate  

- Unified I²C message handling with enable/disable semantics  
  - `MSG_I2C_WRITE` and `MSG_I2C_READ` only processed when enabled  

- Standardized event forwarding using `aui_event<aui_idble_payload>::make()`  
  - simplifies payload handling for variable‑length I²C transfers  
  - ensures consistent ID‑based routing  

### Improved
- Timer infrastructure  
  - weak callback hooks (`aui_timer0_callback`, `aui_timer_callback`)  
  - safe hook into Arduino’s `TIMER0_OVF_vect` without breaking `millis()`  
  - configurable Timer1 CTC dispatch (`TIMER1_COMPA_vect`)  
  - compile‑time frequency selection via `AUI_CONFIG_TIMER_*`  
  - deterministic prescaler/OCR configuration  

- `MSG_RESET` now forces the element back into the enabled state (`set_enable(0)`)  
  - guarantees deterministic reset behavior  
  - avoids half‑disabled states  

- Centralized enable‑state logic  
  - reduces duplicated checks  
  - predictable behavior across all GPIO elements  

- Prepared the I²C subsystem for modular device extensions  
  - supports nested templates such as `aui_i2c<23, MPU6050<0x68>>`  
  - consistent module design matching `aui_i2c_test_module`  

- Enhanced ISR‑driven slave handling  
  - static buffer + ready‑flag ensure deterministic packet reception  
  - avoids dynamic allocation and race conditions  
  - integrates seamlessly with the AUI event system  

## [0.3.14] 

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
  (`aui_event<aui_idble_payload>`, `aui_event`) and updated message handler to  
  `on_MESSAGE(IElement* e, aui_event<aui_idble_payload>* event)`
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
