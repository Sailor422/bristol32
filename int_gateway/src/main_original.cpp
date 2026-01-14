#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Adafruit_SSD1306.h>
#include "proto.h"
#include "config.h"

void smtp_send_async(const String& subject, const String& body);

AppCfg CFG;
static const int PIN_LORA_SS=18, PIN_LORA_RST=14, PIN_LORA_DIO0=26,
                 PIN_LORA_DIO1=35, PIN_LORA_BUSY=32;
SX1276 radio = new Module(PIN_LORA_SS, PIN_LORA_DIO0, PIN_LORA_RST, PIN_LORA_BUSY);
Adafruit_INA219 ina;
Adafruit_SSD1306 oled(128, 64, &Wire, -1);

void task_lora_rx(void*);
void task_power(void*);
void task_oled(void*);

void setup(){
  Serial.begin(115200);
  Wire.begin(4, 15);
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  oled.clearDisplay(); oled.display();
  ina.begin();
  radio.begin(CFG.lora.freq, CFG.lora.bw, CFG.lora.sf, CFG.lora.cr, 8, CFG.lora.power);
  xTaskCreatePinnedToCore(task_lora_rx, "lrx", 4096, nullptr, 2, nullptr, 1);
  xTaskCreatePinnedToCore(task_power, "pwr", 4096, nullptr, 1, nullptr, 1);
  xTaskCreatePinnedToCore(task_oled, "oled",4096, nullptr, 1, nullptr, 0);
}

void loop(){}

void handle_frame(const uint8_t* buf, size_t len) {
  proto::Header h; const uint8_t* p; size_t n;
  if(!proto::decode(buf,len,h,&p,&n)) return;
  if(h.type==proto::MOTION){
    smtp_send_async("[Bristol32] MOTION","Presence detected at pier");
  } else if(h.type==proto::ENV){
    // TODO: update last env snapshot and optionally include in emails
  }
}

void task_lora_rx(void*){
  for(;;){
    uint8_t buf[64];
    int state = radio.receive(buf, sizeof(buf));
    if(state == RADIOLIB_ERR_NONE) {
      handle_frame(buf, 32); // TODO: use actual length from RadioLib
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void task_power(void*){
  for(;;){
    float v = ina.getBusVoltage_V() + ina.getShuntVoltage_mV()/1000.0f;
    float i = ina.getCurrent_mA()/1000.0f;
    (void)v; (void)i; // TODO: detect edges and email
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void task_oled(void*){
  for(;;){
    oled.clearDisplay();
    oled.setCursor(0,0); oled.setTextSize(1); oled.setTextColor(WHITE);
    oled.println("Bristol32 INT");
    oled.display();
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
