#ifndef MESSAGE_PROTOCOL_H
#define MESSAGE_PROTOCOL_H

#include <Arduino.h>

// LoRa Message Types
#define MSG_TYPE_ENVIRONMENTAL  0x01
#define MSG_TYPE_DETECTION      0x02
#define MSG_TYPE_ALARM          0x03
#define MSG_TYPE_HEARTBEAT      0x04
#define MSG_TYPE_CONFIG         0x20
#define MSG_TYPE_TIME_SYNC      0x21
#define MSG_TYPE_WIND           0x22
#define MSG_TYPE_WIND           0x22
#define MSG_TYPE_ACK            0xFF

// Special addresses
#define HUB_ADDRESS             0x00
#define BROADCAST_ADDRESS       0xFF

// Packet sizes
#define ENV_PACKET_SIZE         12
#define DETECTION_PACKET_SIZE   8
#define ALARM_PACKET_SIZE       6
#define HEARTBEAT_PACKET_SIZE   5
#define WIND_PACKET_SIZE        16

// Helper functions for packing/unpacking data

inline void packInt16(uint8_t* buf, int offset, int16_t value) {
  buf[offset] = (value >> 8) & 0xFF;
  buf[offset + 1] = value & 0xFF;
}

inline void packUint16(uint8_t* buf, int offset, uint16_t value) {
  buf[offset] = (value >> 8) & 0xFF;
  buf[offset + 1] = value & 0xFF;
}

inline void packUint32(uint8_t* buf, int offset, uint32_t value) {
  buf[offset] = (value >> 24) & 0xFF;
  buf[offset + 1] = (value >> 16) & 0xFF;
  buf[offset + 2] = (value >> 8) & 0xFF;
  buf[offset + 3] = value & 0xFF;
}

inline int16_t unpackInt16(uint8_t* buf, int offset) {
  return (int16_t)((buf[offset] << 8) | buf[offset + 1]);
}

inline uint16_t unpackUint16(uint8_t* buf, int offset) {
  return (uint16_t)((buf[offset] << 8) | buf[offset + 1]);
}

inline uint32_t unpackUint32(uint8_t* buf, int offset) {
  return (uint32_t)((buf[offset] << 24) | (buf[offset + 1] << 16) |
                    (buf[offset + 2] << 8) | buf[offset + 3]);
}

inline uint8_t calculateChecksum(uint8_t* buf, int len) {
  uint8_t checksum = 0;
  for (int i = 0; i < len; i++) {
    checksum ^= buf[i];
  }
  return checksum;
}

inline bool verifyChecksum(uint8_t* buf, int len) {
  if (len < 2) return false;
  uint8_t received = buf[len - 1];
  uint8_t calculated = calculateChecksum(buf, len - 1);
  return received == calculated;
}

// Environmental packet structure (12 bytes)
// Byte 0:      Node ID
// Byte 1:      Packet Type (0x01)
// Byte 2-3:    Temperature (int16, °C * 100)
// Byte 4-5:    Humidity (uint16, % * 100)
// Byte 6-7:    Pressure (uint16, hPa * 10)
// Byte 8-9:    Battery Voltage (uint16, mV)
// Byte 10:     RSSI (int8, signal strength)
// Byte 11:     Checksum

inline void packEnvironmentalPacket(uint8_t* packet, uint8_t nodeID,
                                   float temp, float humidity, float pressure,
                                   uint16_t batteryMv, int8_t rssi) {
  packet[0] = nodeID;
  packet[1] = MSG_TYPE_ENVIRONMENTAL;
  packInt16(packet, 2, (int16_t)(temp * 100));
  packUint16(packet, 4, (uint16_t)(humidity * 100));
  packUint16(packet, 6, (uint16_t)(pressure * 10));
  packUint16(packet, 8, batteryMv);
  packet[10] = rssi;
  packet[11] = calculateChecksum(packet, 11);
}

inline bool unpackEnvironmentalPacket(uint8_t* packet, uint8_t* nodeID,
                                     float* temp, float* humidity, float* pressure,
                                     uint16_t* batteryMv, int8_t* rssi) {
  if (!verifyChecksum(packet, ENV_PACKET_SIZE)) return false;

  *nodeID = packet[0];
  *temp = unpackInt16(packet, 2) / 100.0;
  *humidity = unpackUint16(packet, 4) / 100.0;
  *pressure = unpackUint16(packet, 6) / 10.0;
  *batteryMv = unpackUint16(packet, 8);
  *rssi = (int8_t)packet[10];

  return true;
}

// Detection event packet (8 bytes)
// Byte 0:      Node ID
// Byte 1:      Packet Type (0x02)
// Byte 2:      Event Type (0x01=Approach, 0x02=Entry, 0x03=Doorbell)
// Byte 3:      Confidence Level (0-100%)
// Byte 4-5:    Distance (uint16, cm)
// Byte 6:      Detection Zone (0=Near, 1=Middle, 2=Far)
// Byte 7:      Checksum

