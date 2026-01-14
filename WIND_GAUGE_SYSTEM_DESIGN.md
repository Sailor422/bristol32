# Bristol 32 Wind Gauge System Design
## Complete True Wind Speed & Direction Measurement System

**Version 1.0.0**  
**Date: November 16, 2025**  
**Target Vessel: Bristol 32 "Liberty"**

---

## Executive Summary

The Bristol 32 Wind Gauge System is a comprehensive marine wind measurement solution that calculates true wind speed and direction using vector mathematics. The system measures apparent wind parameters and accounts for boat movement using GPS and compass data to provide accurate true wind readings for sailing performance optimization.

**Key Features:**
- True wind speed and direction calculation using vector math
- Apparent wind speed and direction measurement
- GPS-based boat speed and course tracking
- Digital compass for boat heading
- ESP32-based design with LoRa wireless communication
- Marine-hardened construction for saltwater environments
- Real-time data transmission to central hub
- Integration with existing boat monitoring system

**System Cost:** $280-350 for complete installation  
**Power Consumption:** 150-250mA during operation  
**Accuracy:** ±1 knot wind speed, ±5° wind direction

---

## 1. System Architecture

### 1.1 Hardware Components

#### Wind Sensor Node:
- **Microcontroller:** ESP32-S3 development board
- **Wind Speed:** Anemometer with 3-cup rotor and analog output
- **Wind Direction:** Wind vane with potentiometer and analog output
- **GPS:** Ultimate GPS module for boat speed/course
- **Compass:** LSM303DLH accelerometer/magnetometer
- **Communication:** RFM95W LoRa transceiver
- **Power:** 12V marine power with buck converter

#### Integration with Existing Hub:
- Uses existing ESP32-S3 hub with touchscreen
- Compatible with current LoRa network (915MHz)
- Extends existing message protocol with wind data

### 1.2 Vector Mathematics for True Wind Calculation

True wind is calculated using the following vector equations:

```
Apparent Wind Vector = True Wind Vector - Boat Velocity Vector

Where:
- Apparent wind speed (Va) and direction (θa) are measured directly
- Boat speed (Vb) and heading (θb) come from GPS and compass
- True wind speed (Vt) and direction (θt) are calculated

Vector Components:
Va_x = Va × cos(θa)
Va_y = Va × sin(θa)
Vb_x = Vb × cos(θb)
Vb_y = Vb × sin(θb)

True Wind Components:
Vt_x = Va_x - Vb_x
Vt_y = Va_y - Vb_y

True Wind Speed and Direction:
Vt = √(Vt_x² + Vt_y²)
θt = atan2(Vt_y, Vt_x)
```

---

## 2. Hardware Implementation

### 2.1 Bill of Materials

| Component | Description | Quantity | Unit Price | Total |
|-----------|-------------|----------|------------|-------|
| ESP32-S3 Dev Board | Microcontroller | 1 | $12 | $12 |
| RFM95W LoRa Module | 915MHz transceiver | 1 | $7 | $7 |
| Anemometer | 3-cup wind speed sensor | 1 | $45 | $45 |
| Wind Vane | Wind direction sensor | 1 | $35 | $35 |
| Ultimate GPS | GPS module with antenna | 1 | $30 | $30 |
| LSM303 Compass | Accelerometer/magnetometer | 1 | $15 | $15 |
| Buck Converter | 12V to 5V regulator | 1 | $3 | $3 |
| LDO Regulator | 5V to 3.3V regulator | 1 | $0.50 | $0.50 |
| Marine Enclosure | IP65 waterproof box | 1 | $25 | $25 |
| Cable Glands | Waterproof cable entries | 4 | $1.50 | $6 |
| Marine Cable | 18AWG tinned copper | 10m | $0.50 | $5 |
| Connectors | Waterproof plugs/sockets | 1 set | $5 | $5 |
| **TOTAL** | | | | **$194.00** |

### 2.2 Sensor Specifications

#### Wind Speed Sensor (Anemometer):
- **Type:** 3-cup rotor with magnetic reed switch
- **Range:** 0.5-50 m/s (1-100 knots)
- **Accuracy:** ±1 m/s or 3% of reading
- **Start Speed:** 0.2 m/s
- **Output:** Analog voltage (0.4V-2.0V)
- **Resolution:** 0.1 m/s

