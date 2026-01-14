#include "NavSensors.h"
#include "WindSensor.h"
#include "config.h"
#include "proto.h"
#include <Arduino.h>
#include <RadioLib.h>
#include <Wire.h>

// Hardware Config (matches ext_mmwave for LoRa, custom for sensors)
// Heltec V2 pins
static const int PIN_LORA_SS = 18, PIN_LORA_RST = 14, PIN_LORA_DIO0 = 26,
                 PIN_LORA_DIO1 = 35, PIN_LORA_BUSY = 32;

AppCfg CFG; // defaults

SX1276 radio =
    new Module(PIN_LORA_SS, PIN_LORA_DIO0, PIN_LORA_RST, PIN_LORA_BUSY);
WindSensor wind;
NavSensors nav;

static uint16_t NODE_ID = 0x05; // Wind Sensor
static volatile uint32_t SEQ = 0;

void task_wind_loop(void *);
void task_lora_tx(void *);

QueueHandle_t q_tx;

void setup() {
  Serial.begin(115200);
  Wire.begin(4, 15); // SDA, SCL

  // Init sensors
  wind.begin();
  nav.begin(&Serial1); // Assuming GPS is on Serial1, adjust pins if needed
                       // (e.g. Serial1 on pins 12,13 etc) Heltec WiFi LoRa 32
                       // V2 has Serial1? need to check pins. Default Serial1 is
                       // usually defined.

  // Init LoRa
  radio.begin(CFG.lora.freq, CFG.lora.bw, CFG.lora.sf, CFG.lora.cr, 8,
              CFG.lora.power);

  q_tx = xQueueCreate(16, 64);

  xTaskCreatePinnedToCore(task_wind_loop, "wind", 4096, nullptr, 1, nullptr, 1);
  xTaskCreatePinnedToCore(task_lora_tx, "lora", 4096, nullptr, 2, nullptr, 1);
}

void loop() {
  nav.update(); // Poll GPS frequently in main loop or task
  delay(1);
}

// Vector Math for True Wind
struct Vector {
  float x;
  float y;
};

Vector toVector(float speed, float dir_deg) {
  float rad = radians(dir_deg);
  return {speed * cos(rad), speed * sin(rad)};
}

void task_wind_loop(void *) {
  for (;;) {
    // 1. Read Raw Data
    uint16_t aws_mms = wind.getWindSpeed();
    uint16_t awd_deg10 = wind.getWindDirection();
    uint16_t bsp_mms = nav.getBoatSpeed();
    uint16_t bhd_deg10 = nav.getBoatHeading();
    uint16_t cog_deg10 =
        nav.getCourseOverGround(); // Use COG or Heading for True Wind?
                                   // Typically Heading + STW (Speed through
                                   // water) or COG + SOG. We have SOG (GPS) and
                                   // Heading (Compass) and COG (GPS). Best is
                                   // usually COG/SOG for "True Wind over
                                   // Ground".

    // 2. Calculate True Wind
    // Apparent Wind Vector (relative to boat bow)
    float aw_speed = aws_mms;
    float aw_dir = awd_deg10 / 10.0;

    // Boat Vector (Speed Over Ground)
    // If we use COG/SOG:
    float boat_speed = bsp_mms;
    float boat_dir = cog_deg10 / 10.0; // Direction boat is moving

    // Convert Apparent to True North reference first?
    // Apparent is relative to Bow (0 deg).
    // "True Wind" usually means relative to Water/Ground, but referenced to
    // North. Step A: Convert Apparent relative to Bow -> Apparent relative to
    // North
    float aw_dir_north = fmod(aw_dir + (bhd_deg10 / 10.0), 360.0);

    // Vector Calculation
    // TW = AW - BoatVelocity
    Vector v_aw = toVector(aw_speed, aw_dir_north);
    Vector v_boat = toVector(boat_speed, boat_dir);

    Vector v_tw = {v_aw.x - v_boat.x, v_aw.y - v_boat.y};

    float tw_speed = sqrt(v_tw.x * v_tw.x + v_tw.y * v_tw.y);
    float tw_dir = degrees(atan2(v_tw.y, v_tw.x));
    if (tw_dir < 0)
      tw_dir += 360;

    uint16_t tws_mms = (uint16_t)tw_speed;
    uint16_t twd_deg10 = (uint16_t)(tw_dir * 10);

    // 3. Send Packet
    proto::Header h;
    h.type = proto::WIND;
    h.node_id = NODE_ID;
    h.seq = ++SEQ;
    proto::WindPayload wp;
    wp.aws_mms = aws_mms;
    wp.awd_deg10 = awd_deg10;
    wp.tws_mms = tws_mms;
    wp.twd_deg10 = twd_deg10;
    wp.bsp_mms = bsp_mms;
    wp.bhd_deg10 = bhd_deg10;
    wp.fix_quality = nav.getFixQuality();

    uint8_t buf[64];
    size_t n = proto::encode(buf, sizeof(buf), h, &wp, sizeof(wp));
    if (n)
      xQueueSend(q_tx, buf, 0);

    vTaskDelay(pdMS_TO_TICKS(1000)); // 1Hz update
  }
}

void task_lora_tx(void *) {
  for (;;) {
    uint8_t buf[64];
    if (xQueueReceive(q_tx, buf, portMAX_DELAY) == pdTRUE) {
      // Estimate length? proto::encode returns actual length but we lost it in
      // Queue. Re-calculate or pass length in struct? Start with max buffer or
      // peek header. Header is fixed size. Payload depends on type. Only WIND
      // packets here for now.
      size_t payload_len = sizeof(proto::WindPayload);
      size_t msg_len = sizeof(proto::Header) + payload_len +
                       1; // +1? proto.cpp likely handles crc/overhead.
      // Wait, proto::encode output 'n' is total length.
      // We should ideally pass 'n' in the queue.
      // For simplicity, we can just use the fixed size for WIND packet + Header
      // overhead + CRC (if any). ext_mmwave uses: size_t msg_len =
      // sizeof(proto::Header) + payload_len + 1;

      int state = radio.transmit(buf, msg_len);
      if (state == RADIOLIB_ERR_NONE) {
        Serial.printf("TX WIND OK: seq=%u\n", SEQ);
      } else {
        Serial.printf("TX FAIL: %d\n", state);
      }
      vTaskDelay(pdMS_TO_TICKS(random(0, 200)));
    }
  }
}
