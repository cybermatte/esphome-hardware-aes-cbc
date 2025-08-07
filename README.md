# ESPHome Hardware AES-CBC Component

Secure your LoRa or ESP32-based sensor network with AES-CBC encryption using ESP32's built-in crypto engine.

## Features

- AES-128 CBC mode encryption/decryption
- PKCS#7 padding
- IV generation using ESP32 RNG
- Compatible with ESPHome custom components

## Usage

```yaml
external_components:
  - source: github://cybermatte/esphome-hardware-aes-cbc

custom_component:
  - lambda: |-
      auto aes = new HardwareAESCBCComponent();
      App.register_component(aes);
