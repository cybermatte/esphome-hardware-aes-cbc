#include "encrypted_sender.h"
#include "esphome/core/log.h"

namespace esphome {
namespace encrypted_sender {

static const char *const TAG = "encrypted_sender";

void EncryptedSenderComponent::setup() {
  ESP_LOGI(TAG, "EncryptedSenderComponent initialized");
}

void EncryptedSenderComponent::loop() {
  uint32_t now = millis();
  if (now - last_send_ >= update_interval_ms_) {
    last_send_ = now;
    send_encrypted_data_();
  }
}

void EncryptedSenderComponent::send_encrypted_data_() {
  if (!temperature_sensor_ || !humidity_sensor_ || !aes_ || !lora_) {
    ESP_LOGW(TAG, "Missing dependencies, skipping send");
    return;
  }

  float temp = temperature_sensor_->state;
  float hum = humidity_sensor_->state;

  // Format payload
  char payload[32];
  snprintf(payload, sizeof(payload), "T:%.2f H:%.2f", temp, hum);

  // Try casting to CBC-capable AES
  auto *cbc = dynamic_cast<hardware_aes::HardwareAESCBCComponent *>(aes_);
  if (cbc == nullptr) {
    ESP_LOGW(TAG, "AES component is not CBC-capable, skipping encryption");
    return;
  }

  // Encrypt
  std::vector<uint8_t> encrypted;
  if (!cbc->encrypt(reinterpret_cast<const uint8_t *>(payload), strlen(payload), encrypted)) {
    ESP_LOGE(TAG, "Encryption failed");
    return;
  }

  // Transmit
  if (!lora_->transmit(encrypted)) {
    ESP_LOGE(TAG, "LoRa transmission failed");
    return;
  }

  ESP_LOGI(TAG, "Sent encrypted payload: %s", payload);
}

}  // namespace encrypted_sender
}  // namespace esphome
