(kicad_sch (version 20231120) (generator "eeschema")
  (paper "A4")

  (lib_symbols
    (symbol "Device:R" (pin_names (offset 1.016)) (in_bom yes) (on_board yes)
      (property "Reference" "R" (at 2.032 0 90) (effects (font (size 1.27 1.27)) hide))
      (property "Value" "R" (at 0 0 90) (effects (font (size 1.27 1.27))))
      (property "Footprint" "" (at 0 0 0) (effects (font (size 1.27 1.27)) hide))
      (pin "~" (at -5.08 0 180) (length 2.54) (name "~" (effects (font (size 1.27 1.27)))))
      (pin "~" (at 5.08 0 0) (length 2.54) (name "~" (effects (font (size 1.27 1.27))))
    )
    (symbol "power:GND" (power) (pin_names (offset 0)) (in_bom yes) (on_board yes)
      (property "Reference" "#PWR" (at 0 -6.35 0) (effects (font (size 1.27 1.27)) hide))
      (property "Value" "GND" (at 0 -3.81 0) (effects (font (size 1.27 1.27))))
      (property "Footprint" "" (at 0 0 0) (effects (font (size 1.27 1.27)) hide))
      (pin "GND" (at 0 0 270) (length 2.54) (name "GND" (effects (font (size 1.27 1.27)))))
    )
    (symbol "power:VCC" (power) (pin_names (offset 0)) (in_bom yes) (on_board yes)
      (property "Reference" "#PWR" (at 0 3.81 0) (effects (font (size 1.27 1.27)) hide))
      (property "Value" "VCC" (at 0 3.81 0) (effects (font (size 1.27 1.27))))
      (property "Footprint" "" (at 0 0 0) (effects (font (size 1.27 1.27)) hide))
      (pin "VCC" (at 0 0 90) (length 2.54) (name "VCC" (effects (font (size 1.27 1.27)))))
    )
  )

  ; Power Supply
  (symbol (lib_id "power:VCC") (at 50.8 50.8 0) (unit 1)
    (in_bom yes) (on_board yes) (fields_autoplaced)
    (property "Reference" "#PWR01" (at 50.8 54.61 0))
    (property "Value" "12V_MARINE" (at 50.8 47 0))
    (property "Footprint" "" (at 50.8 50.8 0))
  )

  (symbol (lib_id "Device:R") (at 76.2 50.8 0) (unit 1)
    (in_bom yes) (on_board yes) (fields_autoplaced)
    (property "Reference" "R1" (at 78.232 50.8 90))
    (property "Value" "VOLTAGE_REG" (at 74.168 50.8 90))
    (property "Footprint" "" (at 76.2 50.8 0))
  )

  (symbol (lib_id "power:VCC") (at 101.6 50.8 0) (unit 1)
    (in_bom yes) (on_board yes) (fields_autoplaced)
    (property "Reference" "#PWR02" (at 101.6 54.61 0))
    (property "Value" "3.3V" (at 101.6 47 0))
    (property "Footprint" "" (at 101.6 50.8 0))
  )

  ; ESP32 Gateway
  (symbol (lib_id "Device:R") (at 152.4 50.8 0) (unit 1)
    (in_bom yes) (on_board yes) (fields_autoplaced)
    (property "Reference" "U1" (at 154.432 50.8 90))
    (property "Value" "ESP32_HELTEC" (at 150.368 50.8 90))
    (property "Footprint" "" (at 152.4 50.8 0))
  )

  ; LoRa Module
  (symbol (lib_id "Device:R") (at 203.2 50.8 0) (unit 1)
    (in_bom yes) (on_board yes) (fields_autoplaced)
    (property "Reference" "U2" (at 205.232 50.8 90))
    (property "Value" "SX1276_LORA" (at 201.168 50.8 90))
    (property "Footprint" "" (at 203.2 50.8 0))
  )

  ; WiFi Module
  (symbol (lib_id "Device:R") (at 152.4 25.4 0) (unit 1)
    (in_bom yes) (on_board yes) (fields_autoplaced)
    (property "Reference" "U3" (at 154.432 25.4 90))
    (property "Value" "ESP32_WIFI" (at 150.368 25.4 90))
    (property "Footprint" "" (at 152.4 25.4 0))
  )

  ; SMTP Email Client
  (symbol (lib_id "Device:R") (at 203.2 25.4 0) (unit 1)
    (in_bom yes) (on_board yes) (fields_autoplaced)
    (property "Reference" "U4" (at 205.232 25.4 90))
    (property "Value" "SMTP_CLIENT" (at 201.168 25.4 90))
    (property "Footprint" "" (at 203.2 25.4 0))
  )

  ; Power Monitor
  (symbol (lib_id "Device:R") (at 152.4 76.2 0) (unit 1)
    (in_bom yes) (on_board yes) (fields_autoplaced)
    (property "Reference" "U5" (at 154.432 76.2 90))
    (property "Value" "INA219_PWR" (at 150.368 76.2 90))
    (property "Footprint" "" (at 152.4 76.2 0))
  )

  ; OLED Display
  (symbol (lib_id "Device:R") (at 203.2 76.2 0) (unit 1)
    (in_bom yes) (on_board yes) (fields_autoplaced)
    (property "Reference" "U6" (at 205.232 76.2 90))
    (property "Value" "SSD1306_OLED" (at 201.168 76.2 90))
    (property "Footprint" "" (at 203.2 76.2 0))
  )

  ; Ground
  (symbol (lib_id "power:GND") (at 76.2 76.2 0) (unit 1)
    (in_bom yes) (on_board yes) (fields_autoplaced)
    (property "Reference" "#PWR03" (at 76.2 72.39 0))
    (property "Value" "GND" (at 76.2 78.74 0))
    (property "Footprint" "" (at 76.2 76.2 0))
  )

  ; Connections
  (wire (pts (xy 50.8 50.8) (xy 71.12 50.8)))
  (wire (pts (xy 81.28 50.8) (xy 96.52 50.8)))
  (wire (pts (xy 101.6 50.8) (xy 147.32 50.8)))
  (wire (pts (xy 157.48 50.8) (xy 198.12 50.8)))
  (wire (pts (xy 208.28 50.8) (xy 220 50.8)))
  (wire (pts (xy 76.2 55.88) (xy 76.2 71.12)))

  (sheet_instances
    (path "/" (page "1"))
  )
)
