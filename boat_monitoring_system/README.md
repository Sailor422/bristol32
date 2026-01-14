# Boat Monitoring System for Bristol 32 "Liberty"

Distributed human detection and environment monitoring system using ESP32 nodes with LoRa communication.

## Overview

This system provides comprehensive monitoring and security for a sailboat using multiple ESP32-based sensor nodes distributed around the vessel. Each node monitors environmental conditions (temperature, humidity, barometric pressure) and human presence via mmWave radar, communicating with a central hub via LoRa radio.

### Key Features

- **Distributed Sensor Network**: 3-5 nodes covering companionway, foredeck, cockpit, cabin, and engine compartment
- **Human Detection**: mmWave radar for motion/presence detection (doorbell and alarm functions)
- **Environmental Monitoring**: Temperature, humidity, barometric pressure with trend analysis
- **LoRa Communication**: Long-range, low-power wireless network optimized for marine environment
- **Multiple Alarm Modes**: Disarmed, Doorbell, Perimeter Armed, Full Armed, Quiet Hours
- **Data Logging**: SD card logging of all sensor data and events
- **Marine Hardened**: Designed for salt air, moisture, vibration, and temperature extremes

## Project Structure

```
boat_monitoring_system/
├── README.md                  # This file
├── common/                    # Shared code between nodes and hub
│   ├── CommonTypes.h         # Data structures and enums
│   └── MessageProtocol.h     # LoRa packet formats
├── node_firmware/            # Firmware for sensor nodes
│   ├── node_firmware.ino     # Main Arduino sketch
│   ├── config/               # Configuration management
│   ├── sensors/              # Sensor driver classes
│   ├── lora/                 # LoRa communication
│   ├── display/              # TFT display management
│   └── README.md
├── hub_firmware/             # Firmware for central hub
│   ├── hub_firmware.ino      # Main Arduino sketch
│   ├── lora/                 # LoRa hub receiver
│   ├── display/              # Hub display management
│   ├── storage/              # SD card data logging
│   ├── alarm/                # Alarm state management
│   └── README.md
└── docs/                     # Documentation
    └── SETUP_GUIDE.md        # Detailed setup instructions
```

## Hardware Requirements

### For Each Node:
- ESP32-S3 or ESP32-WROOM development board
- RFM95W LoRa module (915 MHz for US, 868 MHz for EU, 433 MHz for Asia)
- BME280 temperature/humidity/pressure sensor
- HLK-LD2410 or similar mmWave human presence detector
- 2.4" TFT display (ILI9341) - optional for some nodes
- Piezo buzzer
- Push buttons (3x)
- Marine-grade enclosure (IP65 or better)
- Power supply: 12V boat power with buck converter OR battery + solar

**Estimated cost per node: $35-50**

### For Hub:
- ESP32-S3 development board (or Raspberry Pi Zero 2 W)
- RFM95W LoRa module
- 3.5" or 4.3" TFT touchscreen (ILI9488)
- microSD card module + SD card
- DS3231 RTC module (optional but recommended)
- Buzzer/speaker
- Marine-grade enclosure
- 12V boat power with buck converter + battery backup

**Estimated cost for hub: $70-100**

**Total system cost (3 nodes + hub): $180-250**

## Software Requirements

- Arduino IDE 2.0+ or PlatformIO
- ESP32 board support package
- Required Libraries:
  - RadioHead (LoRa communication)
  - Adafruit_BME280 (environmental sensor)
  - Adafruit_GFX + Adafruit_ILI9341/ILI9488 (displays)
  - SD (data logging)
  - Preferences (ESP32 NVS storage)

## Quick Start

### 1. Install Arduino IDE and Libraries

```bash
# Install Arduino IDE from arduino.cc
# Then install ESP32 board support:
# File -> Preferences -> Additional Board Manager URLs:
# https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

# Install libraries via Library Manager:
# RadioHead, Adafruit BME280, Adafruit GFX, Adafruit ILI9341
```

### 2. Configure and Upload Node Firmware

