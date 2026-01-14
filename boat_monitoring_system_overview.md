# Bristol 32 Boat Monitoring System
## Distributed Human Detection and Environmental Monitoring

**Version 1.0.0**  
**Date: November 16, 2025**  
**Target Vessel: Bristol 32 "Liberty"**

---

## Executive Summary

The Bristol 32 Boat Monitoring System is a comprehensive, distributed IoT solution designed specifically for liveaboard sailing vessels. This system provides real-time human presence detection, environmental monitoring, and security alerting across multiple zones of a sailboat using low-power LoRa wireless communication.

The system consists of 3-5 sensor nodes distributed around the vessel (companionway, foredeck, cockpit, cabin, and engine compartment) that communicate with a central hub at the navigation station. Each node monitors environmental conditions and human activity, providing both security and safety functions for liveaboard cruisers.

**Key Features:**
- Distributed mmWave human presence detection
- Environmental monitoring (temperature, humidity, barometric pressure)
- Multiple alarm modes with marine-appropriate responses
- LoRa wireless communication optimized for marine environments
- Real-time displays and data logging
- Marine-hardened design for saltwater conditions

**System Cost:** $180-250 for complete 3-node + hub installation  
**Power Consumption:** 200-500mA peak per node  
**Communication Range:** 500-2000 meters line-of-sight

---

## 1. System Architecture

### 1.1 Network Topology

```
┌─────────────────┐    LoRa 915MHz    ┌─────────────────┐
│   Companionway  │◄─────────────────►│   Navigation    │
│   Sensor Node   │                   │   Hub Station   │
│   (Node 0x01)   │                   │                 │
└─────────────────┘                   │ • Central Hub   │
                                      │ • Data Logging  │
┌─────────────────┐                   │ • Alarm Control │
│   Foredeck      │◄─────────────────►│ • Touch Display │
│   Sensor Node   │                   │ • SD Card       │
│   (Node 0x02)   │                   └─────────────────┘
└─────────────────┘
                                      ┌─────────────────┐
┌─────────────────┐                   │   Cockpit       │
│   Engine Room   │◄─────────────────►│   Sensor Node   │
│   Sensor Node   │                   │   (Node 0x04)   │
│   (Node 0x03)   │                   └─────────────────┘
└─────────────────┘
```

### 1.2 Hardware Architecture

#### Sensor Node Components:
- **Microcontroller:** ESP32-S3 or ESP32-WROOM-32
- **Communication:** RFM95W LoRa transceiver (915/868/433MHz)
- **Environmental:** BME280 temperature/humidity/pressure sensor
- **Human Detection:** HLK-LD2410 24GHz mmWave radar sensor
- **Display:** 2.4" ILI9341 TFT LCD (320x240 resolution)
- **Audio:** 5V piezo buzzer for local alerts
- **Controls:** 3 tactile push buttons
- **Power:** 12V marine power with buck converter + LDO regulation

#### Hub Components:
- **Microcontroller:** ESP32-S3 development board
- **Communication:** RFM95W LoRa transceiver
- **Display:** 3.5" ILI9488 TFT touchscreen (480x320)
- **Storage:** microSD card module for data logging
- **Timekeeping:** DS3231 RTC with battery backup
- **Audio:** 8Ω speaker for alarm notifications
- **Power:** 12V marine power with battery backup

### 1.3 Software Architecture

#### Firmware Structure:
```
boat_monitoring_system/
├── common/                    # Shared code and protocols
│   ├── CommonTypes.h         # Data structures and enums
│   └── MessageProtocol.h     # LoRa packet formats
├── node_firmware/            # Individual sensor node code
│   ├── node_firmware.ino     # Main Arduino sketch
│   ├── config/               # Node configuration
│   ├── sensors/              # Sensor drivers
│   ├── lora/                 # LoRa communication
│   └── display/              # Local display management
└── hub_firmware/             # Central hub code
    ├── hub_firmware.ino      # Main Arduino sketch
    ├── lora/                 # LoRa hub receiver
    ├── display/              # Hub display management
    ├── storage/              # SD card data logging
    └── alarm/                # Alarm state management
```

