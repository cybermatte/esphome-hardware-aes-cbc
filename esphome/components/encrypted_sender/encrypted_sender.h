#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/sx126x/sx126x.h"
#include "esphome/components/hardware_aes/hardware_aes.h"

namespace esphome {
namespace encrypted_sender {

class EncryptedSenderComponent : public Component {
 public:
  void set_temperature_sensor(sensor::Sensor *sensor) { temperature_sensor_ = sensor; }
  void set_humidity_sensor(sensor::Sensor *sensor) { humidity_sensor_ = sensor; }
  void set_aes(hardware_aes::HardwareAESComponent *aes);

  void set_lora(sx126x::SX126x *lora) { lora_ = lora; }
  void set_update_interval(uint32_t ms) { update_interval_ms_ = ms; }

  void setup() override;
  void loop() override;

 protected:
  sensor::Sensor *temperature_sensor_{nullptr};
  sensor::Sensor *humidity_sensor_{nullptr};
  hardware_aes::HardwareAESComponent *aes_{nullptr};

  sx126x::SX126x *lora_{nullptr};

  uint32_t update_interval_ms_{60000};  // default to 60s
  uint32_t last_send_{0};

  void send_encrypted_data_();
};

}  // namespace encrypted_sender
}  // namespace esphome

