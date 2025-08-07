#pragma once
#include "esphome.h"
#include "esp_aes.h"
#include "esp_random.h"

class HardwareAESCBCComponent : public esphome::Component {
 public:
  void setup() override {
    esp_aes_init(&ctx_);
  }

  void encrypt_cbc(const std::vector<uint8_t> &input, std::vector<uint8_t> &output, uint8_t iv[16]) {
    size_t len = input.size();
    size_t padded_len = ((len / 16) + 1) * 16;
    std::vector<uint8_t> padded(padded_len);

    memcpy(padded.data(), input.data(), len);
    uint8_t pad = padded_len - len;
    for (size_t i = len; i < padded_len; ++i) padded[i] = pad;

    esp_fill_random(iv, 16);
    output.resize(padded_len);

    esp_aes_setkey_enc(&ctx_, key_, 128);
    esp_aes_crypt_cbc(&ctx_, ESP_AES_ENCRYPT, padded_len, iv, padded.data(), output.data());
  }

  bool decrypt_cbc(const std::vector<uint8_t> &input, std::vector<uint8_t> &output, const uint8_t iv[16]) {
    size_t len = input.size();
    output.resize(len);

    esp_aes_setkey_dec(&ctx_, key_, 128);
    esp_aes_crypt_cbc(&ctx_, ESP_AES_DECRYPT, len, const_cast<uint8_t *>(iv), input.data(), output.data());

    uint8_t pad = output[len - 1];
    if (pad == 0 || pad > 16) return false;
    output.resize(len - pad);
    return true;
  }

 protected:
  esp_aes_context ctx_;
  const uint8_t key_[16] = {
    0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
    0x10, 0x32, 0x54, 0x76, 0x98, 0xBA, 0xDC, 0xFE
  };
};

uint16_t crc16(const uint8_t *data, size_t len) {
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