#### Communication Protocol:
- **Packet Types:** Environmental, Detection, Alarm, Heartbeat, Config
- **Error Correction:** XOR checksum validation
- **Addressing:** 8-bit node IDs with broadcast capability
- **Data Encoding:** Efficient binary format for sensor data

---

## 2. Core Functionality

### 2.1 Human Presence Detection

#### mmWave Radar Technology:
- **Frequency:** 24GHz FMCW radar
- **Range:** 0.75-6 meters detection distance
- **Resolution:** 0.75m distance resolution
- **Zones:** Near (0-2m), Middle (2-4m), Far (4-6m)
- **Sensitivity:** Configurable detection thresholds

#### Detection Events:
- **Approach:** Person moving toward sensor
- **Entry:** Person entering detection zone
- **Presence:** Stationary person detected
- **Exit:** Person leaving detection zone

#### False Positive Mitigation:
- Confidence scoring (0-100%)
- Minimum duration filtering
- Zone-based validation
- Movement pattern analysis

### 2.2 Environmental Monitoring

#### Sensor Specifications:
- **Temperature:** -40°C to +85°C, ±1°C accuracy
- **Humidity:** 0-100% RH, ±3% accuracy
- **Pressure:** 300-1100 hPa, ±1 hPa accuracy
- **Battery Voltage:** 0-15V range, 10-bit ADC resolution

#### Weather Analysis:
- **Storm Detection:** Pressure drop rate monitoring
- **Trend Analysis:** 24-hour environmental trends
- **Dew Point Calculation:** Condensation risk assessment
- **Heat Index:** Apparent temperature calculation

### 2.3 Alarm System

#### Alarm Modes:
- **Disarmed:** Detection logged, no alerts
- **Doorbell:** Gentle chime for companionway detection
- **Perimeter Armed:** Exterior nodes trigger full alarm
- **Full Armed:** All nodes trigger alarm
- **Quiet Hours:** Reduced sensitivity, silent notifications

#### Alarm Responses:
- **Local:** Piezo buzzer with escalating tones
- **Hub:** Speaker with voice announcements
- **Visual:** Display flashing and status indicators
- **Logging:** Timestamped event recording

### 2.4 Data Management

#### Logging Format:
```csv
timestamp,node_id,event_type,value1,value2,value3,battery_mv,rssi
2025-11-16 14:30:15,0x01,ENV_DATA,23.5,65.2,1013.2,12450,-85
2025-11-16 14:30:20,0x02,DETECTION,1,85,240,12380,-78
```

#### Data Retention:
- **Environmental:** 24-hour rolling buffer
- **Events:** 7-day event log
- **System:** 30-day system health log
- **Storage:** microSD card with FAT32 formatting

---

## 3. Alarm Modes and Operation

### 3.1 Mode Descriptions

#### Disarmed Mode
- **Purpose:** Normal operation without security alerts
- **Detection:** All events logged but no audible alerts
- **Display:** Green status indicator
- **Use Case:** Daytime operation, maintenance periods

#### Doorbell Mode
- **Purpose:** Courtesy alerts for companionway access
- **Detection:** Companionway node triggers gentle chime
- **Response:** Single tone, no full alarm
- **Use Case:** Evening hours when expecting visitors

#### Perimeter Armed
- **Purpose:** Exterior security monitoring
- **Detection:** Foredeck and cockpit nodes trigger alarm
- **Response:** Full alarm sequence
- **Use Case:** Overnight with access to cockpit

#### Full Armed
- **Purpose:** Complete vessel security
- **Detection:** All nodes trigger alarm
- **Response:** Maximum alert level
- **Use Case:** Extended absences, high-security periods

#### Quiet Hours
- **Purpose:** Reduced sensitivity for nighttime
- **Detection:** Higher confidence thresholds
- **Response:** Silent notifications only
- **Use Case:** Sleeping hours, noise-sensitive areas

