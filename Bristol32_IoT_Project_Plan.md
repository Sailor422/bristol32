# Bristol 32 IoT Sensor Network - Project Implementation Plan

## Executive Summary

This document outlines a comprehensive implementation plan for a complete IoT sensor network system for the Bristol 32 sailboat "Liberty". The system combines millimeter wave motion detection, environmental monitoring, and AI-powered task orchestration to create an intelligent marine monitoring platform.

**Project Goals:**
- Deploy a dual-node IoT network for comprehensive boat monitoring
- Implement motion detection and environmental sensing
- Create reliable data transmission via LoRa and internet gateway
- Build an AI agent orchestration system for automated task management
- Ensure system reliability for liveaboard cruising conditions

**Key Deliverables:**
- External mmWave sensor node with motion and environmental monitoring
- Internet gateway node for data transmission and system management
- AI agent orchestration system with 6 specialized agents
- Complete firmware implementation with PlatformIO
- Testing and validation across marine conditions

---

## 1. System Architecture Overview

### 1.1 Network Topology

```
┌─────────────────┐    LoRa    ┌─────────────────┐    WiFi/GSM    ┌─────────────┐
│   ext_mmwave    │◄──────────►│   int_gateway   │◄─────────────►│   Internet  │
│   Sensor Node   │            │   Gateway Node  │                │   Services  │
│                 │            │                 │                │             │
│ • mmWave Motion │            │ • Data Relay    │                │ • Email     │
│ • BME280 Env    │            │ • SMTP Client   │                │ • Monitoring│
│ • OLED Display  │            │ • System Mgmt   │                │ • AI Agents │
└─────────────────┘            └─────────────────┘                └─────────────┘
         │                             │
         └─────────────────────────────┘
          └─────────────────────────────┘
                (Feature Removed)
```

### 1.2 Hardware Architecture

#### **External Sensor Node (ext_mmwave)**
- **MCU**: Heltec WiFi LoRa 32 V2 (ESP32-based)
- **Sensors**:
  - Millimeter wave motion sensor (GPIO-based)
  - BME280 environmental sensor (I2C)
  - SSD1306 OLED display (I2C)
- **Communication**: LoRa transmitter
- **Power**: 12V marine system with voltage monitoring

#### **Internet Gateway Node (int_gateway)**
- **MCU**: Heltec WiFi LoRa 32 V2 (ESP32-based)
- **Communication**:
  - LoRa receiver/transceiver
  - WiFi (ESP32 built-in)
  - GSM module (future expansion)
- **Services**: SMTP email client for data transmission
- **Management**: System configuration and monitoring

### 1.3 Software Architecture

#### **Embedded Firmware Layer**
```
PlatformIO Project Structure:
├── ext_mmwave/           # External sensor node
│   └── src/
│       ├── main.cpp      # Motion detection & environmental monitoring
│       ├── mmwave_gpio.h # mmWave sensor interface
│       └── proto.h       # Communication protocol
├── int_gateway/          # Internet gateway node
│   └── src/
│       ├── main.cpp      # Data reception & transmission
│       ├── smtp_client.* # Email transmission
│       └── proto.h       # Communication protocol
└── lib/                  # Shared libraries
    └── common/
        ├── config.h      # System configuration
        ├── crc8.*        # Data integrity
        └── proto.*       # Communication protocol
```

#### **AI Orchestration Layer**
(Feature removed by user request 2026-01-13)


---

## 2. Component Implementation Details

### 2.1 External Sensor Node (ext_mmwave)

#### **Core Functionality**
- **Motion Detection**: Millimeter wave sensor monitoring with GPIO interrupts
- **Environmental Monitoring**: Temperature, humidity, pressure via BME280
- **Data Display**: Real-time readings on OLED display
- **Data Transmission**: LoRa packet transmission with error correction

#### **Key Implementation Files**

**main.cpp - Main Application Loop**
```cpp
// Core tasks
void task_motion(void*);     // Motion detection processing
void task_env(void*);       // Environmental sensor reading
void task_oled(void*);      // Display updates
void task_lora_tx(void*);   // Data transmission

// FreeRTOS task management
xTaskCreatePinnedToCore(task_motion, "motion", 4096, nullptr, 2, nullptr, 1);
xTaskCreatePinnedToCore(task_env, "env", 4096, nullptr, 1, nullptr, 1);
xTaskCreatePinnedToCore(task_oled, "oled", 4096, nullptr, 1, nullptr, 0);
xTaskCreatePinnedToCore(task_lora_tx, "lora", 4096, nullptr, 2, nullptr, 1);
```