inline void packDetectionPacket(uint8_t* packet, uint8_t nodeID,
                               uint8_t eventType, uint8_t confidence,
                               uint16_t distance, uint8_t zone) {
  packet[0] = nodeID;
  packet[1] = MSG_TYPE_DETECTION;
  packet[2] = eventType;
  packet[3] = confidence;
  packUint16(packet, 4, distance);
  packet[6] = zone;
  packet[7] = calculateChecksum(packet, 7);
}

inline bool unpackDetectionPacket(uint8_t* packet, uint8_t* nodeID,
                                 uint8_t* eventType, uint8_t* confidence,
                                 uint16_t* distance, uint8_t* zone) {
  if (!verifyChecksum(packet, DETECTION_PACKET_SIZE)) return false;

  *nodeID = packet[0];
  *eventType = packet[2];
  *confidence = packet[3];
  *distance = unpackUint16(packet, 4);
  *zone = packet[6];

  return true;
}

// Alarm command packet (6 bytes)
// Byte 0:      Node ID (or 0x00 for hub broadcast)
// Byte 1:      Packet Type (0x03)
// Byte 2:      Command (0x01=Arm, 0x02=Disarm, 0x03=Trigger, 0x04=Silence)
// Byte 3:      Mode (0=Disarmed, 1=Doorbell, 2=Perimeter, 3=Full, 4=Quiet)
// Byte 4:      Target Node (0xFF = all nodes)
// Byte 5:      Checksum

inline void packAlarmPacket(uint8_t* packet, uint8_t fromNodeID,
                           uint8_t command, uint8_t mode, uint8_t targetNode) {
  packet[0] = fromNodeID;
  packet[1] = MSG_TYPE_ALARM;
  packet[2] = command;
  packet[3] = mode;
  packet[4] = targetNode;
  packet[5] = calculateChecksum(packet, 5);
}

inline bool unpackAlarmPacket(uint8_t* packet, uint8_t* fromNodeID,
                             uint8_t* command, uint8_t* mode, uint8_t* targetNode) {
  if (!verifyChecksum(packet, ALARM_PACKET_SIZE)) return false;

  *fromNodeID = packet[0];
  *command = packet[2];
  *mode = packet[3];
  *targetNode = packet[4];

  return true;
}

// Heartbeat packet (5 bytes)
// Byte 0:      Node ID
// Byte 1:      Packet Type (0x04)
// Byte 2-3:    Battery Voltage (uint16, mV)
// Byte 4:      Checksum

inline void packHeartbeatPacket(uint8_t* packet, uint8_t nodeID, uint16_t batteryMv) {
  packet[0] = nodeID;
  packet[1] = MSG_TYPE_HEARTBEAT;
  packUint16(packet, 2, batteryMv);
  packet[4] = calculateChecksum(packet, 4);
}

inline bool unpackHeartbeatPacket(uint8_t* packet, uint8_t* nodeID, uint16_t* batteryMv) {
  if (!verifyChecksum(packet, HEARTBEAT_PACKET_SIZE)) return false;

  *nodeID = packet[0];
  *batteryMv = unpackUint16(packet, 2);


// Wind data packet (16 bytes)
// Byte 0:      Node ID (0x05)
// Byte 1:      Packet Type (0x22)
// Byte 2-3:    Apparent Wind Speed (uint16, mm/s)
// Byte 4-5:    Apparent Wind Direction (uint16, degrees * 10)
// Byte 6-7:    True Wind Speed (uint16, mm/s)
// Byte 8-9:    True Wind Direction (uint16, degrees * 10)
// Byte 10-11:  Boat Speed (uint16, mm/s)
// Byte 12-13:  Boat Heading (uint16, degrees * 10)
// Byte 14:     GPS Fix Quality (0-5)
// Byte 15:     Checksum

inline void packWindPacket(uint8_t* packet, uint8_t nodeID,
                          uint16_t aws, uint16_t awd,
                          uint16_t tws, uint16_t twd,
                          uint16_t bsp, uint16_t bhd,
                          uint8_t fixQuality) {
  packet[0] = nodeID;
  packet[1] = MSG_TYPE_WIND;
  packUint16(packet, 2, aws);
  packUint16(packet, 4, awd);
  packUint16(packet, 6, tws);
  packUint16(packet, 8, twd);
  packUint16(packet, 10, bsp);
  packUint16(packet, 12, bhd);
  packet[14] = fixQuality;
  packet[15] = calculateChecksum(packet, 15);
}

inline bool unpackWindPacket(uint8_t* packet, uint8_t* nodeID,
                            uint16_t* aws, uint16_t* awd,
                            uint16_t* tws, uint16_t* twd,
                            uint16_t* bsp, uint16_t* bhd,
                            uint8_t* fixQuality) {
  if (!verifyChecksum(packet, WIND_PACKET_SIZE)) return false;

  *nodeID = packet[0];
  *aws = unpackUint16(packet, 2);
  *awd = unpackUint16(packet, 4);
  *tws = unpackUint16(packet, 6);
  *twd = unpackUint16(packet, 8);
  *bsp = unpackUint16(packet, 10);
  *bhd = unpackUint16(packet, 12);
  *fixQuality = packet[14];

  return true;
}

#endif // MESSAGE_PROTOCOL_H
