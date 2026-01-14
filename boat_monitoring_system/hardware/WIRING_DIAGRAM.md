# Wiring Diagrams - Boat Monitoring System

Detailed pin-by-pin wiring instructions for both node and hub.

---

## Node Wiring Diagram

### Components
- ESP32-S3-DevKitC-1 (or ESP32-WROOM-32)
- RFM95W LoRa Module (915/868/433 MHz)
- BME280 Environmental Sensor (I2C)
- HLK-LD2410 mmWave Human Detector (UART)
- 2.4" TFT Display ILI9341 (SPI)
- Piezo Buzzer (5V)
- 3x Push Buttons
- LM2596 Buck Converter (12V→5V)
- AMS1117 LDO Regulator (5V→3.3V)

---

### Power Supply Connections

```
12V BOAT POWER
    |
    +-- [Fuse 1A] -- [P6KE18A TVS] -- [Reverse Protection Diode]
                          |
                          v
                    +----------+
                    | LM2596   |  12V → 5V Buck Converter
                    | Buck     |
                    +----------+
                          |
                          v
                        5V Rail
                          |
                          +-- [100µF Cap] -- Display Backlight (LED pin)
                          +-- [10µF Cap]  -- mmWave Sensor VCC
                          |
                          v
                    +----------+
                    | AMS1117  |  5V → 3.3V LDO
                    | 3.3V LDO |
                    +----------+
                          |
                          v
                       3.3V Rail
                          |
                          +-- [100nF + 10µF Cap] -- ESP32 3V3
                          +-- [100nF + 10µF Cap] -- RFM95W VCC
                          +-- [100nF Cap] --------- BME280 VCC
                          +-- [100nF Cap] --------- Display VCC
```

**CRITICAL: RFM95W operates at 3.3V ONLY - never connect to 5V!**

---

### ESP32-S3 to RFM95W (LoRa) - SPI0

```
ESP32-S3          RFM95W
--------          ------
GPIO10    ---->   NSS (Chip Select)
GPIO11    ---->   MOSI
GPIO12    ---->   SCK
GPIO13    <----   MISO
GPIO9     <----   DIO0 (Interrupt)
GND       ---->   GND
3.3V      ---->   VCC

Optional:
GPIO14    <----   DIO1 (for advanced features)
```

**Antenna:** Solder 8.2cm wire to ANT pin (915MHz) or use SMA connector + antenna

---

### ESP32-S3 to BME280 (Environmental Sensor) - I2C

```
ESP32-S3          BME280
--------          ------
GPIO8     <---->  SDA
GPIO7     ---->   SCL
3.3V      ---->   VCC (or VIN)
GND       ---->   GND
```

**I2C Address:** 0x76 or 0x77 (check with I2C scanner if unsure)

**Pull-ups:** Internal pull-ups enabled in code, or add 4.7kΩ resistors to 3.3V

---

### ESP32-S3 to HLK-LD2410 (mmWave Detector) - UART

```
ESP32-S3          HLK-LD2410
--------          ----------
GPIO17    <----   TX (sensor transmits to ESP32 RX)
GPIO18    ---->   RX (ESP32 transmits to sensor RX)
5V        ---->   VCC (requires 5V, draws ~50mA)
GND       ---->   GND
```

**Note:** Most mmWave sensors need 5V power but use 3.3V logic (safe for ESP32)

---

### ESP32-S3 to ILI9341 Display (2.4" TFT) - SPI1

```
ESP32-S3          ILI9341
--------          -------
GPIO34    ---->   CS
GPIO33    ---->   DC (Data/Command)
GPIO21    ---->   RST (Reset)
GPIO35    ---->   MOSI (SDI)
GPIO36    ---->   SCK
GPIO37    <----   MISO (SDO) - optional, for touch
3.3V      ---->   VCC
5V        ---->   LED (Backlight) - via 100Ω resistor
GND       ---->   GND
```

