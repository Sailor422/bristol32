/*
 * Boat Monitoring System - Hub Firmware
 * For Bristol 32 "Liberty"
 *
 * Central hub that receives data from all distributed nodes
 * Coordinates alarm system and provides unified display
 *
 * Hardware:
 *   - ESP32-S3 (recommended) or Raspberry Pi Zero 2 W
 *   - RFM95W LoRa module (915 MHz for US)
 *   - 3.5" or 4.3" TFT touchscreen display
 *   - microSD card for data logging
 *   - DS3231 RTC for accurate timestamps
 *   - Buzzer/speaker for alarms
 *
 * Author: Auto-generated from design document
 * Version: 1.0.0
 */

#include <Arduino.h>
#include <Wire.h>
#include "lora/LoRaHub.h"
#include "display/HubDisplay.h"
#include "storage/DataLogger.h"
#include "alarm/AlarmManager.h"
#include "../common/CommonTypes.h"
#include "../common/MessageProtocol.h"

// ============================================================================
// PIN DEFINITIONS
// ============================================================================

// I2C (for RTC, optional sensors)
#define I2C_SDA         8
#define I2C_SCL         7

// LoRa (RFM95W)
#define LORA_CS         10
#define LORA_INT        9
#define LORA_RST        255  // Not connected

// Display (ILI9488 SPI)
#define TFT_CS          34
#define TFT_DC          33
#define TFT_RST         21

// SD Card (SPI)
#define SD_CS           38

// Buzzer/Speaker
#define BUZZER_PIN      15

// Buttons/Controls
#define BTN_ARM         3
#define BTN_DISARM      4
#define BTN_SILENCE     5

// ============================================================================
// CONFIGURATION
// ============================================================================

#define MAX_NODES 10
#define LORA_FREQUENCY 915.0  // MHz (915 for US, 868 for EU, 433 for Asia)
#define NODE_TIMEOUT 600000   // 10 minutes - mark node offline if no contact

// ============================================================================
// GLOBAL OBJECTS
// ============================================================================

LoRaHub lora(LORA_CS, LORA_INT, LORA_RST);
HubDisplay display(TFT_CS, TFT_DC, TFT_RST);
DataLogger logger(SD_CS);
AlarmManager alarmMgr;

// Node registry
NodeInfo nodes[MAX_NODES];
int nodeCount = 0;

// ============================================================================
// TIMING VARIABLES
// ============================================================================

unsigned long lastDisplayUpdate = 0;
unsigned long lastNodeHealthCheck = 0;
unsigned long lastTimeSync = 0;
unsigned long lastButtonCheck = 0;

// ============================================================================
// FUNCTION DECLARATIONS
// ============================================================================

void setupPins();
void registerNode(uint8_t id, const String& name);
NodeInfo* findNode(uint8_t id);
void checkNodeHealth();
void handleButtons();
void soundAlarm();
void updateDisplay();
void onEnvDataReceived(uint8_t nodeID, float temp, float humidity, float pressure, uint16_t batteryMv, int8_t rssi);
void onDetectionReceived(uint8_t nodeID, uint8_t eventType, uint8_t confidence, uint16_t distance, uint8_t zone);
void onAlarmReceived(uint8_t nodeID, uint8_t command, uint8_t mode);