#### Wind Direction Sensor (Vane):
- **Type:** Potentiometer-based wind vane
- **Range:** 0-360°
- **Accuracy:** ±5°
- **Resolution:** 22.5° (16-position)
- **Output:** Analog voltage (0-3.3V)
- **Damping:** Mechanical vane with counterweight

#### GPS Module:
- **Type:** MTK3339 chipset
- **Accuracy:** <3m position, 0.1 m/s velocity
- **Update Rate:** 1-10 Hz
- **Sensitivity:** -165 dBm tracking
- **Antenna:** Built-in patch + external SMA option

#### Compass Module:
- **Type:** LSM303DLH 3-axis magnetometer
- **Accuracy:** ±3° heading (after calibration)
- **Resolution:** 1°
- **Range:** ±1.3 to ±8.1 gauss
- **Update Rate:** 30-220 Hz

### 2.3 Power System

#### Input Requirements:
- **Voltage:** 12V marine electrical system
- **Current:** 150-250mA (peak 400mA)
- **Protection:** Inline fuse, reverse polarity diode
- **Filtering:** TVS diode for transients

#### Regulation:
- **Step-down:** LM2596 buck converter (12V → 5V)
- **LDO:** AMS1117-3.3 (5V → 3.3V)
- **Capacitance:** Multiple bypass capacitors (10µF, 100µF)
- **Efficiency:** >85% overall conversion efficiency

---

## 3. Software Implementation

### 3.1 Firmware Architecture

#### Main Components:
1. **Wind Sensor Reading** - Analog inputs for speed/direction
2. **GPS Data Processing** - NMEA parsing for speed/course
3. **Compass Calibration** - Hard/soft iron compensation
4. **Vector Calculations** - True wind computation
5. **LoRa Communication** - Data transmission to hub
6. **Data Logging** - Local storage and diagnostics

#### Task Scheduling:
- **Sensor Reading:** 10 Hz (100ms intervals)
- **GPS Processing:** Event-driven on NMEA receipt
- **Compass Update:** 30 Hz (33ms intervals)
- **True Wind Calc:** 10 Hz (100ms intervals)
- **LoRa Transmission:** 1 Hz (1 second intervals)
- **Data Logging:** 1 Hz (1 second intervals)

### 3.2 Message Protocol Extension

#### Wind Data Packet (16 bytes):
```
Byte 0:      Node ID (0x05 for wind sensor)
Byte 1:      Packet Type (0x22)
Byte 2-3:    Apparent Wind Speed (uint16, mm/s * 10)
Byte 4-5:    Apparent Wind Direction (uint16, degrees * 10)
Byte 6-7:    True Wind Speed (uint16, mm/s * 10)
Byte 8-9:    True Wind Direction (uint16, degrees * 10)
Byte 10-11:  Boat Speed (uint16, mm/s * 10)
Byte 12-13:  Boat Heading (uint16, degrees * 10)
Byte 14:     GPS Fix Quality (0-5)
Byte 15:     Checksum
```

### 3.3 Calibration Procedures

#### Wind Direction Calibration:
1. Point vane at magnetic north (0°)
2. Record ADC reading
3. Rotate 22.5° increments clockwise
4. Record all 16 positions
5. Store calibration table in EEPROM

#### Compass Calibration:
1. Rotate sensor in figure-8 pattern
2. Collect min/max magnetometer readings
3. Calculate hard/soft iron offsets
4. Store calibration data in EEPROM

#### Wind Speed Calibration:
1. Use known wind speed reference (anemometer)
2. Adjust voltage-to-speed scaling factor
3. Verify linearity across speed range

---

## 4. Installation and Mounting

### 4.1 Masthead Installation

#### Location Requirements:
- **Height:** Minimum 2m above deck level
- **Clearance:** 360° unobstructed view
- **Stability:** Rigid mounting to prevent vibration
- **Accessibility:** Easy access for maintenance

