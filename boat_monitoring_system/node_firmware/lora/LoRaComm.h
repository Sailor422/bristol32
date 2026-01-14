#ifndef LORA_COMM_H
#define LORA_COMM_H

#include <RH_RF95.h>
#include <RHReliableDatagram.h>
#include "../../common/MessageProtocol.h"
#include "../../common/CommonTypes.h"

/**
 * LoRa Communication Manager for Nodes
 * Handles all LoRa transmission and reception
 */
class LoRaComm {
private:
  RH_RF95 rf95;
  RHReliableDatagram* manager;
  uint8_t nodeID;
  uint8_t hubID;
  uint8_t sequenceNumber;

  // Pin definitions
  uint8_t csPin;
  uint8_t intPin;
  uint8_t rstPin;

  // Callback for received messages
  typedef void (*MessageCallback)(uint8_t* data, uint8_t len, uint8_t from);
  MessageCallback onMessageReceived;

public:
  LoRaComm(uint8_t cs, uint8_t interrupt, uint8_t reset, uint8_t nodeAddr, uint8_t hubAddr)
    : rf95(cs, interrupt), csPin(cs), intPin(interrupt), rstPin(reset),
      nodeID(nodeAddr), hubID(hubAddr), sequenceNumber(0),
      onMessageReceived(nullptr) {
    manager = new RHReliableDatagram(rf95, nodeAddr);
  }

  ~LoRaComm() {
    delete manager;
  }

  bool begin(float frequency = 915.0) {
    // Reset LoRa module
    if (rstPin != 255) {
      pinMode(rstPin, OUTPUT);
      digitalWrite(rstPin, LOW);
      delay(10);
      digitalWrite(rstPin, HIGH);
      delay(10);
    }

    if (!manager->init()) {
      Serial.println("LoRa init failed");
      return false;
    }

    // Configure LoRa parameters (optimized for boat environment)
    if (!rf95.setFrequency(frequency)) {
      Serial.println("LoRa setFrequency failed");
      return false;
    }

    rf95.setTxPower(17, false);         // 17 dBm (50mW)
    rf95.setSpreadingFactor(8);         // SF8 (balanced speed/range)
    rf95.setSignalBandwidth(125000);    // 125 kHz
    rf95.setCodingRate4(5);             // 4/5 coding rate
    rf95.setPayloadCRC(true);           // Enable CRC

    // Set retries and timeout for reliable datagram
    manager->setRetries(3);
    manager->setTimeout(500);           // 500ms timeout

    Serial.print("LoRa initialized on ");
    Serial.print(frequency);
    Serial.print(" MHz, Node ID: 0x");
    Serial.println(nodeID, HEX);

    return true;
  }

  // Send environmental data
  bool sendEnvironmentalData(const EnvData& data) {
    uint8_t packet[ENV_PACKET_SIZE];
    packEnvironmentalPacket(packet, data.nodeID, data.temperature,
                           data.humidity, data.pressure,
                           data.batteryVoltage, data.rssi);

    bool success = manager->sendtoWait(packet, ENV_PACKET_SIZE, hubID);
    sequenceNumber++;

    if (success) {
      Serial.println("Env data sent successfully");
    } else {
      Serial.println("Env data send failed");
    }

    return success;
  }

  // Send detection event
  bool sendDetectionEvent(const DetectionEvent& event) {
    uint8_t packet[DETECTION_PACKET_SIZE];
    packDetectionPacket(packet, nodeID, event.eventType,
                       event.confidence, event.distance, event.zone);

    bool success = manager->sendtoWait(packet, DETECTION_PACKET_SIZE, hubID);

    if (success) {
      Serial.println("Detection event sent");
    } else {
      Serial.println("Detection send failed");
    }

    return success;
  }

