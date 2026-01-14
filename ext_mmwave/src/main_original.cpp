#include <Arduino.h>
#include <RadioLib.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_SSD1306.h>
#include "mmwave_gpio.h"
#include "proto.h"
#include "config.h"

// Heltec V2 pins
static const int PIN_LORA_SS=18, PIN_LORA_RST=14, PIN_LORA_DIO0=26,
                 PIN_LORA_DIO1=35, PIN_LORA_BUSY=32;

AppCfg CFG; // defaults from config.h

SX1276 radio = new Module(PIN_LORA_SS, PIN_LORA_DIO0, PIN_LORA_RST, PIN_LORA_BUSY);
Adafruit_BME280 bme;
Adafruit_SSD1306 oled(128, 64, &Wire, -1);
MmwaveGPIO mmw(13);

static uint16_t NODE_ID = 0xB032;
static volatile uint32_t SEQ=0;

void task_motion(void*);
void task_env(void*);
void task_oled(void*);
void task_lora_tx(void*);

QueueHandle_t q_tx; // raw payloads for TX (simplified)

void setup() {
  Serial.begin(115200);
  Wire.begin(4, 15);
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  oled.clearDisplay(); oled.display();

  mmw.begin();
  bme.begin(0x76);

  // Radio
  radio.begin(CFG.lora.freq, CFG.lora.bw, CFG.lora.sf, CFG.lora.cr, 8, CFG.lora.power);

  q_tx = xQueueCreate(16, 64);
  xTaskCreatePinnedToCore(task_motion, "motion", 4096, nullptr, 2, nullptr, 1);
  xTaskCreatePinnedToCore(task_env, "env", 4096, nullptr, 1, nullptr, 1);
  xTaskCreatePinnedToCore(task_lora_tx, "lora", 4096, nullptr, 2, nullptr, 1);
  xTaskCreatePinnedToCore(task_oled, "oled", 4096, nullptr, 1, nullptr, 0);
}

void loop() {}

void task_motion(void*) {
  uint32_t lastFire=0, lastChange=0; bool last=false;
  for(;;){
    bool p = mmw.presence();
    uint32_t now = millis();
    if (p != last) { last = p; lastChange = now; }
    if (p && now - lastFire > CFG.motion.refractory_ms) {
      proto::Header h; h.type=proto::MOTION; h.node_id=NODE_ID; h.seq=++SEQ;
      h.flags=0; // req_ack=false
      proto::MotionPayload mp{ now - lastChange };
      uint8_t buf[32]; size_t n = proto::encode(buf, sizeof(buf), h, &mp, sizeof(mp));
      if(n) xQueueSend(q_tx, buf, 0);
      lastFire = now;
    }
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

void task_env(void*) {
  for(;;){
    proto::Header h; h.type=proto::ENV; h.node_id=NODE_ID; h.seq=++SEQ;
    proto::EnvPayload ep{ bme.readTemperature(), bme.readHumidity(), bme.readPressure()/100.0f };
    uint8_t buf[64]; size_t n = proto::encode(buf, sizeof(buf), h, &ep, sizeof(ep));
    if(n) xQueueSend(q_tx, buf, 0);
    vTaskDelay(pdMS_TO_TICKS(CFG.env.period_s*1000));
  }
}

void task_lora_tx(void*) {
  for(;;){
    uint8_t buf[64];
    if(xQueueReceive(q_tx, buf, portMAX_DELAY)==pdTRUE) {
      // Calculate actual message length based on message type
      size_t payload_len = (buf[1] == proto::MOTION) ? sizeof(proto::MotionPayload) : sizeof(proto::EnvPayload);
      size_t msg_len = sizeof(proto::Header) + payload_len + 1; // header + payload + crc

      int state = radio.transmit(buf, msg_len);
      if(state == RADIOLIB_ERR_NONE) {
        Serial.printf("TX OK: type=%d seq=%lu\n", buf[1], *(uint32_t*)&buf[6]);
      } else {
        Serial.printf("TX FAIL: %d\n", state);
      }
      vTaskDelay(pdMS_TO_TICKS(random(0,300))); // jitter
    }
  }
}
      vTaskDelay(pdMS_TO_TICKS(random(0,300))); // jitter
    }
  }
}

void task_oled(void*) {
  for(;;){
    oled.clearDisplay();
    oled.setCursor(0,0); oled.setTextSize(1); oled.setTextColor(WHITE);
    oled.println("Bristol32 EXT");
    oled.printf("mmw:%d\n", (int)mmw.presence());
    oled.display();
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

