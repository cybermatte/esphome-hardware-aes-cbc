#include "hardware_aes_cbc_component.h"

namespace esphome {
namespace hardware_aes {

void HardwareAESCBCComponent::setup() {
  esp_aes_init(&ctx_);
}

std::vector<uint8_t> HardwareAESCBCComponent::pad_or_truncate(const std::string &str, size_t length) {
  std::vector<uint8_t> bytes(str.begin(), str.end());
  if (length == 0) return bytes;

  if (bytes.size() < length)
    bytes.resize(length, 0);
  else if (bytes.size() > length)
    bytes.resize(length);

  return bytes;
}

void HardwareAESCBCComponent::set_key_from_string(const std::string &str, size_t enforce_length) {
  key_ = pad_or_truncate(str, enforce_length);
}

void HardwareAESCBCComponent::set_hmac_key_from_string(const std::string &str, size_t enforce_length) {
  hmac_key_ = pad_or_truncate(str, enforce_length);
}

bool HardwareAESCBCComponent::compute_hmac_sha256(const std::vector<uint8_t> &data, uint8_t out[32]) {
  const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
  if (!md_info) return false;

  int ret = mbedtls_md_hmac(md_info, hmac_key_.data(), hmac_key_.size(), data.data(), data.size(), out);
  return ret == 0;
}

void HardwareAESCBCComponent::encrypt_cbc_with_hmac(const std::vector<uint8_t> &input, std::vector<uint8_t> &output, uint8_t iv[16]) {
  std::vector<uint8_t> data_with_hmac = input;
  uint8_t hmac[32];
  if (!compute_hmac_sha256(input, hmac)) return;

  data_with_hmac.insert(data_with_hmac.end(), hmac, hmac + 32);

  size_t len = data_with_hmac.size();
  size_t padded_len = ((len / 16) + 1) * 16;
  std::vector<uint8_t> padded(padded_len);

  memcpy(padded.data(), data_with_hmac.data(), len);
  uint8_t pad = padded_len - len;
  for (size_t i = len; i < padded_len; ++i) padded[i] = pad;

  esp_fill_random(iv, 16);
  output.resize(padded_len);

  esp_aes_setkey_enc(&ctx_, key_.data(), key_.size() * 8);
  esp_aes_crypt_cbc(&ctx_, ESP_AES_ENCRYPT, padded_len, iv, padded.data(), output.data());
}

bool HardwareAESCBCComponent::decrypt_cbc_with_hmac(const std::vector<uint8_t> &input, std::vector<uint8_t> &output, const uint8_t iv[16]) {
  size_t len = input.size();
  std::vector<uint8_t> decrypted(len);

  esp_aes_setkey_dec(&ctx_, key_.data(), key_.size() * 8);
  esp_aes_crypt_cbc(&ctx_, ESP_AES_DECRYPT, len, const_cast<uint8_t *>(iv), input.data(), decrypted.data());

  uint8_t pad = decrypted[len - 1];
  if (pad == 0 || pad > 16) return false;
  decrypted.resize(len - pad);

  if (decrypted.size() < 33) return false;
  size_t data_len = decrypted.size() - 32;
  uint8_t expected_hmac[32];
  if (!compute_hmac_sha256(std::vector<uint8_t>(decrypted.begin(), decrypted.begin() + data_len), expected_hmac))
    return false;

  if (memcmp(expected_hmac, decrypted.data() + data_len, 32) != 0) return false;

  output.assign(decrypted.begin(), decrypted.begin() + data_len);
  return true;
}

uint16_t HardwareAESCBCComponent::crc16(const uint8_t *data, size_t len) {
  uint16_t crc = 0xFFFF;
  for (size_t i = 0; i < len; ++i) {
    crc ^= data[i];
    for (int j = 0; j < 8; ++j) {
      if (crc & 1)
        crc = (crc >> 1) ^ 0xA001;
      else
        crc >>= 1;
    }
  }
  return crc;
}

}  // namespace hardware_aes
}  // namespace esphome
