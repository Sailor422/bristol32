#include "proto.h"
#include "crc8.h"
#include <string.h>

using namespace proto;

size_t proto::encode(uint8_t* out, size_t cap, const Header& h,
                      const void* payload, size_t payload_len) {
  const size_t hdr_len = sizeof(Header);
  if (cap < hdr_len + payload_len + 1) return 0;
  memcpy(out, &h, hdr_len);
  memcpy(out + hdr_len, payload, payload_len);
  uint8_t crc = crc8_dallas(out, hdr_len + payload_len, 0xFF);
  out[hdr_len + payload_len] = crc;
  return hdr_len + payload_len + 1;
}

bool proto::decode(const uint8_t* in, size_t len, Header& h,
                    const uint8_t** payload, size_t* payload_len) {
  const size_t hdr_len = sizeof(Header);
  if (len < hdr_len + 1) return false;
  memcpy(&h, in, hdr_len);
  uint8_t crc = in[len-1];
  if (crc8_dallas(in, len-1, 0xFF) != crc) return false;
  *payload = in + hdr_len;
  *payload_len = len - hdr_len - 1;
  return true;
}
