#include "hardware_aes_cbc_component.h"

namespace esphome {
namespace hardware_aes {

// Pad or truncate string to desired length
std::vector<uint8_t> HardwareAESCBCComponent::pad_or_truncate(const std::string &str, size_t length) {
  std::vector<uint8_t> bytes(str.begin(), str.end());
  if (length == 0) return bytes;  // No enforcement

  if (bytes.size() < length)
    bytes.resize(length, 0);  // pad with zeros
  else if (bytes.size() > length)
    bytes.resize(length);     // truncate

  return bytes;
}

void HardwareAESCBCComponent::set_key_from_string(const std::string &str, size_t enforce_length) {
  key_ = pad_or_truncate(str, enforce_length);
}

void HardwareAESCBCComponent::set_hmac_key_from_string(const std::string &str, size_t enforce_length) {
  hmac_key_ = pad_or_truncate(str, enforce_length);
}

}  // namespace hardware_aes
}  // namespace esphome
