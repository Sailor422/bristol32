#pragma once
#include <Arduino.h>
#include <stdint.h>

namespace proto {
enum Type : uint8_t {
  ENV = 0,
  MOTION = 1,
  PING = 2,
  POWER = 3,
  ACK = 4,
  WIND = 5
};
struct __attribute__((packed)) Header {
  uint8_t ver{0x01};
  uint8_t type{0};
  uint16_t node_id{0};
  uint32_t seq{0};
  uint8_t flags{0}; // bit0=req_ack, bit1=critical
};
struct EnvPayload {
  float t_c;
  float h_rh;
  float p_hpa;
};
struct MotionPayload {
  uint32_t age_ms;
};
struct PowerPayload {
  float v;
  float i;
};
struct WindPayload {
  uint16_t aws_mms;
  uint16_t awd_deg10;
  uint16_t tws_mms;
  uint16_t twd_deg10;
  uint16_t bsp_mms;
  uint16_t bhd_deg10;
  uint8_t fix_quality;
};

size_t encode(uint8_t *out, size_t cap, const Header &h, const void *payload,
              size_t payload_len);
bool decode(const uint8_t *in, size_t len, Header &h, const uint8_t **payload,
            size_t *payload_len);
} // namespace proto
