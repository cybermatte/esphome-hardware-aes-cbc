#pragma once
#include "esphome.h"
#include "esp_aes.h"
#include "esp_random.h"
#include "mbedtls/md.h"

class HardwareAESCBCComponent : public esphome::Component {
 public:
  void setup() override {
    esp_aes_init(&ctx_);
  }

  // 🔐 AES-CBC encryption with HMAC-SHA256
  void encrypt_cbc_with_hmac(const std::vector<uint8_t> &input, std::vector<uint8_t> &output, uint8_t iv[16]) {
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

    esp_aes_setkey_enc(&ctx_, key_, 128);
    esp_aes_crypt_cbc(&ctx_, ESP_AES_ENCRYPT, padded_len, iv, padded.data(), output.data());
  }

  // 🔓 AES-CBC decryption with HMAC-SHA256 verification
  bool decrypt_cbc_with_hmac(const std::vector<uint8_t> &input, std::vector<uint8_t> &output, const uint8_t iv[16]) {
    size_t len = input.size();
    std::vector<uint8_t> decrypted(len);

    esp_aes_setkey_dec(&ctx_, key_, 128);
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

  // 🧮 Optional: CRC16 (if you want to combine with HMAC)
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

 protected:
  esp_aes_context ctx_;

  // 🔑 AES key (128-bit)
  const uint8_t key_[16] = {
    0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
    0x10, 0x32, 0x54, 0x76, 0x98, 0xBA, 0xDC, 0xFE
  };

  // 🔐 HMAC key (256-bit)
  const uint8_t hmac_key_[32] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE, 0xBA, 0xBE,
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
    0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0
  };

  // 🔧 HMAC-SHA256 helper
  bool compute_hmac_sha256(const std::vector<uint8_t> &data, uint8_t out[32]) {
    const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    if (!md_info) return false;

    int ret = mbedtls_md_hmac(md_info, hmac_key_, sizeof(hmac_key_), data.data(), data.size(), out);
    return ret == 0;
  }
};