// ============================================================================
// SETUP
// ============================================================================

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n========================================");
  Serial.println("  Boat Monitoring System - HUB");
  Serial.println("  Bristol 32 'Liberty'");
  Serial.println("========================================\n");

  setupPins();

  // Initialize I2C
  Wire.begin(I2C_SDA, I2C_SCL);

  // Initialize LoRa Hub
  Serial.println("Initializing LoRa Hub...");
  if (!lora.begin(LORA_FREQUENCY)) {
    Serial.println("ERROR: LoRa init failed!");
    while (1) delay(1000);
  }

  // Set LoRa callbacks
  lora.setEnvDataCallback(onEnvDataReceived);
  lora.setDetectionCallback(onDetectionReceived);
  lora.setAlarmCallback(onAlarmReceived);

  // Initialize display
  Serial.println("Initializing display...");
  if (!display.begin()) {
    Serial.println("WARNING: Display init failed");
  } else {
    display.showWelcome();
  }

  // Initialize data logger
  Serial.println("Initializing SD card...");
  if (!logger.begin()) {
    Serial.println("WARNING: SD card not available - logging disabled");
  }

  // Initialize alarm manager
  alarmMgr.begin();

  // Register known nodes
  registerNode(0x01, "Companionway");
  registerNode(0x02, "Foredeck");
  registerNode(0x03, "Cockpit");
  registerNode(0x04, "Cabin");
  registerNode(0x05, "Engine");

  logger.logEvent("Hub started");

  Serial.println("\nHub initialized successfully!");
  Serial.println("Listening for nodes...\n");
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
  unsigned long now = millis();

  // Process incoming LoRa messages
  lora.receive();

  // Handle button presses
  if (now - lastButtonCheck >= 100) {
    handleButtons();
    lastButtonCheck = now;
  }

  // Update display
  if (now - lastDisplayUpdate >= 1000) {
    updateDisplay();
    lastDisplayUpdate = now;
  }

  // Check node health
  if (now - lastNodeHealthCheck >= 30000) { // Every 30 seconds
    checkNodeHealth();
    lastNodeHealthCheck = now;
  }

  // Broadcast time sync every 10 minutes
  if (now - lastTimeSync >= 600000) {
    uint32_t timestamp = now / 1000; // Simple timestamp (seconds since boot)
    lora.broadcastTimeSync(timestamp);
    lastTimeSync = now;
  }

  // Process alarm manager
  alarmMgr.process();

  // Sound alarm if triggered
  if (alarmMgr.isTriggered()) {
    soundAlarm();
  }

  delay(10);
}

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

void setupPins() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  pinMode(BTN_ARM, INPUT_PULLUP);
  pinMode(BTN_DISARM, INPUT_PULLUP);
  pinMode(BTN_SILENCE, INPUT_PULLUP);
}

void registerNode(uint8_t id, const String& name) {
  if (nodeCount >= MAX_NODES) {
    Serial.println("ERROR: Max nodes reached");
    return;
  }

  nodes[nodeCount].id = id;
  nodes[nodeCount].name = name;
  nodes[nodeCount].online = false;
  nodes[nodeCount].lastContact = 0;
  nodes[nodeCount].temperature = 0;
  nodes[nodeCount].humidity = 0;
  nodes[nodeCount].pressure = 0;
  nodes[nodeCount].batteryVoltage = 0;
  nodes[nodeCount].rssi = 0;
  nodes[nodeCount].state = STATE_INIT;

  Serial.print("Registered node: 0x");
  Serial.print(id, HEX);
  Serial.print(" - ");
  Serial.println(name);

  nodeCount++;
}

NodeInfo* findNode(uint8_t id) {
  for (int i = 0; i < nodeCount; i++) {
    if (nodes[i].id == id) {
      return &nodes[i];
    }
  }
  return nullptr;
}

void checkNodeHealth() {
  unsigned long now = millis();

  for (int i = 0; i < nodeCount; i++) {
    if (nodes[i].online && (now - nodes[i].lastContact > NODE_TIMEOUT)) {
      Serial.print("Node offline: ");
      Serial.println(nodes[i].name);

      nodes[i].online = false;

      logger.logEvent(("Node offline: " + nodes[i].name).c_str());
    }
  }
}

void handleButtons() {
  static bool btnArmPressed = false;
  static bool btnDisarmPressed = false;
  static bool btnSilencePressed = false;

  // ARM button - cycle through armed modes
  if (digitalRead(BTN_ARM) == LOW && !btnArmPressed) {
    btnArmPressed = true;

    AlarmMode currentMode = alarmMgr.getMode();
    AlarmMode newMode;

    switch (currentMode) {
      case MODE_DISARMED:
      case MODE_DOORBELL:
        newMode = MODE_PERIMETER;
        break;
      case MODE_PERIMETER:
        newMode = MODE_FULL;
        break;
      default:
        newMode = MODE_PERIMETER;
        break;
    }

    alarmMgr.armSystem(newMode);
    lora.sendAlarmCommand(BROADCAST_ADDRESS, 0x01, (uint8_t)newMode); // Arm all nodes
    logger.logEvent(("Armed: " + String(alarmModeToString(newMode))).c_str());

    tone(BUZZER_PIN, 1000, 100); // Acknowledge

  } else if (digitalRead(BTN_ARM) == HIGH) {
    btnArmPressed = false;
  }

  // DISARM button
  if (digitalRead(BTN_DISARM) == LOW && !btnDisarmPressed) {
    btnDisarmPressed = true;

    alarmMgr.disarm();
    lora.sendAlarmCommand(BROADCAST_ADDRESS, 0x02, MODE_DISARMED); // Disarm all nodes
    logger.logEvent("Disarmed");

    tone(BUZZER_PIN, 800, 100);

  } else if (digitalRead(BTN_DISARM) == HIGH) {
    btnDisarmPressed = false;
  }

  // SILENCE button
  if (digitalRead(BTN_SILENCE) == LOW && !btnSilencePressed) {
    btnSilencePressed = true;

    if (alarmMgr.isTriggered()) {
      alarmMgr.disarm();
      lora.sendAlarmCommand(BROADCAST_ADDRESS, 0x04, MODE_DISARMED); // Silence all
      logger.logEvent("Alarm silenced");
      noTone(BUZZER_PIN);
    }

  } else if (digitalRead(BTN_SILENCE) == HIGH) {
    btnSilencePressed = false;
  }
}