  // Send alarm trigger
  bool sendAlarmTrigger(AlarmMode mode) {
    uint8_t packet[ALARM_PACKET_SIZE];
    packAlarmPacket(packet, nodeID, 0x03, (uint8_t)mode, BROADCAST_ADDRESS);

    bool success = manager->sendtoWait(packet, ALARM_PACKET_SIZE, hubID);

    if (success) {
      Serial.println("Alarm trigger sent");
    } else {
      Serial.println("Alarm send failed");
    }

    return success;
  }

  // Send heartbeat
  bool sendHeartbeat(uint16_t batteryMv) {
    uint8_t packet[HEARTBEAT_PACKET_SIZE];
    packHeartbeatPacket(packet, nodeID, batteryMv);

    bool success = manager->sendtoWait(packet, HEARTBEAT_PACKET_SIZE, hubID);

    if (!success) {
      Serial.println("Heartbeat send failed");
    }

    return success;
  }

  // Process incoming messages
  void processIncoming() {
    if (manager->available()) {
      uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);
      uint8_t from;

      if (manager->recvfromAck(buf, &len, &from)) {
        Serial.print("Received message from 0x");
        Serial.print(from, HEX);
        Serial.print(", RSSI: ");
        Serial.println(rf95.lastRssi());

        handleIncomingMessage(buf, len, from);
      }
    }
  }

  // Set callback for received messages
  void setMessageCallback(MessageCallback callback) {
    onMessageReceived = callback;
  }

  // Get last RSSI
  int16_t getLastRSSI() {
    return rf95.lastRssi();
  }

  // Get last SNR
  int8_t getLastSNR() {
    return rf95.lastSNR();
  }

private:
  void handleIncomingMessage(uint8_t* buf, uint8_t len, uint8_t from) {
    // Only accept messages from hub
    if (from != hubID && from != BROADCAST_ADDRESS) {
      Serial.println("Ignoring message from non-hub source");
      return;
    }

    if (len < 2) {
      Serial.println("Message too short");
      return;
    }

    uint8_t packetType = buf[1];

    switch (packetType) {
      case MSG_TYPE_ALARM:
        handleAlarmCommand(buf, len);
        break;

      case MSG_TYPE_CONFIG:
        handleConfigUpdate(buf, len);
        break;

      case MSG_TYPE_TIME_SYNC:
        handleTimeSync(buf, len);
        break;

      default:
        Serial.print("Unknown packet type: 0x");
        Serial.println(packetType, HEX);
        break;
    }

    // Call user callback if set
    if (onMessageReceived) {
      onMessageReceived(buf, len, from);
    }
  }

  void handleAlarmCommand(uint8_t* buf, uint8_t len) {
    if (len != ALARM_PACKET_SIZE) {
      Serial.println("Invalid alarm packet size");
      return;
    }

    uint8_t fromNodeID, command, mode, targetNode;
    if (!unpackAlarmPacket(buf, &fromNodeID, &command, &mode, &targetNode)) {
      Serial.println("Alarm packet checksum failed");
      return;
    }

    // Check if this message is for us
    if (targetNode != nodeID && targetNode != BROADCAST_ADDRESS) {
      return; // Not for us
    }

    Serial.print("Alarm command: ");
    Serial.print(command);
    Serial.print(", Mode: ");
    Serial.println(mode);

    // Process alarm command (will be handled by main firmware)
    // Commands: 0x01=Arm, 0x02=Disarm, 0x03=Trigger, 0x04=Silence
  }

  void handleConfigUpdate(uint8_t* buf, uint8_t len) {
    Serial.println("Config update received");
    // Implement configuration update logic
    // This would update node settings like sensitivity, zones, etc.
  }

  void handleTimeSync(uint8_t* buf, uint8_t len) {
    // Time sync packet contains current timestamp
    // Can be used to synchronize RTC or internal clock
    Serial.println("Time sync received");

    if (len >= 6) {
      uint32_t timestamp = unpackUint32(buf, 2);
      Serial.print("Timestamp: ");
      Serial.println(timestamp);
      // Update RTC or system time here
    }
  }
};

#endif // LORA_COMM_H