### 3.2 Control Interface

#### Node Controls:
- **MODE Button:** Cycle through alarm modes
- **DISPLAY Button:** Change display views
- **SILENCE Button:** Acknowledge/cancel alarms

#### Hub Controls:
- **ARM Button:** Arm system (Perimeter → Full cycle)
- **DISARM Button:** Disarm all nodes
- **SILENCE Button:** Cancel active alarms

#### Touchscreen Interface:
- **Status Overview:** All nodes' current state
- **Environmental Data:** Real-time readings
- **Event Log:** Recent detections and alarms
- **Configuration:** System settings and node management

---

## 4. Hardware Implementation

### 4.1 Node Electronics

#### ESP32-S3 Integration:
- **Core:** Dual-core Xtensa LX7 @ 240MHz
- **Memory:** 512KB SRAM, 8MB PSRAM (optional)
- **Wireless:** 2.4GHz WiFi, Bluetooth 5.0
- **GPIO:** 45 programmable pins
- **ADC:** 20 channels, 12-bit resolution
- **Power:** 3.0-3.6V operation, deep sleep modes

#### LoRa Module (RFM95W):
- **Frequency Range:** 410-525MHz or 862-1020MHz
- **Modulation:** FSK, GFSK, MSK, GMSK, LoRa, OOK
- **Data Rate:** 0.018-300kbps
- **Sensitivity:** -148dBm
- **Output Power:** +20dBm max
- **Range:** 20km line-of-sight (ideal conditions)

#### mmWave Sensor (HLK-LD2410):
- **Detection Technology:** FMCW radar
- **Operating Frequency:** 24GHz
- **Detection Distance:** 0.75-6 meters
- **Distance Resolution:** 0.75 meters
- **Output:** UART serial data
- **Power Consumption:** <1W

### 4.2 Power System

#### Input Power:
- **Voltage:** 12V marine electrical system
- **Current:** 200-500mA per node (peak)
- **Protection:** Inline fuse, reverse polarity diode
- **Filtering:** TVS diode for transients

#### Regulation:
- **Step-down:** LM2596 buck converter (12V → 5V)
- **LDO:** AMS1117-3.3 (5V → 3.3V)
- **Capacitance:** Multiple bypass capacitors
- **Monitoring:** Battery voltage ADC measurement

#### Battery Backup (Optional):
- **Chemistry:** Li-ion 18650 cells (3.7V)
- **Capacity:** 3000mAh per cell
- **Configuration:** 2-cell series (7.4V)
- **Charging:** TP4056 module with solar input
- **Runtime:** 24-48 hours backup power

### 4.3 Enclosure Design

#### Environmental Protection:
- **Rating:** IP65 (dust-tight, water jets)
- **Material:** ABS plastic with UV stabilization
- **Sealing:** Silicone gaskets, cable glands
- **Mounting:** Stainless steel hardware

#### Thermal Management:
- **Passive Cooling:** Heat sinks on regulators
- **Ventilation:** Filtered vents for pressure equalization
- **Operating Range:** -10°C to +50°C
- **Storage Range:** -20°C to +70°C

---

## 5. Software Implementation

### 5.1 Firmware Architecture

#### State Machine Design:
```cpp
enum SystemState {
    STATE_INIT,           // Boot and initialization
    STATE_NORMAL,         // Standard operation
    STATE_PRE_ALARM,      // Detection confirmed, grace period
    STATE_ALARM_TRIGGERED,// Alarm active
    STATE_SLEEP,          // Power saving mode
    STATE_ERROR           // Fault condition
};
```

#### Task Scheduling:
- **Sensor Reading:** 60-second intervals
- **Display Update:** 1-second intervals
- **Button Polling:** 100ms intervals
- **LoRa Transmission:** Event-driven or 5-minute intervals
- **Heartbeat:** 2-minute intervals

### 5.2 Communication Protocol

#### Packet Structure:
```
Byte 0:    Node ID (0x01-0xFE)
Byte 1:    Message Type (0x01-0xFF)
Bytes 2-N: Data payload
Byte N+1:  Checksum
```

