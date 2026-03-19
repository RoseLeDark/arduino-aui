# AUI Release Roadmap  
## Version 0.3.99 — RC for 0.4.0
### Planned
- [ ] `aui_hardware_serial`  
  Custom hardware‑serial driver independent of Arduino `Serial` and without using `String`  
  - fully deterministic  
  - zero dynamic allocation  
  - integrated with the AUI message system (`MSG_UART_WRITE`, `MSG_UART_READ`)  

- [ ] **`aui_software_serial`**  
  Software‑based serial driver implemented as an AUI element  
  - tick‑driven timing  
  - deterministic bit‑level state machine  
  - optional ISR‑assisted mode  

- [ ] **Test suite for AUI**  
  - write tests for all GPIO elements  
  - test I²C master/slave functionality  
  - test timer system, soft‑locks, enable/disable semantics  
  - run full validation on real hardware (Arduino Uno recommended)

## Version 0.4.0 — I²C & SPI Core
### Planned / Progress
- [x] `aui_i2c` — generic I²C abstraction layer  
- [x] I²C slave support (`aui_i2c_slave`)  
- [x] I²C test module (`aui_i2c_test_module`)  
- [x] Unified enable/disable handling for all GPIO elements  
- [x] Deterministic timer system with weak callbacks  
- [x] Soft‑lock system (`aui_soft_lock`, `aui_lock_guard`)  
- [ ] `aui_spi` — generic SPI abstraction layer  
- [ ] Unified message-based configuration for bus elements  
- [ ] Deterministic bus transaction model (non-blocking, event-driven)

## Version 0.4.5 — Basic Sensor & Display Modules
### Planned
- [ ] `aui_oled_i2c` — OLED display driver (I²C)  
- [ ] `aui_dht` — DHT11/DHT22 temperature & humidity sensor  
- [ ] `aui_barometer_i2c` — barometric pressure sensor (BMP/BME series)  
- [ ] Standardized sensor event structures (`aui_sensor_event`)  
- [x] `aui_devices.h` — alias layer for common sensors/actuators  

## Version 0.5.0 — Audio & Streaming
### Planned
- [ ] `aui_i2s` — I²S audio interface  
- [ ] Deterministic audio streaming model  
- [ ] Basic audio sink/source elements  

## Version 1.0 — Full Hardware Ecosystem
### Planned
- [ ] CAN bus support (`aui_can`)  
- [ ] Interrupt-capable digital input (`aui_interrupt_pin`)  
- [ ] `aui_xyz_i2c` — 3‑axis accelerometer/gyroscope  
- [ ] `aui_temp_ao` — analog temperature sensor  
- [ ] `aui_9achsen_i2c` — 9‑axis IMU (accel/gyro/magnetometer)  
- [ ] `aui_wifi_esp32` — WiFi via ESP32 (UART-based AT interface)  
- [ ] `aui_bt_esp32` — Bluetooth via ESP32  
- [ ] `aui_usb_host` — USB host abstraction  
- [ ] Unified hardware capability registry  
- [ ] Stable API freeze for 1.0  

## 🧭 Notes
- All modules follow the AUI design philosophy:  
  - deterministic event-driven architecture  
  - zero dynamic allocation  
  - compile-time configuration  
  - reproducible state machines  
- Version numbers reflect feature completeness, not calendar dates.