**Backlight Control:**
- Direct: 5V → 100Ω resistor → LED pin
- PWM Control: GPIO16 → NPN transistor → LED pin (for brightness control)

**Touch (XPT2046):**
```
GPIO42    ---->   T_CS
GPIO1     <----   T_IRQ
```

---

### Buttons

```
ESP32-S3          Button          To
--------          ------          --
GPIO3     ---->   BTN_MODE        GND (use internal pull-up)
GPIO4     ---->   BTN_DISPLAY     GND (use internal pull-up)
GPIO5     ---->   BTN_SILENCE     GND (use internal pull-up)
```

**In code:** `pinMode(BTN_MODE, INPUT_PULLUP);`

**Hardware debouncing (optional):** Add 100nF capacitor from pin to GND

---

### Buzzer / Piezo

```
ESP32-S3          Component       To
--------          ---------       --
GPIO15    ---->   2N2222 Base (via 1kΩ resistor)
                  2N2222 Collector ---> Buzzer+ (Piezo or active buzzer)
                  2N2222 Emitter   ---> GND
                  Buzzer-          ---> 5V
```

**For Piezo Speaker:** Add 100Ω resistor in series to limit current

**For Active Buzzer:** Can drive directly if <20mA, otherwise use transistor

---

### Battery Voltage Monitor (Optional)

```
ESP32-S3          Divider
--------          -------
GPIO1     <----   Voltage divider output

Divider circuit:
Battery+ ----[ 10kΩ ]----+----[ 10kΩ ]---- GND
                         |
                         +---> GPIO1 (ADC)

Voltage = ADC_reading * (3.3V / 4095) * 2
```

---

### Complete Node Wiring Table

| ESP32 Pin | Function      | Connects To           | Signal Type |
|-----------|---------------|-----------------------|-------------|
| GPIO7     | I2C SCL       | BME280 SCL            | Output      |
| GPIO8     | I2C SDA       | BME280 SDA            | Bidir       |
| GPIO9     | LoRa INT      | RFM95W DIO0           | Input       |
| GPIO10    | LoRa CS       | RFM95W NSS            | Output      |
| GPIO11    | SPI MOSI      | RFM95W MOSI           | Output      |
| GPIO12    | SPI SCK       | RFM95W SCK            | Output      |
| GPIO13    | SPI MISO      | RFM95W MISO           | Input       |
| GPIO15    | Buzzer        | Transistor Base       | Output      |
| GPIO17    | UART RX       | mmWave TX             | Input       |
| GPIO18    | UART TX       | mmWave RX             | Output      |
| GPIO21    | Display RST   | ILI9341 RST           | Output      |
| GPIO33    | Display DC    | ILI9341 DC            | Output      |
| GPIO34    | Display CS    | ILI9341 CS            | Output      |
| GPIO35    | Display MOSI  | ILI9341 MOSI          | Output      |
| GPIO36    | Display SCK   | ILI9341 SCK           | Output      |
| GPIO37    | Display MISO  | ILI9341 MISO          | Input       |
| GPIO3     | Button        | BTN_MODE              | Input PU    |
| GPIO4     | Button        | BTN_DISPLAY           | Input PU    |
| GPIO5     | Button        | BTN_SILENCE           | Input PU    |
| GPIO1     | Battery ADC   | Voltage Divider       | Input       |

**PU = Internal Pull-Up enabled**

---

## Hub Wiring Diagram

### Components
- ESP32-S3-DevKitC-1
- RFM95W LoRa Module
- 3.5" or 4.3" TFT Display ILI9488
- microSD Card Module
- DS3231 RTC Module
- Speaker/Buzzer
- 3x Buttons or Keyswitch
- LM2596 Buck Converter
- 12V 7Ah Backup Battery (optional)

---

### Power Supply (Hub)

