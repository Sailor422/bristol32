#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "../../common/CommonTypes.h"

/**
 * Display Manager for Node
 * Handles TFT display rendering for environmental data and status
 */
class DisplayManager {
private:
  Adafruit_ILI9341* tft;
  uint8_t csPin;
  uint8_t dcPin;
  uint8_t rstPin;

  uint8_t brightness;
  unsigned long lastUpdate;
  bool displayOn;

  // Screen dimensions
  static const int WIDTH = 240;
  static const int HEIGHT = 320;

  // Colors
  static const uint16_t COLOR_BG = 0x0000;       // Black
  static const uint16_t COLOR_TEXT = 0xFFFF;     // White
  static const uint16_t COLOR_HEADER = 0x07E0;   // Green
  static const uint16_t COLOR_ALARM = 0xF800;    // Red
  static const uint16_t COLOR_WARNING = 0xFD20;  // Orange
  static const uint16_t COLOR_OK = 0x07E0;       // Green

public:
  DisplayManager(uint8_t cs, uint8_t dc, uint8_t rst)
    : csPin(cs), dcPin(dc), rstPin(rst), brightness(128),
      lastUpdate(0), displayOn(true) {
    tft = new Adafruit_ILI9341(cs, dc, rst);
  }

  ~DisplayManager() {
    delete tft;
  }

  bool begin() {
    tft->begin();
    tft->setRotation(0); // Portrait mode
    tft->fillScreen(COLOR_BG);

    setBrightness(brightness);
    displayOn = true;

    Serial.println("Display initialized");
    return true;
  }

  void setBrightness(uint8_t level) {
    brightness = level;
    // Note: ILI9341 doesn't have built-in backlight control
    // You'll need to control backlight LED via PWM on separate pin
    // Example: analogWrite(BACKLIGHT_PIN, brightness);
  }

  void powerOn() {
    if (!displayOn) {
      // Turn on backlight
      setBrightness(brightness);
      displayOn = true;
    }
  }

  void powerOff() {
    if (displayOn) {
      // Turn off backlight
      setBrightness(0);
      displayOn = false;
    }
  }

  void showBootScreen(const String& nodeName) {
    tft->fillScreen(COLOR_BG);
    tft->setTextColor(COLOR_HEADER);
    tft->setTextSize(2);
    tft->setCursor(20, 100);
    tft->println("LIBERTY");
    tft->setTextColor(COLOR_TEXT);
    tft->setTextSize(1);
    tft->setCursor(20, 130);
    tft->println(nodeName);
    tft->setCursor(20, 150);
    tft->println("Initializing...");
    delay(2000);
  }

  void drawMainScreen(const String& nodeName, float temp, float humidity,
                     float pressure, const char* trend, AlarmMode mode,
                     int rssi, uint16_t batteryMv) {
    tft->fillScreen(COLOR_BG);

    // Header
    drawHeader(nodeName, mode);

    // Environmental data
    int yPos = 40;

    tft->setTextSize(2);
    tft->setTextColor(COLOR_TEXT);

    // Temperature
    tft->setCursor(10, yPos);
    tft->print("Temp:  ");
    tft->print(temp, 1);
    tft->print("C");
    yPos += 30;

    // Humidity
    tft->setCursor(10, yPos);
    tft->print("Humid: ");
    tft->print((int)humidity);
    tft->print("%");
    yPos += 30;

    // Pressure with trend
    tft->setCursor(10, yPos);
    tft->print("Baro:  ");
    tft->print((int)pressure);
    tft->print("hPa");
    yPos += 25;

    tft->setTextSize(1);
    tft->setCursor(10, yPos);
    tft->print("Trend: ");
    tft->setTextSize(2);
    tft->print(trend);
    yPos += 40;

    // Status info
    tft->setTextSize(1);
    tft->setTextColor(COLOR_TEXT);

    tft->setCursor(10, yPos);
    tft->print("Signal: ");
    tft->print(rssi);
    tft->print(" dBm");
    yPos += 15;

    tft->setCursor(10, yPos);
    tft->print("Battery: ");
    tft->print(batteryMv / 1000.0, 2);
    tft->print("V");

    // Mode indicator at bottom
    drawModeIndicator(mode);

    lastUpdate = millis();
  }

  void showAlarmTriggered(const String& zone) {
    tft->fillScreen(COLOR_ALARM);
    tft->setTextColor(COLOR_TEXT);
    tft->setTextSize(3);
    tft->setCursor(20, 100);
    tft->println("ALARM!");
    tft->setTextSize(2);
    tft->setCursor(20, 140);
    tft->print("Zone: ");
    tft->println(zone);
    tft->setTextSize(1);
    tft->setCursor(20, 180);
    tft->println("Disarm to silence");
  }

  void showPreAlarm(int countdown) {
    tft->fillRect(0, 200, WIDTH, 60, COLOR_WARNING);
    tft->setTextColor(COLOR_TEXT);
    tft->setTextSize(2);
    tft->setCursor(20, 215);
    tft->print("WARNING: ");
    tft->print(countdown);
    tft->print("s");
  }

  void showVisitor() {
    // Show doorbell notification
    tft->fillRect(0, 200, WIDTH, 60, COLOR_OK);
    tft->setTextColor(COLOR_TEXT);
    tft->setTextSize(2);
    tft->setCursor(20, 215);
    tft->println("VISITOR");
  }

  void showError(const String& errorMsg) {
    tft->fillScreen(COLOR_ALARM);
    tft->setTextColor(COLOR_TEXT);
    tft->setTextSize(2);
    tft->setCursor(20, 100);
    tft->println("ERROR");
    tft->setTextSize(1);
    tft->setCursor(20, 130);
    tft->println(errorMsg);
  }

  void clear() {
    tft->fillScreen(COLOR_BG);
  }

private:
  void drawHeader(const String& nodeName, AlarmMode mode) {
    // Draw header bar
    tft->fillRect(0, 0, WIDTH, 30, COLOR_HEADER);
    tft->setTextColor(COLOR_TEXT);
    tft->setTextSize(1);
    tft->setCursor(5, 10);
    tft->print(nodeName);

    // Status indicator
    uint16_t statusColor = getStatusColor(mode);
    tft->fillCircle(WIDTH - 15, 15, 8, statusColor);
  }

  void drawModeIndicator(AlarmMode mode) {
    tft->fillRect(0, HEIGHT - 30, WIDTH, 30, COLOR_HEADER);
    tft->setTextColor(COLOR_TEXT);
    tft->setTextSize(1);
    tft->setCursor(10, HEIGHT - 20);
    tft->print("Mode: ");
    tft->print(alarmModeToString(mode));
  }

  uint16_t getStatusColor(AlarmMode mode) {
    switch (mode) {
      case MODE_DISARMED:
        return COLOR_OK;       // Green
      case MODE_DOORBELL:
        return 0x07FF;         // Cyan
      case MODE_PERIMETER:
      case MODE_FULL:
        return COLOR_ALARM;    // Red
      case MODE_QUIET:
        return 0x001F;         // Blue
      default:
        return 0x7BEF;         // Gray
    }
  }
};

#endif // DISPLAY_MANAGER_H