**mmwave_gpio.h - Motion Sensor Interface**
```cpp
class MmwaveGPIO {
public:
    void begin();
    bool motionDetected();
    uint32_t getLastMotionTime();
    void setSensitivity(uint8_t level);
private:
    const uint8_t PIN_MMWAVE = 13;
    volatile bool motion_flag = false;
    uint32_t last_motion_time = 0;
};
```

### 2.2 Internet Gateway Node (int_gateway)

#### **Core Functionality**
- **LoRa Reception**: Receive sensor data packets
- **Data Processing**: Validate and format received data
- **Email Transmission**: SMTP client for data forwarding
- **System Management**: Configuration and status monitoring

#### **Key Implementation Files**

**main.cpp - Gateway Main Loop**
```cpp
// Core tasks
void task_lora_rx(void*);    // LoRa data reception
void task_data_process(void*); // Data validation and formatting
void task_email_tx(void*);   // Email transmission
void task_system_mgmt(void*); // System monitoring

// Communication setup
SX1276 radio = new Module(LORA_SS, LORA_DIO0, LORA_RST, LORA_BUSY);
SMTPClient smtp;
```

**smtp_client.* - Email Transmission**
```cpp
class SMTPClient {
public:
    bool connect(const char* server, uint16_t port);
    bool authenticate(const char* user, const char* pass);
    bool sendEmail(const char* to, const char* subject, const char* body);
    void disconnect();
private:
    WiFiClient client;
    String server;
    uint16_t port;
};
```

### 2.3 Communication Protocol

#### **Data Packet Structure**
```cpp
struct SensorData {
    uint16_t node_id;        // Unique node identifier (0xB032)
    uint32_t sequence;       // Packet sequence number
    uint32_t timestamp;      // Unix timestamp
    float temperature;       // °C
    float humidity;          // %
    float pressure;          // hPa
    bool motion_detected;    // Motion flag
    uint8_t battery_level;   // 0-100%
    uint8_t crc8;           // Error checking
};
```

#### **Protocol Features**
- **Error Correction**: CRC8 checksum validation
- **Sequence Tracking**: Prevent duplicate processing
- **Timestamp Sync**: UTC-based timing
- **Battery Monitoring**: Low-power awareness
- **Node Identification**: Multi-node support

### 2.4 AI Agent Orchestration System
(Feature removed)


---

## 3. Implementation Phases and Timeline

### Phase 1: Foundation Setup (Week 1-2)
**Goal**: Establish development environment and basic communication

#### **Week 1: Development Environment**
- [ ] Set up PlatformIO development environment
- [ ] Configure ESP32 toolchains and dependencies
- [ ] Test basic ESP32 compilation and flashing
- [ ] Set up version control and project structure
- [ ] Document hardware requirements and pinouts

#### **Week 2: Basic Communication**
- [ ] Implement LoRa communication between nodes
- [ ] Create basic packet structure and CRC validation
- [ ] Test point-to-point data transmission
- [ ] Implement basic error handling and retry logic
- [ ] Validate communication range and reliability

**Milestones:**
- Both nodes can exchange basic packets
- PlatformIO build system fully configured
- Basic debugging and monitoring in place

### Phase 2: Sensor Node Development (Week 3-5)
**Goal**: Complete external sensor node functionality

#### **Week 3: Motion Detection**
- [ ] Implement mmWave sensor GPIO interface
- [ ] Create motion detection algorithms
- [ ] Add motion event logging and timestamping
- [ ] Test motion detection accuracy and range
- [ ] Implement motion sensitivity adjustments

#### **Week 4: Environmental Monitoring**
- [ ] Integrate BME280 sensor (I2C communication)
- [ ] Implement temperature, humidity, pressure reading
- [ ] Add sensor calibration and error checking
- [ ] Create environmental data filtering and averaging
- [ ] Test sensor accuracy across temperature ranges

#### **Week 5: Display and User Interface**
- [ ] Implement OLED display driver
- [ ] Create real-time data display interface
- [ ] Add status indicators and system information
- [ ] Implement display power management
- [ ] Test display readability in various conditions

**Milestones:**
- Motion detection working with >95% accuracy
- Environmental sensors providing stable readings
- OLED display showing real-time data
- Node can operate for 24+ hours on battery

