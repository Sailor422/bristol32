/*
 * Boat Monitoring System - Node Firmware
 * For Bristol 32 "Liberty"
 *
 * This firmware runs on ESP32-based sensor nodes distributed around the boat
 * Each node monitors environmental conditions and human presence via mmWave radar
 * Data is transmitted to central hub via LoRa
 *
 * Hardware:
 *   - ESP32-S3 or ESP32-WROOM
 *   - RFM95W LoRa module (915 MHz for US)
 *   - BME280 temperature/humidity/pressure sensor
 *   - HLK-LD2410 mmWave human presence detector
 *   - 2.4" TFT display (ILI9341)
 *   - Piezo buzzer for alarms
 *
 * Author: Auto-generated from design document
 * Version: 1.0.0
 */

#include <Arduino.h>
#include <Wire.h>
#include "config/NodeConfig.h"
#include "sensors/BME280Sensor.h"
#include "sensors/HumanDetector.h"
#include "lora/LoRaComm.h"
#include "display/DisplayManager.h"
#include "../common/CommonTypes.h"
#include "../common/MessageProtocol.h"

// ============================================================================
// PIN DEFINITIONS (Adjust for your hardware)
// ============================================================================

// I2C (for BME280)
#define I2C_SDA         8
#define I2C_SCL         7

// LoRa (RFM95W)
#define LORA_CS         10
#define LORA_INT        9
#define LORA_RST        255  // Not connected, use 255

// Display (ILI9341 SPI)
#define TFT_CS          34
#define TFT_DC          33
#define TFT_RST         21

// Human Detector (UART)
#define HUMAN_RX        17
#define HUMAN_TX        18

// Buzzer
#define BUZZER_PIN      15

// Buttons
#define BTN_MODE        3
#define BTN_DISPLAY     4
#define BTN_SILENCE     5

// Battery voltage sensing (ADC)
#define BATTERY_PIN     1
#define BATTERY_FACTOR  2.0  // Voltage divider factor

// ============================================================================
// GLOBAL OBJECTS
// ============================================================================

NodeConfig config;
BME280Sensor envSensor;
HumanDetector motionSensor(HUMAN_RX, HUMAN_TX);
LoRaComm lora(LORA_CS, LORA_INT, LORA_RST, 0x01, 0x00); // Will be updated from config
DisplayManager display(TFT_CS, TFT_DC, TFT_RST);

// ============================================================================
// STATE MACHINE
// ============================================================================

SystemState currentState = STATE_INIT;
AlarmMode currentMode = MODE_DISARMED;

// ============================================================================
// TIMING VARIABLES
// ============================================================================

unsigned long lastEnvRead = 0;
unsigned long lastEnvTransmit = 0;
unsigned long lastHeartbeat = 0;
unsigned long lastDisplayUpdate = 0;
unsigned long lastButtonCheck = 0;
unsigned long preAlarmStartTime = 0;

// ============================================================================
// FUNCTION DECLARATIONS
// ============================================================================

void setupPins();
uint16_t readBatteryVoltage();
void handleDetection();
void handleButtons();
void updateDisplay();
void soundAlarm();
void playDoorbellChime();
void enterSleepMode();
bool validateDetection(const DetectionEvent& event);
void onLoRaMessage(uint8_t* data, uint8_t len, uint8_t from);

