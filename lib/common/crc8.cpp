#include "crc8.h"
uint8_t crc8_dallas(const uint8_t* data, size_t len, uint8_t init) {
  uint8_t crc = init; // poly 0x31 (x^8 + x^5 + x^4 + 1)
  for (size_t i = 0; i < len; ++i) {
    crc ^= data[i];
    for (uint8_t b = 0; b < 8; ++b)
      crc = (crc & 0x80) ? (crc << 1) ^ 0x31 : (crc << 1);
  }
  return crc;
}
