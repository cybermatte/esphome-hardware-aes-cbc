# Define the component name
idf_component_register(
  SRCS "hardware_aes.cpp"
       "hardware_aes.h"
  INCLUDE_DIRS "."
  REQUIRES esp_hw_support
)