### Phase 3: Gateway Node Development (Week 6-8)
**Goal**: Complete internet gateway functionality

#### **Week 6: LoRa Reception**
- [ ] Implement LoRa receiver functionality
- [ ] Create packet reception and validation
- [ ] Add packet buffering and sequencing
- [ ] Implement reception statistics and error tracking
- [ ] Test reception reliability at various distances

#### **Week 7: Data Processing**
- [ ] Create data validation and formatting logic
- [ ] Implement data storage and queuing
- [ ] Add timestamp synchronization
- [ ] Create data compression for transmission
- [ ] Test data processing performance

#### **Week 8: Email Transmission**
- [ ] Implement SMTP client functionality
- [ ] Create email formatting and templates
- [ ] Add authentication and security
- [ ] Implement transmission queuing and retry
- [ ] Test email delivery reliability

**Milestones:**
- Gateway can receive and validate sensor data
- Email transmission working reliably
- End-to-end data flow functional
- System can operate autonomously

### Phase 4: AI Integration (Removed)
(Feature removed)

### Phase 5: Testing and Optimization (Week 11-12)
**Goal**: Comprehensive testing and performance optimization

#### **Week 11: System Testing**
- [ ] Conduct range testing for LoRa communication
- [ ] Test system reliability under various conditions
- [ ] Validate power consumption and battery life
- [ ] Test environmental sensor accuracy
- [ ] Perform stress testing and load analysis

#### **Week 12: Optimization and Documentation**
- [ ] Optimize code for memory and power efficiency
- [ ] Implement final error handling and recovery
- [ ] Create comprehensive documentation
- [ ] Prepare deployment and installation guides
- [ ] Final system validation and testing

**Milestones:**
- System tested in real marine environment
- All performance requirements met
- Complete documentation package
- Ready for production deployment

---

## 4. Hardware and Dependencies

### 4.1 Required Hardware

