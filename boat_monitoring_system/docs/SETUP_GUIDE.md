# Boat Monitoring System - Setup Guide

Complete step-by-step guide for building and installing the distributed monitoring system on Bristol 32 "Liberty".

## Table of Contents

1. [Parts List](#parts-list)
2. [Tools Required](#tools-required)
3. [Software Setup](#software-setup)
4. [Hardware Assembly](#hardware-assembly)
5. [Firmware Upload](#firmware-upload)
6. [Testing](#testing)
7. [Marine Installation](#marine-installation)
8. [Configuration](#configuration)

---

## Parts List

### Per Node (Build 3-5 nodes)

#### Electronics
- 1x ESP32-S3-DevKitC-1 or ESP32-WROOM-32 ($8-12)
- 1x RFM95W LoRa module 915MHz ($6-8)
- 1x BME280 breakout board ($5-10)
- 1x HLK-LD2410 mmWave sensor ($10-15)
- 1x 2.4" TFT LCD ILI9341 ($12-18)
- 1x Piezo buzzer 5V ($1-2)
- 3x Tactile push buttons ($0.50 each)
- 1x LM2596 buck converter 12V→5V ($2-4)
- 1x AMS1117-3.3 LDO regulator ($0.50)
- Resistors, capacitors, wires

#### Enclosure & Power
- 1x IP65 plastic enclosure (Hammond 1554 or similar) ($12-20)
- 2x PG7 cable glands ($2 each)
- 1x 915MHz wire antenna or spring whip ($0-5)
- Marine-grade wire 18AWG ($2/meter)
- 1x Inline fuse holder + 1A fuse ($2)

**Total per node: ~$75-110 depending on display choice**

### Hub

#### Electronics
- 1x ESP32-S3-DevKitC-1 ($10-15)
- 1x RFM95W LoRa module 915MHz ($6-8)
- 1x 3.5" TFT LCD ILI9488 with touch ($25-35)
- 1x microSD card module ($2-4)
- 1x DS3231 RTC module with battery ($3-5)
- 1x microSD card 16GB+ ($8-12)
- 1x Speaker/buzzer ($3-6)
- 3x Push buttons or keyswitch ($5-10)
- 1x LM2596 buck converter ($3-5)
- 1x 12V 7Ah SLA battery (backup) ($25-35)

#### Enclosure
- 1x IP54 enclosure for hub ($20-30)
- Cable glands, wires, fuse holders

**Total hub: ~$140-200**

---

## Tools Required

- Soldering iron and solder
- Wire strippers
- Multimeter
- Small screwdrivers
- Drill + step bit (for cable glands)
- Heat shrink tubing + heat gun
- Cable ties
- Marine sealant (3M 4200 or 5200)
- Crimping tool (optional but recommended)

---

## Software Setup

### 1. Install Arduino IDE

Download from [arduino.cc](https://www.arduino.cc/en/software)

### 2. Add ESP32 Board Support

1. Open Arduino IDE
2. File → Preferences
3. Additional Board Manager URLs:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Tools → Board → Boards Manager
5. Search "esp32" and install "esp32 by Espressif Systems"

### 3. Install Required Libraries

Tools → Manage Libraries, search and install:

- **RadioHead** by Mike McCauley
- **Adafruit BME280 Library**
- **Adafruit BusIO**
- **Adafruit GFX Library**
- **Adafruit ILI9341** (for 2.4" displays)
- **Adafruit ILI9488** (for larger displays)

### 4. Download Project Code

Clone or download the `boat_monitoring_system` folder to your Arduino projects directory.

---

## Hardware Assembly

### Node Assembly (Breadboard Prototype First!)

#### Step 1: ESP32 + LoRa Wiring

**ESP32-S3 to RFM95W:**
```
ESP32        RFM95W
GPIO10   →   CS (NSS)
GPIO11   →   MOSI
GPIO12   →   SCK
GPIO13   →   MISO
GPIO9    →   DIO0 (INT)
GND      →   GND
3.3V     →   VCC
```

**IMPORTANT:** RFM95W operates at 3.3V only - never connect to 5V!

#### Step 2: I2C Sensors

**ESP32-S3 to BME280:**
```
ESP32        BME280
GPIO8    →   SDA
GPIO7    →   SCL
3.3V     →   VCC
GND      →   GND
```

#### Step 3: mmWave Sensor (UART)

**ESP32-S3 to HLK-LD2410:**
```
ESP32        HLK-LD2410
GPIO17   →   RX
GPIO18   →   TX
5V       →   VCC (via buck converter)
GND      →   GND
```

#### Step 4: Display (SPI)

**ESP32-S3 to ILI9341:**
```
ESP32        ILI9341
GPIO34   →   CS
GPIO33   →   DC
GPIO21   →   RST
GPIO35   →   MOSI
GPIO36   →   SCK
GPIO37   →   MISO
5V       →   LED (backlight)
3.3V     →   VCC
GND      →   GND
```

#### Step 5: Buttons & Buzzer

**Buttons:** Connect between GPIO and GND (use internal pullups)
- BTN_MODE: GPIO3
- BTN_DISPLAY: GPIO4
- BTN_SILENCE: GPIO5

**Buzzer:** GPIO15 with NPN transistor driver

#### Step 6: Power Supply

**12V Boat Power → LM2596 → 5V → AMS1117 → 3.3V**

Add protection:
- TVS diode (P6KE18A) across 12V input
- Reverse polarity protection (Schottky diode)
- 1A fast-blow fuse

### Hub Assembly

Similar to node, plus:

**SD Card Module:**
```
ESP32        SD Module
GPIO38   →   CS
GPIO39   →   SCK
GPIO40   →   MOSI
GPIO41   →   MISO
5V       →   VCC
GND      →   GND
```

**RTC Module:**
```
ESP32        DS3231
GPIO8    →   SDA (shared with BME280 if present)
GPIO7    →   SCL
3.3V     →   VCC
GND      →   GND
```

---

## Firmware Upload

### For Nodes:

1. Connect ESP32 to computer via USB
2. Open `node_firmware/node_firmware.ino`
3. Tools → Board → ESP32S3 Dev Module (or your board)
4. Tools → Port → Select your COM port
5. Modify pin definitions if needed (lines 28-51)
6. **Important:** Set LoRa frequency for your region:
   - US/Americas: 915.0 MHz
   - Europe: 868.0 MHz
   - Asia: 433.0 MHz
7. Upload (Ctrl+U)
8. Open Serial Monitor (115200 baud) to verify operation

**Configure Node ID:**
First node should have ID 0x01, second 0x02, etc.
- Modify `config.nodeID` in setup() temporarily, or
- Add serial configuration menu (future feature)

### For Hub:

1. Same process as node
2. Open `hub_firmware/hub_firmware.ino`
3. Register your nodes in setup() (lines 114-118)
4. Upload

---

## Testing

### Bench Test (Before Marine Installation)

#### Test 1: LoRa Communication

1. Power on hub
2. Power on first node
3. Observe serial output on both:
   - Node should send environmental data every 5 minutes
   - Hub should receive and display RSSI/SNR values

**Expected RSSI:** -30 to -70 dBm at close range

#### Test 2: Environmental Sensors

Check node serial output:
```
BME280: 23.5C 62% 1013hPa
```

Verify values are reasonable.

#### Test 3: Human Detection

Walk near mmWave sensor:
- Node should detect presence
- Serial output: "Detection event! Confidence: 85%, Distance: 150cm"

#### Test 4: Alarm System

1. Press MODE button on node to arm (Perimeter mode)
2. Trigger detection
3. After 5-second warning, alarm should sound
4. Press SILENCE to disarm

#### Test 5: Display

Verify all screens render correctly:
- Boot screen
- Main environmental display
- Alarm screen

### Range Test

1. Place hub in boat's nav station
2. Walk around boat with battery-powered node
3. Note RSSI values at each location
4. Mark any "dead zones" where signal drops below -100 dBm

**Target:** Reliable communication (RSSI > -95 dBm) throughout boat

---

## Marine Installation

### Location Planning

**Hub:** Navigation station or main cabin
- Central location for best LoRa coverage
- Protected from weather
- Easy access to 12V power
- Visible display

**Companionway Node:**
- Inside companionway, upper bulkhead
- Sensor aimed at boarding steps
- IP65+ rated enclosure
- Height: ~1.5m from sole

**Cockpit Node:**
- Inside cockpit locker (if dry) or
- Weather-protected cockpit bulkhead
- Sensor aimed aft
- Easy to read display

**Foredeck Node:**
- Below deck near forward hatch
- Sensor aimed forward through deck vent
- Battery-powered option for easier install

**Cabin/Engine Nodes:**
- Overhead or bulkhead mounted
- Away from direct spray
- Good ventilation

### Power Wiring

1. **Plan wire runs:**
   - Avoid bilge, engine compartment heat
   - Use existing wire chases if possible
   - Keep away from VHF/AIS antennas

2. **Wire installation:**
   - Use marine-grade tinned copper wire (18 AWG)
   - Label both ends clearly
   - Add inline fuse at power source (1A)
   - Crimp connections with heat shrink
   - Test voltage at each node location (should be 11.5-14.5V)

3. **Connection to boat's 12V:**
   - Tap into accessory panel or
   - Direct to battery with circuit breaker
   - Use proper marine-grade terminals

### Enclosure Mounting

1. **Drill pilot holes:**
   - Use marine sealant under screw heads
   - Stainless steel screws only (not zinc-plated)
   - Don't overtighten plastic enclosures

2. **Cable entry:**
   - Drill holes for PG7 cable glands
   - Apply silicone sealant to threads
   - Tighten glands around cables for waterproofing

3. **Antenna placement:**
   - Vertical orientation preferred
   - Away from metal (mast, tanks, engine)
   - Through-deck gland for best performance (optional)

### Final Installation Steps

1. Mount hub in nav station
2. Install companionway node
3. Install remaining nodes
4. Connect all power
5. Verify LoRa communication from each node
6. Seal all penetrations with marine sealant
7. Cable-tie all loose wires
8. Label all components

---

## Configuration

### Node Configuration

Access via serial monitor after upload:

```cpp
// In node_firmware.ino setup():
config.nodeID = 0x01;          // Unique ID for each node
config.nodeName = "Companionway";
config.detectionSensitivity = 50;  // 0-100 (50=default)
config.nearZoneMax = 100;      // cm
config.middleZoneMax = 300;    // cm
config.farZoneMax = 600;       // cm
config.save();
```

### Hub Configuration

Register all nodes:

```cpp
// In hub_firmware.ino setup():
registerNode(0x01, "Companionway");
registerNode(0x02, "Foredeck");
registerNode(0x03, "Cockpit");
registerNode(0x04, "Cabin");
registerNode(0x05, "Engine");
```

### Alarm PIN

Default PIN: `1234`

Change in `AlarmManager.h`:
```cpp
alarmPin("your_pin_here")
```

### Quiet Hours

Configure in `NodeConfig.h`:
```cpp
quietHoursEnabled = true;
quietHourStart = 22;  // 10 PM
quietHourEnd = 6;     // 6 AM
```

---

## Maintenance

### Weekly:
- Check node displays for warnings
- Verify all nodes online on hub

### Monthly:
- Check battery voltages
- Inspect enclosures for water ingress
- Clean displays
- Test alarm trigger

### Quarterly:
- Tighten mounting screws
- Check antenna connections
- Verify sensor accuracy (compare to known values)

### Annually:
- Replace batteries (battery-powered nodes)
- Check LoRa signal strength
- Firmware updates if available
- Re-apply conformal coating if repairs made

---

## Troubleshooting Guide

### Node Won't Boot
- Check power supply voltage (should be 3.3V and 5V)
- Verify ESP32 USB connection
- Re-upload firmware
- Check serial monitor for error messages

### No LoRa Communication
- Verify antenna connected
- Check frequency setting (must match hub)
- Inspect SPI wiring (MOSI, MISO, SCK, CS)
- Test RSSI with devices close together

### False Alarms
- Reduce detection sensitivity
- Adjust mmWave sensor angle (away from moving objects)
- Increase minimum confidence threshold
- Configure exclusion zones

### Display Blank
- Check backlight connection (5V to LED pin)
- Verify SPI wiring
- Try different display library
- Check pin definitions match hardware

### SD Card Not Detected
- Format SD card as FAT32
- Check SPI wiring
- Verify CS pin definition
- Try different SD card

---

## Safety Notes

- Install appropriate fuses on all 12V connections
- Do not short LoRa module (3.3V only!)
- Use marine-grade wire and connectors
- Follow ABYC standards for marine electrical
- This system supplements, not replaces, standard security
- Test alarm system regularly

---

## Next Steps

Once basic system is operational:

1. Fine-tune detection sensitivity for your environment
2. Add additional sensors (bilge, battery monitor, etc.)
3. Implement WiFi web interface on hub (future)
4. Integrate with chart plotter via NMEA
5. Add cellular/satellite alerting for remote monitoring

---

## Support Resources

- Arduino ESP32 Forum: https://esp32.com/
- RadioHead Documentation: http://www.airspayce.com/mikem/arduino/RadioHead/
- Adafruit Learning Guides: https://learn.adafruit.com/

---

**Congratulations! Your boat monitoring system is ready for deployment.**

**Fair winds!**