void soundAlarm() {
  // Continuous loud alarm
  static unsigned long lastToggle = 0;
  static bool toneOn = false;

  if (millis() - lastToggle >= 500) {
    if (toneOn) {
      noTone(BUZZER_PIN);
    } else {
      tone(BUZZER_PIN, 2500); // 2.5 kHz alarm
    }
    toneOn = !toneOn;
    lastToggle = millis();
  }
}

void updateDisplay() {
  display.drawMainScreen(nodes, nodeCount, alarmMgr.getMode(), alarmMgr.isTriggered());

  if (alarmMgr.isTriggered()) {
    NodeInfo* node = findNode(alarmMgr.getTriggeringNode());
    const char* nodeName = node ? node->name.c_str() : "Unknown";
    display.showAlarm(nodeName);
  }
}

// ============================================================================
// LORA CALLBACKS
// ============================================================================

void onEnvDataReceived(uint8_t nodeID, float temp, float humidity, float pressure,
                      uint16_t batteryMv, int8_t rssi) {
  NodeInfo* node = findNode(nodeID);
  if (!node) {
    Serial.print("Unknown node: 0x");
    Serial.println(nodeID, HEX);
    return;
  }

  // Update node data
  node->online = true;
  node->lastContact = millis();
  node->temperature = temp;
  node->humidity = humidity;
  node->pressure = pressure;
  node->batteryVoltage = batteryMv;
  node->rssi = rssi;

  // Log to SD card
  logger.logEnvironmental(nodeID, temp, humidity, pressure, batteryMv, rssi);

  // Check for low battery alert
  if (batteryMv < 3300 && batteryMv > 1000) {
    Serial.print("LOW BATTERY: ");
    Serial.print(node->name);
    Serial.print(" - ");
    Serial.print(batteryMv);
    Serial.println("mV");
  }
}

void onDetectionReceived(uint8_t nodeID, uint8_t eventType, uint8_t confidence,
                        uint16_t distance, uint8_t zone) {
  NodeInfo* node = findNode(nodeID);
  if (!node) return;

  Serial.print("Detection at ");
  Serial.print(node->name);
  Serial.print(": Type=");
  Serial.print(eventType);
  Serial.print(", Conf=");
  Serial.print(confidence);
  Serial.println("%");

  // Log detection
  logger.logDetection(nodeID, eventType, confidence, distance, zone);

  // If system is armed and this is a high-confidence detection, trigger alarm
  AlarmMode mode = alarmMgr.getMode();
  if ((mode == MODE_PERIMETER || mode == MODE_FULL) && confidence >= 80) {
    alarmMgr.triggerAlarm(nodeID);
    logger.logAlarm(nodeID, "Detection triggered alarm");
  }
}

void onAlarmReceived(uint8_t nodeID, uint8_t command, uint8_t mode) {
  NodeInfo* node = findNode(nodeID);
  const char* nodeName = node ? node->name.c_str() : "Unknown";

  switch (command) {
    case 0x03: // Alarm triggered
      Serial.print("ALARM from ");
      Serial.println(nodeName);
      alarmMgr.triggerAlarm(nodeID);
      logger.logAlarm(nodeID, "Alarm triggered");
      break;

    default:
      Serial.print("Alarm command 0x");
      Serial.print(command, HEX);
      Serial.print(" from ");
      Serial.println(nodeName);
      break;
  }
}
