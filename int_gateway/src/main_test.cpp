#include <Arduino.h>
#include <RadioLib.h>

// Heltec V2 pins
static const int PIN_LORA_SS=18, PIN_LORA_RST=14, PIN_LORA_DIO0=26,
                 PIN_LORA_DIO1=35, PIN_LORA_BUSY=32;

SX1276 radio = new Module(PIN_LORA_SS, PIN_LORA_DIO0, PIN_LORA_RST, PIN_LORA_BUSY);

// Configuration
#define TARGET_FREQ 433.775f
#define LORA_BW 125
#define LORA_SF 9
#define LORA_CR 7
#define TX_POWER 10

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Bristol32 LoRa Test - Internal Gateway");

  // Initialize LoRa
  int state = radio.begin(TARGET_FREQ, LORA_BW, LORA_SF, LORA_CR, 8, TX_POWER);
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("LoRa initialized successfully!");
  } else {
    Serial.printf("LoRa init failed: %d\n", state);
    while (true);
  }
}

void loop() {
  // Listen for messages
  String received = "";
  int state = radio.receive(received);

  if (state == RADIOLIB_ERR_NONE) {
    Serial.printf("Received: %s\n", received.c_str());

    // Send acknowledgment
    String response = "INT_ACK_" + String(millis());
    state = radio.transmit(response.c_str(), response.length());

    if (state == RADIOLIB_ERR_NONE) {
      Serial.printf("Sent ACK: %s\n", response.c_str());
    } else {
      Serial.printf("ACK TX failed: %d\n", state);
    }
  }

  delay(100);
}