// ============================================================================
// SETUP
// ============================================================================

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n========================================");
  Serial.println("  Boat Monitoring System - Node");
  Serial.println("  Bristol 32 'Liberty'");
  Serial.println("========================================\n");

  // Setup pins
  setupPins();

  // Load configuration
  if (!config.load()) {
    Serial.println("WARNING: Failed to load config, using defaults");
  }

  // Initialize I2C bus
  Wire.begin(I2C_SDA, I2C_SCL);

  // Initialize sensors
  Serial.println("Initializing sensors...");

  if (!envSensor.begin()) {
    Serial.println("ERROR: BME280 init failed!");
    currentState = STATE_ERROR;
  } else {
    Serial.println("BME280 initialized successfully");
  }

  if (!motionSensor.begin()) {
    Serial.println("WARNING: Motion sensor init failed");
  } else {
    Serial.println("Motion sensor initialized successfully");
    // Configure detection parameters from config
    motionSensor.setZones(config.nearZoneMax, config.middleZoneMax, config.farZoneMax);
    motionSensor.setSensitivity(config.getMinConfidence(), config.getMinDuration());
  }

  // Initialize LoRa
  Serial.println("Initializing LoRa...");
  if (!lora.begin(config.loraFrequency)) {
    Serial.println("ERROR: LoRa init failed!");
    currentState = STATE_ERROR;
  } else {
    Serial.println("LoRa initialized successfully");
    lora.setMessageCallback(onLoRaMessage);
  }

  // Initialize display
  Serial.println("Initializing display...");
  if (!display.begin()) {
    Serial.println("WARNING: Display init failed");
  } else {
    display.showBootScreen(config.nodeName);
  }

  // Send boot notification
  Serial.println("Sending boot notification...");
  uint16_t batteryMv = readBatteryVoltage();
  lora.sendHeartbeat(batteryMv);

  // Read initial sensor values
  envSensor.read();

  currentMode = config.alarmMode;
  currentState = STATE_NORMAL;

  Serial.println("\nNode initialized successfully!");
  Serial.print("Node ID: 0x");
  Serial.println(config.nodeID, HEX);
  Serial.print("Node Name: ");
  Serial.println(config.nodeName);
  Serial.println("Ready to operate.\n");
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
  unsigned long now = millis();

  // Always process incoming LoRa messages
  lora.processIncoming();

  // Handle button presses
  if (now - lastButtonCheck >= 100) {
    handleButtons();
    lastButtonCheck = now;
  }

  // State machine
  switch (currentState) {

    // ========================================================================
    // NORMAL OPERATION
    // ========================================================================
    case STATE_NORMAL:
      // Read environmental sensors periodically
      if (now - lastEnvRead >= 60000) { // Every 60 seconds
        envSensor.read();
        lastEnvRead = now;

        // Check for storm warning
        if (envSensor.isStormApproaching()) {
          Serial.println("WARNING: Pressure falling rapidly - storm approaching!");
        }
      }

      // Check for human detection
      if (motionSensor.detectionEvent()) {
        handleDetection();
      }

      // Transmit environmental data periodically
      if (now - lastEnvTransmit >= config.envDataInterval) {
        EnvData data;
        data.nodeID = config.nodeID;
        data.temperature = envSensor.getTemperature();
        data.humidity = envSensor.getHumidity();
        data.pressure = envSensor.getPressure();
        data.batteryVoltage = readBatteryVoltage();
        data.rssi = lora.getLastRSSI();

        if (lora.sendEnvironmentalData(data)) {
          Serial.println("Environmental data transmitted");
        }

        lastEnvTransmit = now;
      }

      // Send heartbeat
      if (now - lastHeartbeat >= config.heartbeatInterval) {
        lora.sendHeartbeat(readBatteryVoltage());
        lastHeartbeat = now;
      }

      // Update display
      if (now - lastDisplayUpdate >= 1000) {
        updateDisplay();
        lastDisplayUpdate = now;
      }

      break;

    // ========================================================================
    // PRE-ALARM WARNING
    // ========================================================================
    case STATE_PRE_ALARM:
      {
        unsigned long elapsed = (now - preAlarmStartTime) / 1000;
        int countdown = 5 - elapsed;

        if (countdown > 0) {
          // Show countdown warning
          display.showPreAlarm(countdown);

          // Beep periodically
          if ((now / 500) % 2 == 0) {
            tone(BUZZER_PIN, 2000, 100);
          }
        } else {
          // Timeout - trigger full alarm
          currentState = STATE_ALARM_TRIGGERED;
          Serial.println("Pre-alarm timeout - triggering full alarm");
        }
      }
      break;

    // ========================================================================
    // ALARM TRIGGERED
    // ========================================================================
    case STATE_ALARM_TRIGGERED:
      soundAlarm();

      // Send alarm trigger to hub
      static unsigned long lastAlarmSend = 0;
      if (now - lastAlarmSend >= 5000) {
        lora.sendAlarmTrigger(currentMode);
        lastAlarmSend = now;
      }

      // Display alarm screen
      display.showAlarmTriggered(config.nodeName);
      break;

    // ========================================================================
    // SLEEP MODE (Low Power)
    // ========================================================================
    case STATE_SLEEP:
      // Deep sleep implementation would go here
      // For now, just reduce activity
      delay(1000);
      break;

    // ========================================================================
    // ERROR STATE
    // ========================================================================
    case STATE_ERROR:
      display.showError("System Error");
      delay(1000);
      break;
  }

  // Check battery voltage periodically
  static unsigned long lastBatteryCheck = 0;
  if (now - lastBatteryCheck >= 60000) {
    uint16_t batteryMv = readBatteryVoltage();
    if (batteryMv < 3300 && batteryMv > 1000) { // Below 3.3V (and not zero/invalid)
      Serial.print("WARNING: Low battery - ");
      Serial.print(batteryMv);
      Serial.println(" mV");
    }
    lastBatteryCheck = now;
  }

  // Small delay to prevent watchdog issues
  delay(10);
}

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

