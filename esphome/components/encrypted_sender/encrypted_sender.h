#pragma once
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/lora/lora.h"
#include "esphome/components/hardware_aes/hardware_aes.h"

namespace esphome {
namespace encrypted_sender {

class EncryptedSenderComponent : public Component {
 public:
  sensor::Sensor *temperature_sensor;
  sensor::Sensor *humidity_sensor;
  hardware_aes::HardwareAESComponent *aes;
  lora::LoraComponent *lora;

  void set_temperature_sensor(sensor::Sensor *sensor) { temperature_sensor = sensor; }
  void set_humidity_sensor(sensor::Sensor *sensor) { humidity_sensor = sensor; }
  void set_aes(hardware_aes::HardwareAESComponent *comp) { aes = comp; }
  void set_lora(lora::LoraComponent *comp) { lora = comp; }

  void update() override;
};

}  // namespace encrypted_sender
}  // namespace esphome