#### Mounting Hardware:
- **Mast Bracket:** Stainless steel U-bolt clamp
- **Sensor Arm:** 1m aluminum pole (1" OD)
- **Base:** Marine-grade aluminum mounting plate
- **Fasteners:** Stainless steel 316 hardware
- **Cable:** Waterproof conduit or flexible tubing

### 4.2 Cable Routing

#### Cable Types:
- **Power:** 18AWG marine-grade tinned copper
- **Signal:** Shielded twisted pair for analog signals
- **GPS Antenna:** RG-174 coaxial with SMA connectors
- **LoRa Antenna:** RG-58 with N-type connectors

#### Protection:
- **Conduit:** PVC or flexible metal conduit
- **Glands:** Waterproof cable entry points
- **Connectors:** Waterproof plugs and sockets
- **Labeling:** Heat-shrink tubing with labels

### 4.3 Commissioning Procedure

#### Hardware Setup:
1. Mount sensors at masthead
2. Route cables to electronics enclosure
3. Install ESP32 and sensors in enclosure
4. Connect antennas and power
5. Verify voltage levels and polarity

#### Software Configuration:
1. Upload firmware to ESP32
2. Set unique node ID (0x05)
3. Configure LoRa frequency/channel
4. Calibrate wind direction sensor
5. Calibrate compass module

#### System Testing:
1. Verify GPS satellite acquisition
2. Test wind speed response (fan test)
3. Test wind direction accuracy
4. Confirm LoRa communication range
5. Validate true wind calculations

---

## 5. Performance Specifications

### 5.1 Accuracy Specifications

| Parameter | Accuracy | Resolution | Range |
|-----------|----------|------------|-------|
| Apparent Wind Speed | ±1 knot / 3% | 0.1 knot | 0-100 knots |
| Apparent Wind Direction | ±5° | 1° | 0-360° |
| True Wind Speed | ±2 knots | 0.1 knot | 0-120 knots |
| True Wind Direction | ±10° | 1° | 0-360° |
| Boat Speed (GPS) | ±0.1 knot | 0.01 knot | 0-50 knots |
| Boat Heading | ±3° | 1° | 0-360° |

### 5.2 Environmental Specifications

#### Operating Conditions:
- **Temperature:** -10°C to +50°C
- **Humidity:** 0-100% RH (condensing)
- **Wind Speed:** 0-70 m/s survival
- **Vibration:** IEC 60945 marine standard
- **IP Rating:** IP65 (enclosure)

#### Storage Conditions:
- **Temperature:** -20°C to +70°C
- **Humidity:** <95% RH non-condensing

### 5.3 Reliability Specifications

#### MTBF (Mean Time Between Failures):
- **Electronic Components:** 50,000 hours
- **Mechanical Sensors:** 25,000 hours
- **Overall System:** 10,000 hours

#### Data Integrity:
- **Packet Delivery:** >99% within LoRa range
- **Data Accuracy:** >95% within specifications
- **Self-Diagnostics:** Automatic fault detection

---

## 6. Integration with Existing System

### 6.1 Hub Display Integration

#### Wind Data Display:
- **Apparent Wind:** Speed and direction
- **True Wind:** Speed and direction  
- **Boat Motion:** Speed, heading, course
- **GPS Status:** Satellite count, fix quality
- **Wind Trends:** Historical data graphs

#### Touchscreen Controls:
- **Calibration:** Initiate sensor calibration
- **Zero Reference:** Reset wind direction reference
- **Data Logging:** Start/stop data recording
- **Diagnostics:** System health monitoring

### 6.2 Data Logging

#### Log Format:
```
timestamp,node_id,apparent_speed,apparent_dir,true_speed,true_dir,boat_speed,boat_heading,gps_quality,battery_mv,rssi
2025-11-16 14:30:15,0x05,12.5,045,14.2,052,6.8,048,3,12450,-85
```

#### Storage Capacity:
- **Internal Flash:** 1MB for configuration/calibration
- **SD Card:** 16GB microSD for data logging
- **Retention:** 30 days at 1Hz sampling

### 6.3 Alarm Integration

#### Wind Speed Alarms:
- **Gust Alert:** Configurable wind speed threshold
- **Storm Warning:** Rapid pressure/wind changes
- **High Wind:** Sustained high wind conditions

#### System Alarms:
- **Sensor Failure:** Wind speed/direction sensor faults
- **GPS Loss:** Loss of satellite fix
- **Communication:** LoRa link failure
- **Power:** Low voltage warnings

---

## 7. Maintenance and Calibration

### 7.1 Routine Maintenance

#### Monthly Tasks:
- **Visual Inspection:** Check mounting hardware
- **Cable Inspection:** Verify connections and seals
- **Antenna Alignment:** Ensure GPS/LoRa antennas clear
- **Display Check:** Verify hub display functionality

#### Quarterly Tasks:
- **Sensor Cleaning:** Remove salt/debris from sensors
- **Calibration Check:** Verify wind direction accuracy
- **Battery Check:** Test GPS coin cell battery
- **Firmware Update:** Check for software updates

#### Annual Tasks:
- **Complete Calibration:** Full sensor recalibration
- **Component Replacement:** Replace worn mechanical parts
- **System Test:** End-to-end performance validation
- **Documentation:** Update maintenance records

### 7.2 Troubleshooting Guide

#### Common Issues:

**Inaccurate Wind Direction:**
- Cause: Loose potentiometer or contamination
- Solution: Clean contacts, recalibrate sensor

**GPS No Fix:**
- Cause: Poor antenna location or obstruction
- Solution: Relocate antenna, check cable connections

**Compass Drift:**
- Cause: Magnetic interference or calibration loss
- Solution: Recalibrate compass, remove magnetic sources

**LoRa Communication Loss:**
- Cause: Range issues or interference
- Solution: Check antenna connections, adjust frequency

---

## 8. Cost Analysis

### 8.1 Component Costs

| Category | Item | Cost |
|----------|------|------|
| **Microcontroller** | ESP32-S3 Dev Board | $12 |
| **Communication** | RFM95W LoRa Module | $7 |
| **Wind Sensors** | Anemometer + Wind Vane | $80 |
| **Navigation** | GPS Module + Compass | $45 |
| **Power** | Converters + Protection | $10 |
| **Enclosure** | Marine Box + Hardware | $30 |
| **Cabling** | Marine Cable + Connectors | $10 |
| **TOTAL HARDWARE** | | **$194** |

### 8.2 Installation Costs

| Category | Cost |
|----------|------|
| **Professional Installation** | $100-200 |
| **Mast Modifications** | $50-100 |
| **Cable Routing** | $25-50 |
| **Commissioning** | $50-100 |
| **TOTAL INSTALLATION** | **$225-450** |

### 8.3 Operating Costs

| Category | Annual Cost |
|----------|-------------|
| **Power Consumption** | $5-10 |
| **Maintenance** | $25-50 |
| **Calibration** | $50-100 |
| **TOTAL OPERATING** | **$80-160** |

---

## 9. Safety and Compliance

### 9.1 Marine Standards Compliance

#### Electrical Safety:
- **ABYC E-11:** Marine electrical systems
- **IEC 60945:** Maritime navigation equipment
- **IP65 Rating:** Dust and water protection

#### RF Compliance:
- **FCC Part 15:** Unlicensed radio operation
- **ISM Band:** 915MHz license-free operation
- **Power Limits:** <1W transmit power

### 9.2 Installation Safety

#### Working at Height:
- **Fall Protection:** Required for masthead work
- **Weather Conditions:** Calm weather only
- **Team Requirements:** Two-person installation

#### Electrical Safety:
- **Lockout/Tagout:** Power isolation procedures
- **Grounding:** Proper marine grounding practices
- **Cable Protection:** Strain relief and waterproofing

---

## 10. Conclusion

The Bristol 32 Wind Gauge System provides accurate true wind measurements essential for sailing performance optimization. By combining high-quality sensors with sophisticated vector mathematics, the system delivers reliable wind data that enhances safety and efficiency for liveaboard cruisers.

### Key Advantages:

- **Accuracy:** Precise true wind calculations using GPS/compass data
- **Integration:** Seamless compatibility with existing monitoring system
- **Reliability:** Marine-hardened design for harsh saltwater environments
- **Cost-Effective:** Affordable implementation with professional features
- **Maintainable:** Easy calibration and troubleshooting procedures

### Implementation Timeline:

1. **Week 1:** Component procurement and initial assembly
2. **Week 2:** Firmware development and sensor integration
3. **Week 3:** System testing and calibration
4. **Week 4:** Installation and commissioning

### Expected Performance:

- **Wind Speed Accuracy:** ±1 knot for apparent, ±2 knots for true
- **Wind Direction Accuracy:** ±5° for apparent, ±10° for true
- **Update Rate:** 1 Hz data transmission
- **Range:** 500-2000m LoRa communication
- **Reliability:** >99% uptime with proper maintenance

This wind gauge system transforms the Bristol 32 into a performance-oriented sailing platform with professional-grade wind measurement capabilities.

---

**Document Version:** 1.0.0  
**Last Updated:** November 16, 2025  
**Project Status:** Ready for Implementation  
**Estimated Completion Time:** 4-6 weeks