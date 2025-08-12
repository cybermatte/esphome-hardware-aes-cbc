#include "encrypted_sender.h"
#include "esphome/core/log.h"

namespace esphome {
namespace encrypted_sender {

static const char *TAG = "encrypted_sender";

void EncryptedSenderComponent::update() {
  static uint8_t counter = 0;

  std::vector<uint8_t> payload = {
    0x01,
    counter++,
    static_cast<uint8_t>(temperature_sensor->state * 10),
    static_cast<uint8_t>(humidity_sensor->state)
  };

  std::vector<uint8_t> encrypted;
  uint8_t iv[16];
  aes->encrypt_cbc_with_hmac(payload, encrypted, iv);

  std::vector<uint8_t> packet(iv, iv + 16);
  packet.insert(packet.end(), encrypted.begin(), encrypted.end());

  lora->send_packet(packet);
  ESP_LOGI(TAG, "Encrypted SHT40 sensor packet sent");
}

}  // namespace encrypted_sender
}  // namespace esphome