```
12V BOAT POWER ----+
                   |
              [Fuse 2A]
                   |
              [TVS Diode]
                   |
                   +----[Schottky Diode]----+
                   |                         |
                   |                    12V Backup
                   v                    Battery 7Ah
            +------------+                   |
            |  LM2596    |                   |
            | 12V → 5V   |<------------------+
            | 3A Buck    |
            +------------+
                   |
                   v
                 5V Rail (3A capable)
                   |
                   +-- Display Backlight
                   +-- SD Card Module
                   +-- Speaker/Buzzer
                   |
                   v
            +------------+
            |  AMS1117   |
            | 5V → 3.3V  |
            | 1A LDO     |
            +------------+
                   |
                   v
                3.3V Rail
                   |
                   +-- ESP32
                   +-- RFM95W
                   +-- RTC (can also use 5V)
```

**Backup Battery OR-ing:**
- Use two Schottky diodes (1N5819) to OR boat power and battery
- Battery charges through diode when boat power present
- Switches to battery automatically when boat power fails

---

### ESP32-S3 to RFM95W - SPI0 (Same as Node)

```
ESP32-S3          RFM95W
--------          ------
GPIO10    ---->   NSS (CS)
GPIO11    ---->   MOSI
GPIO12    ---->   SCK
GPIO13    <----   MISO
GPIO9     <----   DIO0
GND       ---->   GND
3.3V      ---->   VCC
```

---

### ESP32-S3 to ILI9488 Display (3.5") - SPI1

```
ESP32-S3          ILI9488
--------          -------
GPIO34    ---->   CS
GPIO35    ---->   MOSI
GPIO36    ---->   SCK
GPIO37    <----   MISO
GPIO33    ---->   DC
GPIO21    ---->   RST
5V        ---->   LED (Backlight via 100Ω)
3.3V      ---->   VCC
GND       ---->   GND
```

**Touchscreen (XPT2046 or FT6236):**
```
GPIO42    ---->   T_CS
GPIO1     <----   T_IRQ
```

---

### ESP32-S3 to microSD Card - SPI2

```
ESP32-S3          SD Card Module
--------          --------------
GPIO38    ---->   CS
GPIO39    ---->   SCK
GPIO40    ---->   MOSI
GPIO41    <----   MISO
5V        ---->   VCC (most modules have onboard regulator)
GND       ---->   GND
```

**Important:** Use short wires (<10cm) for reliable SD card communication

---

### ESP32-S3 to DS3231 RTC - I2C

```
ESP32-S3          DS3231
--------          ------
GPIO8     <---->  SDA
GPIO7     ---->   SCL
3.3V      ---->   VCC (or 5V, module has regulator)
GND       ---->   GND
```

**Battery:** Install CR2032 coin cell in RTC module for time retention when power off

**I2C Address:** 0x68 (fixed)

---

### Hub Buttons/Controls

```
ESP32-S3          Button/Switch   Function
--------          -------------   --------
GPIO3     ---->   BTN_ARM         Arm system (to GND, use pull-up)
GPIO4     ---->   BTN_DISARM      Disarm system
GPIO5     ---->   BTN_SILENCE     Silence alarm
```

**Alternative:** Use 3-position keyswitch (Disarm-Off-Arm)

---

### Speaker/Buzzer (Hub)

```
ESP32-S3          Audio
--------          -----
GPIO15    ---->   Amplifier input (or transistor base via 1kΩ)

For larger speaker:
  Use LM386 or similar audio amplifier
  Speaker: 8Ω, 0.5-1W
```

---

### Complete Hub Wiring Table

