# Hub Firmware

Central hub firmware for boat monitoring system.

## Overview

The hub is the central coordinator that:
- Receives data from all nodes via LoRa
- Coordinates alarm state across the system
- Displays unified view of all sensors
- Logs data to SD card
- Provides user interface for system control

## Files

- `hub_firmware.ino` - Main Arduino sketch
- `lora/LoRaHub.h` - LoRa receiver and node communication
- `display/HubDisplay.h` - Large TFT display management
- `storage/DataLogger.h` - SD card data logging
- `alarm/AlarmManager.h` - Centralized alarm state management

## Pin Configuration

Default pins for ESP32-S3:

```cpp
// I2C (for RTC, optional sensors)
#define I2C_SDA         8
#define I2C_SCL         7

// LoRa (RFM95W)
#define LORA_CS         10
#define LORA_INT        9
#define LORA_RST        255

// Display (ILI9488)
#define TFT_CS          34
#define TFT_DC          33
#define TFT_RST         21

// SD Card
#define SD_CS           38

// Buzzer/Speaker
#define BUZZER_PIN      15

// Control Buttons
#define BTN_ARM         3
#define BTN_DISARM      4
#define BTN_SILENCE     5
```

## Node Registry

Register all nodes in `setup()`:

```cpp
registerNode(0x01, "Companionway");
registerNode(0x02, "Foredeck");
registerNode(0x03, "Cockpit");
registerNode(0x04, "Cabin");
registerNode(0x05, "Engine");
```

Each node registration creates an entry in the node array with:
- Node ID (unique identifier)
- Name (human-readable)
- Status (online/offline)
- Last contact time
- Sensor data (temp, humidity, pressure)
- Battery voltage
- Signal strength (RSSI)

## Hub Functions

### 1. Data Aggregation

Hub receives and processes three types of messages:

**Environmental Data (every 5 minutes):**
- Temperature
- Humidity
- Barometric pressure
- Battery voltage
- RSSI/SNR

**Detection Events (immediate):**
- Event type (approach/entry/doorbell)
- Confidence level
- Distance
- Zone

**Alarm Messages (immediate):**
- Alarm triggered
- Node arming/disarming

### 2. Alarm Coordination

Hub maintains global alarm state:
- Current mode (Disarmed, Doorbell, Perimeter, Full, Quiet)
- Triggered status
- Which node triggered alarm
- Alarm duration

Commands sent to nodes:
- Arm system (broadcast to all nodes)
- Disarm system
- Silence alarm

### 3. Data Logging

All data logged to SD card in CSV format:

```
Timestamp,NodeID,Type,Data1,Data2,Data3,Data4
12345,01,ENV,23.5,62.0,1013.2,3700,-72
12456,01,DETECT,1,85,150,1
12567,HUB,ALARM,Triggered by Companionway
```

Log file: `/boat_log.csv`

### 4. Node Health Monitoring

Hub tracks node status:
- Mark node offline if no contact for 10 minutes
- Alert on low battery (<3.3V)
- Monitor signal strength
- Detect communication failures

### 5. Display Management

Hub display shows:
- All nodes in grid layout
- Status indicators (online/offline)
- Current environmental data
- Alarm mode and status
- Alert messages

## Controls

### Button Functions

**BTN_ARM:**
- Cycles through armed modes
- Disarmed → Perimeter → Full → Perimeter...
- Broadcasts arm command to all nodes

**BTN_DISARM:**
- Immediately disarms entire system
- Broadcasts disarm to all nodes
- Stops local alarm

**BTN_SILENCE:**
- Silences active alarm
- Disarms system
- Logs event

### Future: Touchscreen Interface

With touchscreen display:
- Tap node to view details
- Tap mode to change settings
- Swipe for additional screens
- Touch controls for arming/disarming

## Alarm State Machine

```
DISARMED ──[BTN_ARM]──> PERIMETER_ARMED
    ↑                          │
    │                          │ [detection]
    │                          ↓
[BTN_DISARM] ←──── ALARM_TRIGGERED
```

## LoRa Communication

### Receive

Hub listens continuously for:
- Packet type identification
- Checksum verification
- RSSI/SNR measurement
- Callback dispatch

### Transmit

Hub sends:
- Alarm commands to nodes
- Configuration updates
- Time sync broadcasts (every 10 minutes)

## Data Logging Format