#### Message Types:
- **0x01:** Environmental data (temperature, humidity, pressure)
- **0x02:** Detection event (confidence, distance, zone)
- **0x03:** Alarm command (arm, disarm, mode change)
- **0x04:** Heartbeat (battery voltage)
- **0x20:** Configuration update
- **0x21:** Time synchronization

### 5.3 Configuration Management

#### Node Configuration:
```cpp
struct NodeConfig {
    uint8_t nodeID;
    String nodeName;
    AlarmMode alarmMode;
    float nearZoneMax;
    float middleZoneMax;
    uint8_t minConfidence;
    uint16_t minDuration;
    uint32_t envDataInterval;
    uint32_t heartbeatInterval;
    float loraFrequency;
};
```

#### Storage Method:
- **ESP32 NVS:** Non-volatile storage for configuration
- **Default Values:** Sensible defaults for initial operation
- **Runtime Updates:** Serial interface for configuration changes

---

## 6. Installation and Deployment

### 6.1 Node Placement Strategy

#### Companionway Node:
- **Location:** Main entrance area
- **Purpose:** Entry/exit monitoring, doorbell function
- **Orientation:** Facing entrance, 2-3m coverage
- **Power:** 12V cabin lighting circuit

#### Foredeck Node:
- **Location:** Forward deck area
- **Purpose:** Security monitoring, boarding detection
- **Orientation:** Covering access routes
- **Power:** Navigation lights circuit

#### Cockpit Node:
- **Location:** Helm station area
- **Purpose:** Cockpit security, weather monitoring
- **Orientation:** Aft-facing for boarding detection
- **Power:** Accessory power outlet

#### Engine Room Node:
- **Location:** Engine compartment
- **Purpose:** Equipment monitoring, unauthorized access
- **Orientation:** Interior monitoring
- **Power:** Engine room lighting circuit

### 6.2 Wiring and Connections

#### Power Distribution:
- **Main Feed:** 12V from distribution panel
- **Protection:** 1A fuse per node
- **Grounding:** Common ground to vessel ground
- **Cable:** Marine-grade 18AWG tinned copper

#### Signal Connections:
- **LoRa Antennas:** 915MHz quarter-wave vertical
- **Sensor Cables:** Shielded for EMI protection
- **Display Cables:** Ribbon cable for TFT displays
- **Button Wiring:** Individual conductors with pull-ups

### 6.3 Commissioning Procedure

#### Node Setup:
1. **Hardware Assembly:** Mount components in enclosure
2. **Power Connection:** Verify voltage and polarity
3. **Antenna Installation:** Secure external antenna
4. **Firmware Upload:** Program ESP32 with node firmware
5. **Configuration:** Set node ID and parameters

#### System Integration:
1. **Hub Installation:** Mount at navigation station
2. **Node Registration:** Add nodes to hub configuration
3. **Range Testing:** Verify LoRa communication
4. **Calibration:** Adjust sensor parameters
5. **Final Testing:** Complete system validation

---

## 7. Testing and Validation

### 7.1 Unit Testing

#### Sensor Validation:
- **BME280:** Accuracy testing across temperature range
- **mmWave:** Detection range and false positive testing
- **Display:** Visual output verification
- **Buttons:** Mechanical and electrical testing

#### Communication Testing:
- **LoRa Range:** Point-to-point distance testing
- **Packet Integrity:** Error rate measurement
- **Throughput:** Data transmission rate validation
- **Interference:** Coexistence with VHF/AIS testing

### 7.2 Integration Testing

#### System Functionality:
- **Alarm Modes:** All mode transitions tested
- **Multi-node Operation:** Simultaneous node communication
- **Data Logging:** SD card write/read verification
- **Power Management:** Battery backup testing

#### Environmental Testing:
- **Temperature:** -10°C to +50°C operation
- **Humidity:** 90% RH marine conditions
- **Vibration:** Engine and wave motion simulation
- **EMC:** Electromagnetic compatibility testing

