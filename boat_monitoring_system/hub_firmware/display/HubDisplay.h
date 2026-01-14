#ifndef HUB_DISPLAY_H
#define HUB_DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9488.h>
#include "../../common/CommonTypes.h"

#define MAX_DISPLAY_NODES 6

/**
 * Hub Display Manager
 * Manages larger TFT display showing all nodes and system status
 */
class HubDisplay {
private:
  Adafruit_ILI9488* tft;
  uint8_t csPin, dcPin, rstPin;

  static const int WIDTH = 480;
  static const int HEIGHT = 320;

  static const uint16_t COLOR_BG = 0x0000;
  static const uint16_t COLOR_TEXT = 0xFFFF;
  static const uint16_t COLOR_HEADER = 0x07E0;
  static const uint16_t COLOR_ALARM = 0xF800;
  static const uint16_t COLOR_OK = 0x07E0;

public:
  HubDisplay(uint8_t cs, uint8_t dc, uint8_t rst)
    : csPin(cs), dcPin(dc), rstPin(rst) {
    tft = new Adafruit_ILI9488(cs, dc, rst);
  }

  ~HubDisplay() {
    delete tft;
  }

  bool begin() {
    tft->begin();
    tft->setRotation(3); // Landscape
    tft->fillScreen(COLOR_BG);
    Serial.println("Hub display initialized");
    return true;
  }

  void showWelcome() {
    tft->fillScreen(COLOR_BG);
    tft->setTextColor(COLOR_HEADER);
    tft->setTextSize(3);
    tft->setCursor(100, 120);
    tft->println("LIBERTY");
    tft->setTextSize(2);
    tft->setTextColor(COLOR_TEXT);
    tft->setCursor(100, 160);
    tft->println("Boat Monitor Hub");
    delay(2000);
  }

  void drawMainScreen(NodeInfo* nodes, int nodeCount, AlarmMode mode, bool alarmActive) {
    tft->fillScreen(COLOR_BG);

    // Header
    tft->fillRect(0, 0, WIDTH, 30, COLOR_HEADER);
    tft->setTextColor(COLOR_TEXT);
    tft->setTextSize(2);
    tft->setCursor(10, 8);
    tft->print("LIBERTY - ");
    tft->print(alarmModeToString(mode));

    // Alarm indicator
    if (alarmActive) {
      tft->fillCircle(WIDTH - 20, 15, 10, COLOR_ALARM);
    }

    // Node status grid
    int x = 10, y = 50;
    for (int i = 0; i < nodeCount && i < MAX_DISPLAY_NODES; i++) {
      drawNodeStatus(&nodes[i], x, y);

      x += 150;
      if (x > WIDTH - 150) {
        x = 10;
        y += 80;
      }
    }
  }

  void showAlarm(const char* triggeringNode) {
    tft->fillRect(0, HEIGHT - 60, WIDTH, 60, COLOR_ALARM);
    tft->setTextColor(COLOR_TEXT);
    tft->setTextSize(3);
    tft->setCursor(20, HEIGHT - 45);
    tft->print("ALARM: ");
    tft->println(triggeringNode);
  }

private:
  void drawNodeStatus(NodeInfo* node, int x, int y) {
    // Draw node box
    uint16_t boxColor = node->online ? 0x18C3 : 0x7BEF;
    tft->drawRect(x, y, 140, 70, boxColor);

    // Node name
    tft->setTextSize(1);
    tft->setTextColor(COLOR_TEXT);
    tft->setCursor(x + 5, y + 5);
    tft->println(node->name);

    // Status indicator
    uint16_t statusColor = node->online ? COLOR_OK : 0x7BEF;
    tft->fillCircle(x + 130, y + 10, 5, statusColor);

    // Environmental data (if online)
    if (node->online) {
      tft->setCursor(x + 5, y + 20);
      tft->print(node->temperature, 1);
      tft->print("C ");
      tft->print((int)node->humidity);
      tft->println("%");

      tft->setCursor(x + 5, y + 35);
      tft->print((int)node->pressure);
      tft->println(" hPa");

      tft->setCursor(x + 5, y + 50);
      tft->print(node->batteryVoltage / 1000.0, 1);
      tft->print("V ");
      tft->print(node->rssi);
      tft->println("dBm");
    } else {
      tft->setCursor(x + 5, y + 35);
      tft->println("OFFLINE");
    }
  }
};

#endif // HUB_DISPLAY_H
