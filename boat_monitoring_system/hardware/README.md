# Hardware Documentation

Complete hardware design files, schematics, wiring diagrams, and bill of materials for the boat monitoring system.

## Contents

### 📋 Bill of Materials
**`BOM.csv`** - Complete parts list with pricing and suppliers
- Itemized list for 3 nodes + 1 hub
- Current pricing (~$480 total)
- Budget alternatives (<$360)
- Supplier links and part numbers
- Optional components clearly marked

### 🔌 Wiring Diagrams
**`WIRING_DIAGRAM.md`** - Detailed pin-by-pin connections
- Complete node wiring (ESP32, LoRa, sensors, display)
- Complete hub wiring (ESP32, LoRa, SD card, RTC, display)
- Power supply schematics
- Color-coded wire recommendations
- Testing checklist
- Troubleshooting guide

### 🖥️ KiCad Schematics
**`kicad/`** - Importable KiCad project files

**Node Project:**
- `kicad/node/node.kicad_pro` - KiCad project file
- `kicad/node/node.kicad_sch` - Schematic with component placement

**Hub Project:**
- `kicad/hub/hub.kicad_pro` - KiCad project file
- `kicad/hub/hub.kicad_sch` - Schematic with component placement

**Format:** KiCad 6.0+ S-expression format

## Using the KiCad Files

### Opening in KiCad

1. **Install KiCad 6.0 or later**
   - Download from https://www.kicad.org/download/
   - Free and open source

2. **Open the Project**
   ```
   File → Open Project → Select node.kicad_pro or hub.kicad_pro
   ```

3. **View Schematic**
   - Click "Schematic Editor" icon
   - Schematics will open automatically

### What's Included in Schematics

The KiCad schematics show:
- Component symbols (ESP32, LoRa, sensors)
- Pin assignments and connections
- Power supply topology
- Signal routing
- Net labels for easy reference
- Title block with project info

**Note:** These are functional schematics for reference. Full symbol libraries not included - you may need to assign footprints for PCB design.

### Creating a PCB (Advanced)

To create a custom PCB from these schematics:

1. **Assign Footprints**
   - Tools → Assign Footprints
   - Match each component to physical package

2. **Update PCB from Schematic**
   - Tools → Update PCB from Schematic

3. **Route PCB**
   - Place components
   - Auto-route or manual routing
   - Add ground plane
   - Design rule check (DRC)

4. **Generate Gerbers**
   - File → Plot
   - Select Gerber format
   - Upload to PCB manufacturer (JLCPCB, OSH Park, etc.)

**Recommended PCB specs:**
- 2-layer board
- 1.6mm thickness
- HASL or ENIG finish (ENIG better for marine)
- Green or blue solder mask

## Assembly Options

### Option 1: Breadboard Prototype (Recommended First)
- Use jumper wires
- No soldering required
- Easy to modify and debug
- Follow WIRING_DIAGRAM.md

### Option 2: Perfboard Build
- Solder components on perfboard/stripboard
- More permanent than breadboard
- Lower cost than custom PCB
- Good for low-quantity builds

### Option 3: Custom PCB
- Use KiCad schematics as starting point
- Design custom PCB layout
- Order from manufacturer
- Professional appearance
- Better for multiple builds

## Hardware Tips

### Component Selection

**ESP32 Boards:**
- **ESP32-S3-DevKitC-1** - Recommended (newest, USB-C)
- **ESP32-WROOM-32** - Budget option (works fine)
- **ESP32-C3** - Lower cost, fewer pins (requires pin reassignment)

**LoRa Modules:**
- **RFM95W** - Best price/performance
- **Hope RFM95** - Same chip, different brand
- **915 MHz** for US/Americas
- **868 MHz** for Europe
- **433 MHz** for Asia/Pacific