#### **Per Sensor Node:**
- Heltec WiFi LoRa 32 V2 board ($20-30)
- Millimeter wave motion sensor module ($10-15)
- BME280 environmental sensor ($5-10)
- SSD1306 OLED display (0.96" I2C) ($5-8)
- Antenna (LoRa 433/868/915MHz) ($5-10)
- Power supply/regulator (12V to 3.3V) ($5-10)

#### **Per Gateway Node:**
- Heltec WiFi LoRa 32 V2 board ($20-30)
- GSM module (optional, for cellular backup) ($15-25)
- External antenna (WiFi/GSM) ($5-10)
- Power supply/regulator ($5-10)

#### **Development Tools:**
- USB to UART programmer (CP2102/CH340) ($5-10)
- Multimeter and oscilloscope (for debugging)
- Logic analyzer (optional, for protocol debugging)

### 4.2 Software Dependencies

#### **PlatformIO Libraries:**
```ini
lib_deps =
  jgromes/RadioLib          ; LoRa communication
  mobizt/ESP Mail Client    ; Email transmission
  adafruit/Adafruit BME280 Library  ; Environmental sensor
  adafruit/Adafruit SSD1306 ; OLED display
  adafruit/Adafruit INA219  ; Power monitoring (future)
```

#### **Python Dependencies (AI System):**
```txt
anthropic>=0.7.0
google-generativeai>=0.3.0
openai>=1.0.0
ollama>=0.1.0
requests>=2.31.0
asyncio
```

---

## 5. Testing Strategy

### 5.1 Unit Testing
- **Embedded Code**: Test individual functions and modules
- **Communication**: Validate packet encoding/decoding
- **Sensor Integration**: Test sensor reading accuracy
- **AI Agents**: Test agent routing and response handling

### 5.2 Integration Testing
- **Node Communication**: End-to-end data flow testing
- **System Integration**: Complete sensor-to-gateway-to-cloud flow
- **AI Integration**: Agent orchestration and task completion
- **Power Management**: Battery life and power consumption testing

### 5.3 Environmental Testing
- **Temperature Range**: -10°C to +50°C operation
- **Humidity**: Marine environment (high humidity)
- **Vibration**: Engine and wave motion simulation
- **EMC**: Electromagnetic compatibility testing
- **Water Resistance**: IP65 rating validation

### 5.4 Performance Testing
- **Communication Range**: LoRa range testing (target 1-2km)
- **Data Throughput**: Packet transmission rates
- **Power Consumption**: Current draw analysis
- **Response Time**: Sensor-to-action latency
- **Reliability**: Uptime and error rate monitoring

---

## 6. Risk Analysis and Mitigation

### 6.1 Technical Risks

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|------------|
| LoRa communication interference | Medium | High | Frequency hopping, error correction, retransmission |
| Power consumption too high | Medium | Medium | Sleep modes, duty cycling, power optimization |
| ESP32 resource constraints | Low | Medium | Code optimization, memory management |
| Sensor accuracy degradation | Low | Medium | Calibration routines, error checking |
| AI agent integration complexity | Medium | Medium | Modular design, extensive testing |

### 6.2 Environmental Risks

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|------------|
| Marine corrosion | High | High | Marine-grade components, conformal coating |
| Water ingress | Medium | High | Sealed enclosures, IP65+ rating |
| Temperature extremes | Medium | Medium | Thermal management, tested ranges |
| Vibration damage | Medium | Medium | Shock mounting, robust construction |
| Electromagnetic interference | Low | Medium | Shielding, filtering, EMC compliance |

### 6.3 Project Risks

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|------------|
| Component availability | Low | Medium | Multiple suppliers, stock planning |
| Timeline delays | Medium | Medium | Phased approach, buffer time |
| Budget overruns | Low | Medium | Detailed planning, cost tracking |
| Technical skill gaps | Low | Medium | Documentation, training, expert consultation |

---

## 7. Success Metrics and Validation

### 7.1 Functional Requirements

- [ ] **Motion Detection**: >95% accuracy, <5 second response time
- [ ] **Environmental Monitoring**: ±0.5°C temperature, ±3% humidity accuracy
- [ ] **Communication**: Reliable LoRa transmission up to 1km range
- [ ] **Data Transmission**: Email delivery within 5 minutes of data receipt
- [ ] **Power Consumption**: 30+ days battery life with 200Ah system
- [ ] **System Reliability**: >99% uptime, automatic error recovery

### 7.2 Performance Metrics

- **Latency**: End-to-end sensor-to-email < 10 minutes
- **Range**: LoRa communication > 500 meters line-of-sight
- **Power**: < 50mA average current consumption
- **Memory**: < 70% ESP32 RAM utilization
- **Storage**: < 50% flash memory utilization

### 7.3 Quality Metrics

- **Code Coverage**: >80% unit test coverage
- **Documentation**: Complete user and developer guides
- **Maintainability**: Modular, well-commented code
- **Security**: Secure communication, authentication
- **Compliance**: Marine electrical standards compliance

---

## 8. Deployment and Maintenance

### 8.1 Installation Procedure

1. **Hardware Assembly**
   - Mount sensors and antennas securely
   - Connect power and communication wiring
   - Install in weatherproof enclosures

2. **Firmware Deployment**
   - Flash firmware via PlatformIO
   - Configure node-specific settings
   - Test communication and functionality

3. **System Configuration**
   - Set up email credentials and endpoints
   - Configure AI agent routing rules
   - Test end-to-end data flow

4. **Validation Testing**
   - Verify all sensors functioning
   - Test communication range
   - Validate data transmission
   - Check power consumption

### 8.2 Maintenance Schedule

- **Weekly**: Check system status via email alerts
- **Monthly**: Visual inspection, clean sensors
- **Quarterly**: Firmware updates, battery testing
- **Annually**: Complete system checkout, recalibration

### 8.3 Troubleshooting Guide

**Common Issues:**
- Communication failures: Check antenna connections, range
- Power issues: Verify voltage, current draw
- Sensor failures: Clean sensors, check connections
- Email delivery: Verify credentials, network connectivity

---

## 9. Budget Estimate

### 9.1 Hardware Costs (Per Complete System)

| Component | Quantity | Unit Cost | Total |
|-----------|----------|-----------|-------|
| Heltec WiFi LoRa 32 V2 | 2 | $25 | $50 |
| mmWave motion sensor | 1 | $12 | $12 |
| BME280 sensor | 1 | $7 | $7 |
| OLED display | 1 | $6 | $6 |
| Antennas | 2 | $7 | $14 |
| Power supplies | 2 | $8 | $16 |
| Enclosures | 2 | $15 | $30 |
| Cables/connectors | 1 | $10 | $10 |
| **Hardware Subtotal** | | | **$145** |

### 9.2 Software and Development

| Item | Cost |
|------|------|
| PlatformIO license | $0 (free) |
| Development tools | $50 |
| Testing equipment | $100 |
| AI API credits (initial) | $20 |
| **Software Subtotal** | **$170** |

### 9.3 Installation and Testing

| Item | Cost |
|------|------|
| Professional installation | $200 |
| Marine electrical inspection | $100 |
| Extended testing (1 week) | $300 |
| Documentation/commissioning | $150 |
| **Installation Subtotal** | **$750** |

### 9.4 Total Project Budget

- **Hardware**: $145
- **Software/Development**: $170
- **Installation/Testing**: $750
- **Contingency (20%)**: $213
- **Total**: **$1,278**

**Note**: Costs are estimates for DIY approach. Professional installation would increase total by $500-1,000.

---

## 10. Conclusion and Next Steps

### 10.1 Project Summary

This IoT sensor network project combines embedded systems development, wireless communication, and AI orchestration to create a comprehensive monitoring system for the Bristol 32 sailboat. The dual-node architecture provides reliable data collection and transmission while the AI system enables intelligent task automation.

### 10.2 Key Success Factors

1. **Modular Design**: Each component can be developed and tested independently
2. **Robust Communication**: LoRa provides reliable marine communication
3. **Power Efficiency**: Extended battery life critical for marine applications
4. **AI Integration**: Intelligent task routing enhances system capabilities
5. **Comprehensive Testing**: Marine environment requires thorough validation

### 10.3 Immediate Next Steps

1. **Week 1**: Set up development environment and test basic ESP32 functionality
2. **Week 2**: Implement and test LoRa communication between nodes
3. **Week 3**: Begin sensor integration (motion detection first)
4. **Week 4**: Continue with environmental sensors and display
5. **Week 5**: Complete sensor node and begin gateway development

### 10.4 Risk Mitigation Strategy

- **Technical Risks**: Prototyping and iterative testing
- **Timeline Risks**: Phased approach with buffer time
- **Budget Risks**: Detailed planning and vendor quotes
- **Knowledge Gaps**: Documentation and expert consultation

### 10.5 Long-term Vision

This project establishes a foundation for expanded marine IoT capabilities:
- Additional sensor types (wind, water level, security)
- Multiple node networks for larger vessels
- Integration with navigation systems
- Predictive maintenance algorithms
- Remote vessel monitoring and control

---

## Appendix A: Detailed Pinouts

### Heltec WiFi LoRa 32 V2 Pin Assignments

#### External Sensor Node (ext_mmwave):
```
GPIO 13: mmWave motion sensor input
GPIO 21: I2C SDA (BME280, OLED)
GPIO 22: I2C SCL (BME280, OLED)
GPIO 18: LoRa SPI SS
GPIO 19: LoRa SPI MISO
GPIO 23: LoRa SPI MOSI
GPIO 5:  LoRa SPI SCK
GPIO 14: LoRa RST
GPIO 26: LoRa DIO0
GPIO 35: LoRa DIO1
GPIO 32: LoRa BUSY
```

#### Internet Gateway Node (int_gateway):
```
GPIO 18: LoRa SPI SS
GPIO 19: LoRa SPI MISO
GPIO 23: LoRa SPI MOSI
GPIO 5:  LoRa SPI SCK
GPIO 14: LoRa RST
GPIO 26: LoRa DIO0
GPIO 35: LoRa DIO1
GPIO 32: LoRa BUSY
// WiFi built into ESP32
// GSM module on UART (GPIO 16/17) - future
```

## Appendix B: Communication Protocol Specification

### Packet Format (Binary, 32 bytes):
```
Bytes 0-1:  Node ID (0xB032)
Bytes 2-5:  Sequence number (uint32_t)
Bytes 6-9:  Timestamp (Unix, uint32_t)
Bytes 10-13: Temperature (float, IEEE 754)
Bytes 14-17: Humidity (float, IEEE 754)
Bytes 18-21: Pressure (float, IEEE 754)
Byte 22:    Motion flag (0/1)
Byte 23:    Battery level (0-100)
Byte 24:    Status flags (bitfield)
Bytes 25-30: Reserved
Byte 31:    CRC8 checksum
```

### Error Handling:
- CRC8 validation on all packets
- Sequence number tracking prevents duplicates
- Timeout handling for missed packets
- Retransmission on communication errors

---

**Document Version:** 1.0
**Date:** November 16, 2025
**Project:** Bristol 32 IoT Sensor Network
**Status:** Ready for Implementation</content>
<parameter name="filePath">Bristol32_IoT_Project_Plan.md