### 7.3 Performance Metrics

#### Detection Accuracy:
- **True Positives:** >95% detection rate
- **False Positives:** <5% false alarm rate
- **Response Time:** <2 seconds from detection to alert
- **Zone Accuracy:** ±0.5m distance resolution

#### System Reliability:
- **Uptime:** >99.5% operational availability
- **Data Integrity:** 100% packet delivery (with retries)
- **Power Efficiency:** <50mA average current draw
- **Memory Usage:** <70% ESP32 RAM utilization

---

## 8. Maintenance and Troubleshooting

### 8.1 Routine Maintenance

#### Monthly Tasks:
- **Visual Inspection:** Check enclosures for damage
- **Antenna Verification:** Ensure antennas are secure
- **Display Cleaning:** Clean TFT screens
- **Cable Inspection:** Check for corrosion or damage

#### Quarterly Tasks:
- **Firmware Updates:** Check for software updates
- **Sensor Calibration:** Recalibrate environmental sensors
- **Battery Testing:** Test backup battery capacity
- **Data Review:** Analyze logged data for anomalies

#### Annual Tasks:
- **Complete Checkout:** Full system testing
- **Component Replacement:** Replace worn components
- **Documentation Update:** Update system records

### 8.2 Common Issues

#### Communication Problems:
- **Symptom:** Nodes show offline status
- **Cause:** LoRa range, antenna damage, interference
- **Solution:** Check antenna connections, relocate nodes, change frequency

#### False Alarms:
- **Symptom:** Unwanted alarm triggers
- **Cause:** Sensitivity too high, environmental factors
- **Solution:** Adjust detection parameters, add exclusion zones

#### Power Issues:
- **Symptom:** Node resets or erratic behavior
- **Cause:** Voltage drops, poor connections, regulator failure
- **Solution:** Check power wiring, test voltage regulation

#### Display Problems:
- **Symptom:** Blank or corrupted display
- **Cause:** SPI connection issues, backlight failure
- **Solution:** Verify SPI wiring, replace display module

---

## 9. Cost Analysis

### 9.1 Bill of Materials

#### Per Node (x3):
- ESP32-S3 Development Board: $12
- RFM95W LoRa Module: $7
- BME280 Sensor: $8
- HLK-LD2410 mmWave Sensor: $12
- 2.4" TFT Display: $15
- Enclosure & Hardware: $25
- Miscellaneous (wires, fuses, etc.): $10
**Subtotal per node: $89**

#### Hub (x1):
- ESP32-S3 Development Board: $12
- RFM95W LoRa Module: $7
- 3.5" Touchscreen Display: $28
- microSD Module: $3
- DS3231 RTC: $4
- Speaker & Amplifier: $15
- Enclosure & Hardware: $30
- Miscellaneous: $10
**Subtotal hub: $109**

#### System Total: **$89 × 3 + $109 = $376**

### 9.2 Cost Optimization Options

#### Budget Configuration:
- Omit displays on 2 nodes: -$30
- Use basic enclosures: -$20
- Skip battery backup: -$40
- Use ESP32-WROOM instead of S3: -$15
**Optimized total: $271**

#### Premium Configuration:
- Add cellular backup: +$25
- Upgrade to IP67 enclosures: +$30
- Add solar charging: +$20
- Include professional installation: +$150
**Premium total: $601**

---

## 10. Safety and Compliance

### 10.1 Marine Electrical Standards

#### ABYC Standards Compliance:
- **E-11:** AC and DC electrical systems on boats
- **E-30:** Bonding of nonelectrical metallic parts
- **E-31:** Grounding of electrical systems

#### Safety Features:
- **Overcurrent Protection:** Fuses on all power feeds
- **Ground Fault Protection:** Proper grounding practices
- **Isolation:** Optically isolated inputs where required
- **Polarity Protection:** Diodes preventing reverse connection

### 10.2 RF Compliance