void setupPins() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  pinMode(BTN_MODE, INPUT_PULLUP);
  pinMode(BTN_DISPLAY, INPUT_PULLUP);
  pinMode(BTN_SILENCE, INPUT_PULLUP);

  pinMode(BATTERY_PIN, INPUT);
}

uint16_t readBatteryVoltage() {
  int rawValue = analogRead(BATTERY_PIN);
  // Convert ADC reading to voltage (ESP32 ADC is 12-bit, 0-4095 for 0-3.3V)
  // Account for voltage divider if used
  float voltage = (rawValue / 4095.0) * 3.3 * BATTERY_FACTOR;
  return (uint16_t)(voltage * 1000); // Return in millivolts
}

void handleDetection() {
  DetectionEvent event = motionSensor.getEvent();

  Serial.println("Detection event!");
  Serial.print("  Confidence: ");
  Serial.print(event.confidence);
  Serial.print("%, Distance: ");
  Serial.print(event.distance);
  Serial.print("cm, Zone: ");
  Serial.println(event.zone);

  // Validate detection (apply filtering)
  if (!validateDetection(event)) {
    Serial.println("  -> Filtered as false positive");
    return;
  }

  // Get effective mode (considering quiet hours)
  AlarmMode effectiveMode = config.getEffectiveAlarmMode();

  // Handle based on mode
  switch (effectiveMode) {
    case MODE_DISARMED:
      Serial.println("  -> Mode: Disarmed, logging only");
      // Just log the event, no action
      lora.sendDetectionEvent(event);
      break;

    case MODE_DOORBELL:
      Serial.println("  -> Mode: Doorbell, playing chime");
      playDoorbellChime();
      display.showVisitor();
      lora.sendDetectionEvent(event);
      break;

    case MODE_QUIET:
      Serial.println("  -> Mode: Quiet hours, silent notification");
      // Send to hub but no local alarm
      lora.sendDetectionEvent(event);
      break;

    case MODE_PERIMETER:
    case MODE_FULL:
      Serial.println("  -> Mode: Armed, triggering pre-alarm");
      // Start pre-alarm warning
      currentState = STATE_PRE_ALARM;
      preAlarmStartTime = millis();
      lora.sendDetectionEvent(event);
      break;
  }
}

bool validateDetection(const DetectionEvent& event) {
  // Apply false-positive filtering (see design doc section 2.3)

  // 1. Check confidence level
  if (event.confidence < config.getMinConfidence()) {
    return false;
  }

  // 2. Check sustained duration
  if (event.duration < config.getMinDuration()) {
    return false;
  }

  // 3. Check distance (must be within configured zones)
  if (event.distance > config.farZoneMax || event.distance < 10) {
    return false; // Too far or too close (false reading)
  }

  // 4. TODO: Add IMU-based boat motion compensation
  // if (getBoatMotion() > MOTION_THRESHOLD) {
  //   Require higher confidence when boat is rocking
  // }

  return true; // Valid detection
}

