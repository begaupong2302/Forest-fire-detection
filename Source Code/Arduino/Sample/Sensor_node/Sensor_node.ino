#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include "DHT.h"
#include "printf.h"

#define CE_PIN 7
#define CSN_PIN 8

RF24 radio(CE_PIN, CSN_PIN); // CE, CSN

RF24Network network(radio);

DHT dht22(3, DHT22);

//edit with other SN
const uint16_t this_add = 100;
const uint16_t highLV_add = 10; // all address of higher 1 lv node

//time
const unsigned long interval = 3000;
unsigned long last_sent; 

uint8_t dataBuffer[5];

void setup() {
  Serial.begin(115200);
  Serial.println(F("RF24Network/LV1/CH1/SN1"));
  printf_begin(); 
  dht22.begin();

  if (!radio.begin()) {
    Serial.println(F("Radio hardware not responding!"));
    while (1) {
      // hold in infinite loop
    }
  }
  radio.setChannel(90);
  network.begin(/*node address*/ this_add);
  radio.printDetails();
}

void loop() {
  // put your main code here, to run repeatedly:
  network.update();

  unsigned long now = millis();
  if (now - last_sent >= interval) {
    last_sent = now;
    //sn data
    dataBuffer[0] = this_add;
    dataBuffer[1] = dht22.readHumidity()*100;
    dataBuffer[2] = dht22.readTemperature()*100;
    dataBuffer[3] = analogRead(A0);
    dataBuffer[4] = analogRead(A1);
    print();
    // Fragmentation/reassembly is transparent. Just send payloads as usual.
    RF24NetworkHeader header(/*to node*/ highLV_add);
    bool ok = network.write(header, &dataBuffer, sizeof(dataBuffer));
    Serial.println(ok ? F(" ok.") : F(" failed."));
  }
}

void print(){
  for(uint32_t i = 0; i < 5; i++){
    Serial.print(dataBuffer[i]);
    Serial.print(F(" "));
  }
  Serial.println();
}
