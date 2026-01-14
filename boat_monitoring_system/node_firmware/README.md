# Node Firmware

Firmware for distributed sensor nodes in the boat monitoring system.

## Overview

Each node is an autonomous sensor station that:
- Monitors environmental conditions (temp/humidity/pressure)
- Detects human presence via mmWave radar
- Displays local data on TFT screen
- Communicates with hub via LoRa
- Provides local alarm functionality

## Files

- `node_firmware.ino` - Main Arduino sketch
- `config/NodeConfig.h` - Configuration management (NVS storage)
- `sensors/` - Sensor abstraction classes
  - `SensorBase.h` - Base class for all sensors
  - `BME280Sensor.h` - Environmental sensor (temp/humidity/pressure)
  - `HumanDetector.h` - mmWave human presence detector
- `lora/LoRaComm.h` - LoRa communication manager
- `display/DisplayManager.h` - TFT display rendering

## Pin Configuration

Default pins for ESP32-S3 (adjust in node_firmware.ino):

```cpp
// I2C (BME280)
#define I2C_SDA         8
#define I2C_SCL         7

// LoRa (RFM95W)
#define LORA_CS         10
#define LORA_INT        9
#define LORA_RST        255  // Not connected

// Display (ILI9341)
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

// Battery voltage (ADC)
#define BATTERY_PIN     1
```

## Configuration

Node configuration is stored in ESP32 NVS (non-volatile storage) and persists across reboots.

### Initial Setup

On first boot or after factory reset, default values are used:
- Node ID: 0x01
- Node Name: "Node 1"
- Alarm Mode: Disarmed
- Detection Sensitivity: 50 (medium)
- LoRa Frequency: 915.0 MHz (US)

### Changing Configuration

Temporarily modify in `setup()`:

```cpp
config.nodeID = 0x02;  // Set to unique ID
config.nodeName = "Foredeck";
config.detectionSensitivity = 60;  // 0-100
config.loraFrequency = 868.0;  // EU frequency
config.save();
```

Or implement serial configuration menu (future feature).

## Alarm Modes

Cycle through modes with MODE button:

1. **Disarmed**: Detection logged, no alarms
2. **Doorbell**: Pleasant chime on detection
3. **Perimeter**: Pre-alarm warning → full alarm if sustained
4. **Full**: Immediate alarm on detection

## State Machine

The node operates in these states:

- `STATE_INIT` - Initializing hardware
- `STATE_NORMAL` - Normal operation, monitoring
- `STATE_PRE_ALARM` - 5-second warning before full alarm
- `STATE_ALARM_TRIGGERED` - Alarm sounding
- `STATE_SLEEP` - Low power mode (future)
- `STATE_ERROR` - Hardware fault

## LoRa Communication

### Transmit Schedule

- **Environmental data**: Every 5 minutes (configurable)
- **Heartbeat**: Every 60 seconds
- **Detection events**: Immediate
- **Alarm triggers**: Immediate + every 5s until acknowledged

### Received Messages

Nodes can receive from hub:
- Alarm commands (arm/disarm)
- Configuration updates
- Time synchronization

## Detection Logic

Human detection uses multi-stage filtering to reduce false positives:

1. **Confidence check**: Must exceed threshold (default 70%)
2. **Duration check**: Must sustain for 2+ seconds
3. **Distance check**: Must be within configured zones (0-600cm)
4. **Optional**: Boat motion compensation (requires IMU)

## Power Consumption

Typical power draw (3.3V):

- Normal operation: ~150mA average
- Deep sleep: ~10µA (future feature)
- Alarm active: ~200mA

With 5-minute reporting interval:
- Daily consumption: ~240mAh
- 3000mAh battery life: ~12 days

## Libraries Required

- RadioHead (LoRa)
- Adafruit_BME280
- Adafruit_GFX
- Adafruit_ILI9341 (or ILI9488)
- Preferences (built-in ESP32)

## Troubleshooting

### Sensor Init Failed

Check I2C connections and addresses:
```cpp
// BME280 typically at 0x76 or 0x77
// Run I2C scanner to verify
```

### LoRa Not Transmitting

- Verify antenna connected
- Check SPI wiring
- Ensure frequency setting matches hub
- Check serial output for error messages

### Detection Not Working

- Verify UART connection to mmWave sensor
- Check sensor power (needs 5V)
- Implement sensor-specific configuration commands
- Try reducing sensitivity

## Customization

### Add New Sensor

1. Create sensor class inheriting from `SensorBase`
2. Implement `begin()`, `read()`, `isAvailable()`, `getStatusString()`
3. Instantiate in main firmware
4. Add to display and LoRa transmission

### Change Display Layout

Modify `DisplayManager::drawMainScreen()` to customize:
- Font sizes
- Layout
- Colors
- Additional info

### Adjust Alarm Timing

In `handleDetection()`:
```cpp
preAlarmDuration = 10000;  // 10 seconds instead of 5
```

## Future Enhancements

- Web-based configuration interface
- Over-the-air (OTA) firmware updates
- Additional sensor support (CO2, VOC, etc.)
- Advanced motion compensation
- Multi-language display support

---

See main README and SETUP_GUIDE for complete documentation.
