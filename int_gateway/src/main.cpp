#include "config.h"
#include "proto.h"
#include <Adafruit_INA219.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <Wire.h>

void smtp_send_async(const String &subject, const String &body);

AppCfg CFG;
static const int PIN_LORA_SS = 18, PIN_LORA_RST = 14, PIN_LORA_DIO0 = 26,
                 PIN_LORA_DIO1 = 35, PIN_LORA_BUSY = 32;
SX1276 radio =
    new Module(PIN_LORA_SS, PIN_LORA_DIO0, PIN_LORA_RST, PIN_LORA_BUSY);
Adafruit_INA219 ina;
Adafruit_SSD1306 oled(128, 64, &Wire, -1);

void task_lora_rx(void *);
void task_power(void *);
void task_oled(void *);
void task_alarm_manager(void *);

void setup() {
  Serial.begin(115200);
  Wire.begin(4, 15);
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  oled.clearDisplay();
  oled.display();
  ina.begin();
  radio.begin(CFG.lora.freq, CFG.lora.bw, CFG.lora.sf, CFG.lora.cr, 8,
              CFG.lora.power);
  xTaskCreatePinnedToCore(task_lora_rx, "lrx", 4096, nullptr, 2, nullptr, 1);
  xTaskCreatePinnedToCore(task_power, "pwr", 4096, nullptr, 1, nullptr, 1);
  xTaskCreatePinnedToCore(task_alarm_manager, "alarm", 4096, nullptr, 1,
                          nullptr, 0);
  xTaskCreatePinnedToCore(task_oled, "oled", 4096, nullptr, 1, nullptr, 0);
}

void loop() {}

// Alarm state management
enum AlarmState { DISARMED, ARMED, ALARM_ACTIVE, GRACE_PERIOD };
static AlarmState alarm_state = ARMED; // Default to armed
static uint32_t last_motion_time = 0;
static uint32_t last_alert_time = 0;
static bool grace_period_active = false;
static float last_tws_knots = 0;
static int last_twd_deg = 0;

void handle_frame(const uint8_t *buf, size_t len) {
  proto::Header h;
  const uint8_t *p;
  size_t n;
  if (!proto::decode(buf, len, h, &p, &n))
    return;

  uint32_t now = millis();

  if (h.type == proto::MOTION) {
    last_motion_time = now;

    if (alarm_state == ARMED) {
      // Start grace period for potential visitor
      alarm_state = GRACE_PERIOD;
      grace_period_active = true;
      smtp_send_async("[Bristol32] NOTICE",
                      "Motion detected - grace period started");
      Serial.println("Motion detected - entering grace period");

    } else if (alarm_state == GRACE_PERIOD) {
      // Additional motion during grace period - likely visitor
      smtp_send_async("[Bristol32] VISITOR", "Visitor detected at entrance");
      Serial.println("Visitor confirmed during grace period");

    } else if (alarm_state == ALARM_ACTIVE) {
      // Motion during active alarm - send update
      if (now - last_alert_time > CFG.alarm.alert_cooldown_s * 1000) {
        smtp_send_async("[Bristol32] ALARM UPDATE",
                        "Additional motion detected during active alarm");
        last_alert_time = now;
      }
    }

  } else if (h.type == proto::ENV) {
    // TODO: update last env snapshot and optionally include in emails
    // Could add environmental alarms here (temp too high/low, etc.)
  } else if (h.type == proto::WIND) {
    const proto::WindPayload *wp = (const proto::WindPayload *)p;
    last_tws_knots = wp->tws_mms / 514.444; // mm/s -> knots
    last_twd_deg = wp->twd_deg10 / 10;
    Serial.printf("WIND: %.1fkt %03d\n", last_tws_knots, last_twd_deg);
  }
}

void task_lora_rx(void *) {
  for (;;) {
    uint8_t buf[64];
    int state = radio.receive(buf, sizeof(buf));
    if (state == RADIOLIB_ERR_NONE) {
      handle_frame(buf, 32); // TODO: use actual length from RadioLib
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void task_alarm_manager(void *) {
  for (;;) {
    uint32_t now = millis();

    if (alarm_state == GRACE_PERIOD && grace_period_active) {
      if (now - last_motion_time > CFG.alarm.grace_period_s * 1000) {
        // Grace period expired - this was likely an intruder
        alarm_state = ALARM_ACTIVE;
        grace_period_active = false;
        smtp_send_async("[Bristol32] INTRUDER ALERT",
                        "Motion detected without visitor confirmation - "
                        "possible intruder!");
        Serial.println("Grace period expired - INTRUDER ALERT triggered");
        last_alert_time = now;
      }
    }

    // Auto-reset alarm after cooldown period
    if (alarm_state == ALARM_ACTIVE &&
        now - last_alert_time > CFG.alarm.alert_cooldown_s * 1000) {
      alarm_state = ARMED;
      smtp_send_async("[Bristol32] ALARM RESET",
                      "Alarm system reset to armed state");
      Serial.println("Alarm system reset to armed state");
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void task_power(void *) {
  for (;;) {
    float v = ina.getBusVoltage_V() + ina.getShuntVoltage_mV() / 1000.0f;
    float i = ina.getCurrent_mA() / 1000.0f;
    (void)v;
    (void)i; // TODO: detect edges and email
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void task_oled(void *) {
  for (;;) {
    oled.clearDisplay();
    oled.setCursor(0, 0);
    oled.setTextSize(1);
    oled.setTextColor(WHITE);
    oled.println("Bristol32 INT");

    // Display alarm status
    oled.setCursor(0, 16);
    switch (alarm_state) {
    case DISARMED:
      oled.println("ALARM: DISARMED");
      break;
    case ARMED:
      oled.println("ALARM: ARMED");
      break;
    case GRACE_PERIOD:
      oled.println("ALARM: GRACE PERIOD");
      break;
    case ALARM_ACTIVE:
      oled.println("ALARM: ACTIVE!");
      break;
    }

    // Display last motion time
    if (last_motion_time > 0) {
      uint32_t seconds_ago = (millis() - last_motion_time) / 1000;
      oled.setCursor(0, 32);
      oled.printf("Last motion: %us ago", seconds_ago);
    }

    // Display Wind
    oled.setCursor(0, 48);
    oled.printf("TWS:%.1fkt TWD:%03d", last_tws_knots, last_twd_deg);

    oled.display();
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