void handleButtons() {
  // Mode button - cycle through modes
  static bool btnModePressed = false;
  if (digitalRead(BTN_MODE) == LOW && !btnModePressed) {
    btnModePressed = true;

    // Cycle mode: Disarmed -> Doorbell -> Perimeter -> Full -> Disarmed
    switch (currentMode) {
      case MODE_DISARMED:
        currentMode = MODE_DOORBELL;
        break;
      case MODE_DOORBELL:
        currentMode = MODE_PERIMETER;
        break;
      case MODE_PERIMETER:
        currentMode = MODE_FULL;
        break;
      case MODE_FULL:
        currentMode = MODE_DISARMED;
        break;
      default:
        currentMode = MODE_DISARMED;
        break;
    }

    config.alarmMode = currentMode;
    config.save();

    Serial.print("Mode changed to: ");
    Serial.println(alarmModeToString(currentMode));

    tone(BUZZER_PIN, 1000, 100); // Acknowledge beep

  } else if (digitalRead(BTN_MODE) == HIGH) {
    btnModePressed = false;
  }

  // Silence button - disarm alarm or stop pre-alarm
  static bool btnSilencePressed = false;
  if (digitalRead(BTN_SILENCE) == LOW && !btnSilencePressed) {
    btnSilencePressed = true;

    if (currentState == STATE_PRE_ALARM || currentState == STATE_ALARM_TRIGGERED) {
      Serial.println("Alarm silenced/disarmed");
      currentState = STATE_NORMAL;
      currentMode = MODE_DISARMED;
      config.alarmMode = MODE_DISARMED;
      config.save();
      noTone(BUZZER_PIN);
    }

  } else if (digitalRead(BTN_SILENCE) == HIGH) {
    btnSilencePressed = false;
  }
}

void updateDisplay() {
  if (currentState != STATE_NORMAL) {
    return; // Don't update display in alarm/error states
  }

  display.drawMainScreen(
    config.nodeName,
    config.convertTemperature(envSensor.getTemperature()),
    envSensor.getHumidity(),
    envSensor.getPressure(),
    envSensor.getTrendArrow(),
    config.getEffectiveAlarmMode(),
    lora.getLastRSSI(),
    readBatteryVoltage()
  );
}

void soundAlarm() {
  // Continuous loud alarm tone
  static unsigned long lastToggle = 0;
  static bool toneOn = false;

  if (millis() - lastToggle >= 500) {
    if (toneOn) {
      noTone(BUZZER_PIN);
    } else {
      tone(BUZZER_PIN, 2500); // 2.5 kHz alarm tone
    }
    toneOn = !toneOn;
    lastToggle = millis();
  }
}

void playDoorbellChime() {
  // Pleasant doorbell chime (simple melody)
  tone(BUZZER_PIN, 800, 200);
  delay(250);
  tone(BUZZER_PIN, 600, 300);
  delay(350);
  noTone(BUZZER_PIN);
}

void enterSleepMode() {
  // Implement deep sleep for battery-powered nodes
  // This is a placeholder - actual implementation depends on power strategy

  Serial.println("Entering sleep mode...");

  // esp_sleep_enable_timer_wakeup(config.envDataInterval * 1000);
  // esp_light_sleep_start();

  currentState = STATE_NORMAL;
}

void onLoRaMessage(uint8_t* data, uint8_t len, uint8_t from) {
  // Callback for incoming LoRa messages
  // Additional processing can be done here

  Serial.print("LoRa message received from 0x");
  Serial.print(from, HEX);
  Serial.print(", type: 0x");
  Serial.println(data[1], HEX);

  // Handle specific message types if needed
  if (data[1] == MSG_TYPE_ALARM) {
    // Alarm command received - could update mode
    Serial.println("Alarm command received from hub");
  }
}
