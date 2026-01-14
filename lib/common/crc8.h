#pragma once
#include <stdint.h>
uint8_t crc8_dallas(const uint8_t* data, size_t len, uint8_t init = 0xFF);