**Displays:**
- **ILI9341** (2.4") - Good for nodes, cheap ($12-18)
- **ILI9488** (3.5") - Good for hub, touchscreen (~$28)
- **SSD1306 OLED** (0.96") - Minimal/budget option ($3-5)

**mmWave Sensors:**
- **HLK-LD2410** - Recommended, good docs (~$12)
- **DFRobot SEN0395** - Better filtering, more expensive ($15)
- **Seeed MR60BHA1** - 60GHz, high precision ($18)

### Power Considerations

**Node Power Options:**

1. **12V Wired** (Recommended for most)
   - Tap into boat's 12V system
   - Most reliable
   - No battery maintenance

2. **Battery + Solar**
   - 2x 18650 Li-ion (6000mAh)
   - 5V 1W solar panel
   - TP4056 charger module
   - Good for hard-to-wire locations

3. **Battery Only**
   - Same as #2 without solar
   - Requires periodic charging
   - 12-25 days runtime per charge

**Hub Power:**
- Always use 12V wired
- Optional: Add 12V 7Ah backup battery
- Expect ~350mA @ 5V draw (1.75W)

### Marine Environment Protection

**Essential:**
1. Conformal coating on all PCBs
2. IP65+ rated enclosures for exterior nodes
3. PG7 cable glands for waterproof entry
4. Marine-grade tinned copper wire
5. Heat shrink all solder joints

**Recommended:**
6. Corrosion-X or similar on connectors
7. Silicone sealant on all mounting screws
8. Anti-vibration mounting (rubber grommets)
9. UV-stable enclosures (white or light color)
10. Label all wires and components

### Testing Equipment Needed

**Minimum:**
- Multimeter (voltage, continuity)
- USB cable for programming
- Smartphone (for WiFi testing later)

**Recommended:**
- Oscilloscope (for debugging SPI issues)
- Logic analyzer ($10 USB type works)
- RF power meter (verify LoRa output)
- Bench power supply (for testing)

## Cost Breakdown

From BOM.csv:

| Configuration | Cost |
|---------------|------|
| Full system (3 nodes + hub, all features) | $480 |
| Budget system (3 nodes + hub, minimal) | $360 |
| Single node (for testing) | $75-110 |
| Hub only | $140 |
| Additional node | $75-110 each |

**Cost per node breakdown:**
- ESP32-S3: $12
- LoRa module: $7
- BME280: $8
- mmWave sensor: $12
- Display: $15 (or $3 for OLED)
- Enclosure: $15
- Power supply: $5
- Misc parts: $11-16
- **Total: $75-110**

## Ordering Strategy

### Phase 1: Prototype (Single Node + Hub)
Order enough for 1 node and 1 hub to test:
- 2x ESP32-S3
- 2x RFM95W
- 1x BME280
- 1x mmWave sensor
- 1x small display
- 1x large display
- Basic components (resistors, caps, etc.)

**Cost: ~$120-150**

### Phase 2: Production (After successful test)
Order remaining components for 2-4 more nodes:
- Additional ESP32s, sensors, displays
- Enclosures
- Marine-grade wire and connectors
- Installation hardware

**Cost: ~$150-250**

### Suppliers

**Fast (2-3 days):**
- Amazon - Mixed quality, easy returns
- Adafruit - Best quality, technical support

**Slow (2-4 weeks) but cheap:**
- AliExpress - 50-70% cheaper
- Banggood - Similar to AliExpress

**Components only:**
- Mouser - Excellent for resistors, caps, ICs
- Digi-Key - Same as Mouser
- LCSC - Cheap, integrated with JLCPCB

**Marine supplies:**
- West Marine - Wire, terminals, sealant
- Defender - Similar to West Marine
- Amazon - Cheaper but verify marine-grade

## Common Questions

**Q: Can I use ESP32-WROOM instead of ESP32-S3?**
A: Yes! Just adjust pin numbers (S3 and WROOM have different layouts). Most code is compatible.

**Q: Do I need custom PCBs?**
A: No. Breadboard or perfboard works fine. PCBs are only for cleaner builds.

**Q: What if I can't find exact parts?**
A: Most parts have equivalents. Any BME280 breakout works, any ESP32 works, displays are interchangeable (adjust code).

**Q: Can I skip the mmWave sensor?**
A: Yes. Use PIR sensor instead (~$2) or omit detection entirely (environment monitoring only).

**Q: How do I choose LoRa frequency?**
A: US/Canada/South America: 915 MHz, Europe/Africa: 868 MHz, Asia: 433 MHz. Check local regulations.

**Q: Will this work on a metal boat?**
A: LoRa struggles with metal. May need external antennas through hull or fiberglass sections. Test before committing.

## Next Steps

1. ✅ Review BOM.csv and order components
2. ✅ Read WIRING_DIAGRAM.md thoroughly
3. ✅ Build breadboard prototype
4. ✅ Upload firmware and test
5. ✅ Design PCB (optional) or use perfboard
6. ✅ Assemble production units
7. ✅ Install on boat following SETUP_GUIDE.md

## Support

- **KiCad Issues:** https://forum.kicad.info/
- **ESP32 Questions:** https://esp32.com/
- **General Electronics:** https://electronics.stackexchange.com/

---

**Ready to build? Start with the breadboard prototype following WIRING_DIAGRAM.md!**
