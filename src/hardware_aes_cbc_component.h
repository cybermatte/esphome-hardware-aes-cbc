#pragma once

#include "esphome/core/component.h"
#include "esp_aes.h"
#include "mbedtls/md.h"
#include <vector>
#include <string>

namespace esphome {
namespace hardware_aes {

class HardwareAESCBCComponent : public esphome::Component {
 public:
  void setup() override;

  // 🔐 AES-CBC encryption with HMAC-SHA256
  void encrypt_cbc_with_hmac(const std::vector<uint8_t> &input, std::vector<uint8_t> &output, uint8_t iv[16]);

  // 🔓 AES-CBC decryption with HMAC-SHA256 verification
  bool decrypt_cbc_with_hmac(const std::vector<uint8_t> &input, std::vector<uint8_t> &output, const uint8_t iv[16]);

  // 🧮 Optional: CRC16
  uint16_t crc16(const uint8_t *data, size_t len);

  // 🔑 Set AES key from string with optional enforced length
  void set_key_from_string(const std::string &str, size_t enforce_length = 0);

  // 🔐 Set HMAC key from string with optional enforced length
  void set_hmac_key_from_string(const std::string &str, size_t enforce_length = 0);

 protected:
  esp_aes_context ctx_;

  std::vector<uint8_t> key_;
  std::vector<uint8_t> hmac_key_;

  std::vector<uint8_t> pad_or_truncate(const std::string &str, size_t length);

  bool compute_hmac_sha256(const std::vector<uint8_t> &data, uint8_t out[32]);
};

}  // namespace hardware_aes
}  // namespace esphome