1. Open `node_firmware/node_firmware.ino` in Arduino IDE
2. Adjust pin definitions for your hardware (lines 28-51)
3. Set LoRa frequency for your region (line 123)
4. Select board: ESP32S3 Dev Module (or your specific board)
5. Configure node ID and name in code or via serial monitor
6. Upload to ESP32

### 3. Configure and Upload Hub Firmware

1. Open `hub_firmware/hub_firmware.ino` in Arduino IDE
2. Adjust pin definitions (lines 28-45)
3. Register your nodes (lines 114-118)
4. Upload to ESP32

### 4. Hardware Assembly

See `docs/SETUP_GUIDE.md` for detailed wiring diagrams and assembly instructions.

### 5. Installation on Boat

1. Mount hub at navigation station or central location
2. Install companionway node near main entrance
3. Install additional nodes as needed
4. Connect to 12V boat power
5. Test LoRa communication range
6. Seal all enclosures and cable penetrations

## Configuration

### Node Configuration

Each node stores configuration in non-volatile storage (ESP32 NVS). Initial configuration can be done via serial monitor or by modifying `NodeConfig.h` defaults:

- Node ID (0x01 - 0xFE)
- Node name
- Detection sensitivity (0-100)
- Detection zones (near/middle/far distances)
- Alarm mode
- Quiet hours settings
- LoRa parameters

### Hub Configuration

Hub configuration is done by editing `hub_firmware.ino`:

- Register nodes (ID and name)
- Set LoRa frequency
- Configure alarm PIN
- Adjust timeouts

## Usage

### Alarm Modes

- **Disarmed**: Detection logged but no alarms
- **Doorbell**: Gentle chime on companionway detection
- **Perimeter Armed**: Exterior nodes trigger alarm
- **Full Armed**: All nodes trigger alarm
- **Quiet Hours**: Reduced sensitivity, silent notifications

### Controls (Node)

- **BTN_MODE**: Cycle through alarm modes
- **BTN_DISPLAY**: Change display view (future feature)
- **BTN_SILENCE**: Silence alarm / disarm

### Controls (Hub)

- **BTN_ARM**: Arm system (cycle Perimeter → Full)
- **BTN_DISARM**: Disarm all nodes
- **BTN_SILENCE**: Silence active alarm

### Data Logging

Hub automatically logs to SD card:
- Environmental data from all nodes
- Detection events
- Alarm triggers
- System events

Log format: CSV (`boat_log.csv`)

## Troubleshooting

### LoRa Communication Issues

- Check RSSI values (should be -50 to -90 dBm)
- Verify antenna connections
- Ensure frequency matches in all devices
- Check for interference from VHF/AIS

### False Detections

- Adjust detection sensitivity (lower = fewer false positives)
- Configure exclusion zones
- Increase minimum confidence threshold
- Ensure mmWave sensor not pointing at moving rigging

### Node Offline

- Check power supply voltage
- Verify LoRa antenna not damaged
- Check for metal obstructions
- Restart node

### Display Issues

- Verify SPI pin connections
- Check backlight power
- Try different TFT library if needed

## Advanced Features (Future)

- WiFi web interface for hub
- NMEA 0183/2000 integration
- MQTT/SignalK support
- Cellular/satellite alerts
- Additional sensors (bilge, battery, GPS anchor watch)
- Over-the-air firmware updates

## Safety and Legal

- This system is supplementary to standard marine safety equipment
- Do not rely solely on this alarm for security
- LoRa transmission must comply with local regulations
- Always use licensed frequencies for your region
- Marine installation should follow ABYC standards

## Contributing

This is a custom system for Bristol 32 "Liberty". Feel free to adapt for your own vessel.

## License

Open source for personal/educational use.

## Acknowledgments

Based on design document for Bristol 32 sailboat "Liberty" distributed monitoring system.

Designed for real-world marine use with consideration for:
- Harsh saltwater environment
- Limited power budget
- Vibration and motion
- Regulatory compliance
- Ease of maintenance

## Support

For questions or issues, refer to:
- `docs/SETUP_GUIDE.md` - Detailed assembly and installation
- Design document - Complete technical specifications
- Arduino forums - ESP32 and library support

---

**Fair winds and following seas!**