### Environmental Log Entry
```
Timestamp,NodeID,Type,Temperature,Humidity,Pressure,BatteryMv,RSSI
12345,01,ENV,23.5,62.0,1013.2,3700,-72
```

### Detection Log Entry
```
Timestamp,NodeID,Type,EventType,Confidence,Distance,Zone
12456,01,DETECT,1,85,150,1
```

### Alarm Log Entry
```
Timestamp,NodeID,Type,Event
12567,01,ALARM,Triggered
```

## Configuration

### LoRa Frequency

Set for your region in `setup()`:

```cpp
#define LORA_FREQUENCY 915.0  // US/Americas
// #define LORA_FREQUENCY 868.0  // Europe
// #define LORA_FREQUENCY 433.0  // Asia
```

### Node Timeout

Adjust offline detection threshold:

```cpp
#define NODE_TIMEOUT 600000  // 10 minutes (milliseconds)
```

### Alarm PIN

Change in `AlarmManager.h`:

```cpp
alarmPin("1234")  // Default PIN
```

## Callbacks

Hub uses callbacks to handle received data:

### Environmental Data Callback

```cpp
void onEnvDataReceived(uint8_t nodeID, float temp, float humidity,
                      float pressure, uint16_t batteryMv, int8_t rssi) {
  // Update node registry
  // Log to SD card
  // Check for alerts (low battery, etc.)
}
```

### Detection Callback

```cpp
void onDetectionReceived(uint8_t nodeID, uint8_t eventType,
                        uint8_t confidence, uint16_t distance, uint8_t zone) {
  // Log detection
  // Trigger alarm if armed and high confidence
}
```

### Alarm Callback

```cpp
void onAlarmReceived(uint8_t nodeID, uint8_t command, uint8_t mode) {
  // Handle alarm trigger from node
  // Update alarm state
}
```

## Libraries Required

- RadioHead (LoRa)
- Adafruit_GFX
- Adafruit_ILI9488 (or ILI9486/ILI9341)
- SD (built-in)
- SPI (built-in)
- Wire (built-in)

## Troubleshooting

### No Nodes Detected

- Verify LoRa frequency matches nodes
- Check antenna connection
- Ensure nodes are powered on and transmitting
- Review serial output for received packets

### SD Card Not Working

- Format card as FAT32
- Check SPI wiring
- Verify CS pin
- Try different SD card
- Check for sufficient power (SD cards can draw significant current)

### Display Issues

- Verify SPI connections
- Check backlight power
- Try different TFT library if needed
- Adjust display rotation

### Alarm Not Triggering

- Check alarm mode (must be armed)
- Verify detection confidence threshold
- Review alarm manager state
- Check node communication

## Power Considerations

Hub should be powered from boat's 12V system with backup battery:

**Primary Power:**
- 12V boat supply → LM2596 → 5V
- Fused at source (2A recommended)
- Filtered for noise

**Backup Power:**
- 12V 7Ah SLA battery
- Automatic switchover via Schottky diode
- ~24 hour runtime on backup

**Power Budget:**
- ESP32-S3: ~200mA
- Display: ~100mA
- LoRa RX: ~12mA
- SD card: ~20mA
- Total: ~350mA @ 5V = 1.75W

## Future Enhancements

### WiFi Web Interface

Add web server for remote monitoring:
- Real-time node status
- Historical data graphs
- Configuration interface
- Email/SMS alerts

### NMEA Integration

Output data to chart plotter:
- Custom NMEA sentences for environmental data
- Alarm status on MFD
- Integration with boat's navigation system

### MQTT/SignalK

Modern marine data protocol:
- Publish sensor data
- Subscribe to boat systems
- Integration with home automation

### Cellular/Satellite Alerts

Remote monitoring when away from boat:
- SIM7600 4G module
- RockBLOCK Iridium
- SMS/email on alarm trigger

## Performance

### LoRa Reception

Expected performance:
- Range: 100-200m line-of-sight, 40-60m through boat
- RSSI: -50 to -90 dBm typical
- Packet loss: <1% in normal conditions
- Latency: <500ms for critical alarms

### Data Logging

SD card capacity planning:
- ~200 bytes per environmental sample
- 3 nodes × 12 samples/hour × 24 hours = 864 samples/day
- Daily log size: ~170KB
- 16GB SD card: ~25 years of logs

---

See main README and SETUP_GUIDE for complete documentation.