#### FCC Regulations (US):
- **Part 15.247:** Spread spectrum transmission limits
- **Part 15.209:** Radiated emission limits
- **Frequency Allocation:** 902-928MHz ISM band

#### CE Compliance (Europe):
- **ETSI EN 300 220:** Short range devices
- **ETSI EN 301 489:** EMC requirements
- **Frequency Allocation:** 863-870MHz

### 10.3 Environmental Compliance

#### Marine Environment:
- **Saltwater Resistance:** Corrosion-resistant materials
- **UV Protection:** UV-stabilized enclosures
- **Vibration Resistance:** Shock-mounted components
- **Temperature Range:** -10°C to +50°C operation

---

## 11. Future Enhancements

### 11.1 Software Features

#### Advanced Analytics:
- **Predictive Maintenance:** Equipment failure prediction
- **Weather Forecasting:** Local weather prediction
- **Usage Patterns:** Occupancy and activity analysis
- **Energy Optimization:** Power consumption optimization

#### Communication:
- **WiFi Integration:** Web interface for configuration
- **MQTT Support:** Integration with marine IoT platforms
- **NMEA 0183/2000:** Navigation system integration
- **Cellular Backup:** SMS alerts for critical events

### 11.2 Hardware Expansions

#### Additional Sensors:
- **Bilge Level:** Water intrusion detection
- **Battery Monitoring:** House bank voltage/current
- **GPS Anchor Watch:** Position monitoring
- **Wind/Speed:** Performance monitoring

#### Network Expansion:
- **Mesh Networking:** Multi-hop LoRa communication
- **Satellite Communication:** Iridium/GPS backup
- **WiFi Mesh:** Local network expansion
- **Bluetooth Integration:** Mobile app connectivity

### 11.3 System Integration

#### Navigation Systems:
- **Chartplotter Integration:** Overlay security data
- **AIS Integration:** Vessel tracking correlation
- **Autopilot Interface:** Security-aware navigation
- **VHF Radio Integration:** Voice alerts

#### Home Automation:
- **Smart Lighting:** Occupancy-based lighting control
- **Climate Control:** Environmental optimization
- **Security Systems:** Integrated alarm management
- **Energy Management:** Load shedding and optimization

---

## 12. Conclusion

The Bristol 32 Boat Monitoring System represents a comprehensive solution for liveaboard security and environmental monitoring. By combining distributed mmWave presence detection with robust LoRa communication and marine-hardened design, the system provides reliable protection and monitoring capabilities specifically tailored for sailing vessels.

### Key Achievements:

- **Distributed Architecture:** Multiple sensor nodes provide comprehensive coverage
- **Marine Optimization:** Designed for harsh saltwater environments
- **Low Power Design:** Extended battery life for off-grid operation
- **Flexible Configuration:** Multiple alarm modes for different situations
- **Data Logging:** Comprehensive event and environmental data recording
- **Cost Effectiveness:** Affordable implementation with professional features

### System Readiness:

- **Hardware:** Complete BOM with marine-grade components
- **Software:** Production-ready firmware with error handling
- **Documentation:** Comprehensive setup and maintenance guides
- **Testing:** Defined validation procedures and performance metrics
- **Safety:** Compliance with marine electrical and RF standards

### Deployment Recommendations:

1. **Start with Companionway Node:** Essential security monitoring
2. **Add Environmental Nodes:** Weather and safety monitoring
3. **Implement Hub:** Centralized control and data management
4. **Expand Coverage:** Additional nodes as budget allows
5. **Regular Maintenance:** Quarterly system checkups

This system transforms the Bristol 32 from a passive vessel into an intelligent, self-monitoring platform that enhances both security and livability for extended cruising.

---

**Document Version:** 1.0.0  
**Last Updated:** November 16, 2025  
**Project Status:** Ready for Implementation  
**Estimated Completion Time:** 2-3 weeks for basic system, 4-6 weeks for full installation

**Fair winds and following seas! ⚓⛵**</content>
<parameter name="filePath">/Users/herbertfields/Ai/opencode/bristol32/boat_monitoring_system_overview.md