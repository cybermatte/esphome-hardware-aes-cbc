idf_component_register(
  SRCS "encrypted_sender.cpp"
       "encrypted_sender.h"
  INCLUDE_DIRS "."
  REQUIRES hardware_aes esp_hw_support mbedtls
)
