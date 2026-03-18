# AUI Release Roadmap
This document outlines the planned feature roadmap for upcoming AUI versions.  
Dates are not fixed; versions represent development milestones.

## Version 0.4.0 — I²C & SPI Core
### Planned
- `aui_i2c` — generic I²C abstraction layer
- `aui_spi` — generic SPI abstraction layer
- Unified message-based configuration for bus elements
- Deterministic bus transaction model (non-blocking, event-driven)

## Version 0.4.5 — Basic Sensor & Display Modules
### Planned
- `aui_oled_i2c` — OLED display driver (I²C)
- `aui_dht` — DHT11/DHT22 temperature & humidity sensor
- `aui_barometer_i2c` — barometric pressure sensor (BMP/BME series)
- Standardized sensor event structures (`aui_sensor_event`)

## Version 0.5.0 — Audio & Streaming
### Planned
- `aui_i2s` — I²S audio interface
- Deterministic audio streaming model
- Basic audio sink/source elements

## Version 1.0 — Full Hardware Ecosystem
### Planned
- CAN bus support (`aui_can`)
- Interrupt-capable digital input (`aui_interrupt_pin`)
- `aui_xyz_i2c` — 3‑axis accelerometer/gyroscope
- `aui_temp_ao` — analog temperature sensor
- `aui_9achsen_i2c` — 9‑axis IMU (accel/gyro/magnetometer)
- `aui_wifi_esp32` — WiFi via ESP32 (UART-based AT interface)
- `aui_bt_esp32` — Bluetooth via ESP32
- `aui_usb_host` — USB host abstraction
- Unified hardware capability registry
- Stable API freeze for 1.0

## 🧭 Notes
- All modules follow the AUI design philosophy:
  - deterministic event-driven architecture  
  - zero dynamic allocation  
  - compile-time configuration  
  - reproducible state machines  
- Version numbers reflect feature completeness, not calendar dates.