| ESP32 Pin | Function      | Connects To           | Signal Type |
|-----------|---------------|-----------------------|-------------|
| GPIO7     | I2C SCL       | RTC SCL               | Output      |
| GPIO8     | I2C SDA       | RTC SDA               | Bidir       |
| GPIO9     | LoRa INT      | RFM95W DIO0           | Input       |
| GPIO10    | LoRa CS       | RFM95W NSS            | Output      |
| GPIO11    | SPI0 MOSI     | RFM95W MOSI           | Output      |
| GPIO12    | SPI0 SCK      | RFM95W SCK            | Output      |
| GPIO13    | SPI0 MISO     | RFM95W MISO           | Input       |
| GPIO15    | Speaker       | Audio Amp             | Output      |
| GPIO21    | Display RST   | ILI9488 RST           | Output      |
| GPIO33    | Display DC    | ILI9488 DC            | Output      |
| GPIO34    | Display CS    | ILI9488 CS            | Output      |
| GPIO35    | SPI1 MOSI     | ILI9488 MOSI          | Output      |
| GPIO36    | SPI1 SCK      | ILI9488 SCK           | Output      |
| GPIO37    | SPI1 MISO     | ILI9488 MISO          | Input       |
| GPIO38    | SD CS         | SD Card CS            | Output      |
| GPIO39    | SPI2 SCK      | SD Card SCK           | Output      |
| GPIO40    | SPI2 MOSI     | SD Card MOSI          | Output      |
| GPIO41    | SPI2 MISO     | SD Card MISO          | Input       |
| GPIO42    | Touch CS      | Touch T_CS            | Output      |
| GPIO1     | Touch IRQ     | Touch T_IRQ           | Input       |
| GPIO3     | Button        | BTN_ARM               | Input PU    |
| GPIO4     | Button        | BTN_DISARM            | Input PU    |
| GPIO5     | Button        | BTN_SILENCE           | Input PU    |

---

## Common Wiring Tips

### Wire Gauges
- Power (12V, 5V, 3.3V): 18-22 AWG
- Signals (SPI, I2C, UART): 22-26 AWG

### Wire Colors (Suggested)
- Red: Positive power
- Black: Ground
- Yellow: SPI CLK
- Green: SPI MOSI
- Blue: SPI MISO
- White: SPI CS
- Orange: I2C SDA
- Brown: I2C SCL

### Decoupling Capacitors
Place close to power pins:
- 100nF ceramic (0.1µF) on each IC
- 10µF electrolytic on power rails
- 100µF electrolytic near voltage regulators

### Marine Environment
- Use tinned copper wire (corrosion resistant)
- Heat shrink all solder joints
- Apply conformal coating to PCB after assembly
- Use marine-grade crimp connectors
- Seal all enclosure penetrations

---

## Testing Checklist

### Before Powering On
- [ ] Check all power connections (no shorts)
- [ ] Verify 3.3V rail separate from 5V
- [ ] Confirm RFM95W on 3.3V (NOT 5V!)
- [ ] Check antenna connected
- [ ] Verify ground continuity

### Initial Power-Up
- [ ] Measure voltages: 12V, 5V, 3.3V
- [ ] Check for excessive heat
- [ ] Monitor current draw (should be <200mA initially)

### Functional Tests
- [ ] ESP32 boots (serial output)
- [ ] Display initializes
- [ ] LoRa module detected
- [ ] I2C devices detected (BME280, RTC)
- [ ] SD card mounts
- [ ] Buttons respond
- [ ] Buzzer sounds

---

## Troubleshooting

**Display shows garbage:**
- Check SPI wiring (MOSI, SCK, CS, DC)
- Verify correct voltage on VCC and LED pins
- Try different display library

**LoRa not working:**
- Verify antenna connected (won't work without)
- Check SPI connections
- Confirm 3.3V power (not 5V!)
- Verify frequency setting in code matches module

**SD card not detected:**
- Check CS pin assignment
- Verify SPI2 wiring
- Format card as FAT32
- Try different SD card

**I2C sensors not found:**
- Run I2C scanner to find addresses
- Check SDA/SCL not swapped
- Verify pull-up resistors (internal or external)
- Measure voltage on SDA/SCL (should be 3.3V when idle)

---

**Next:** See `hardware/BOM.csv` for complete parts list and ordering information.
