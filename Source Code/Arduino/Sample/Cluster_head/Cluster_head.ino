#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include "printf.h"

#define CE_PIN 7
#define CSN_PIN 8

RF24 radio(CE_PIN, CSN_PIN); // CE, CSN

RF24Network network(radio);

const unsigned long interval = 5000;

//address
//edit with other CH
const uint16_t this_add = 10; // address of this node
// const uint16_t highLV_add[2] = {10, 11}; // all address of higher 1 lv node
const uint16_t highLV_add = 0;

uint8_t dataBuffer[5];
uint32_t timeBetweenPackets = 0;

void setup() {
  Serial.begin(115200);
  Serial.println(F("RF24Network/LV1/CH2"));
  printf_begin();  // needed for RF24* libs' internal printf() calls
 
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
  while (network.available()){
    //receive
    RF24NetworkHeader header;                        // If so, grab it and print it out
    uint16_t payloadSize = network.peek(header);     // Use peek() to get the size of the payload
    network.read(header, &dataBuffer, payloadSize);  // Get the data
    Serial.print("Received packet, size ");          // Print info about received data
    Serial.print(payloadSize);
    Serial.print("(");
    Serial.print(millis() - timeBetweenPackets);
    Serial.println("ms since last)");
    timeBetweenPackets = millis();
    print();

    //send
    RF24NetworkHeader header2(/*to node*/ highLV_add);
    bool ok = network.write(header2, &dataBuffer, sizeof(dataBuffer));
    Serial.println(ok ? F(" ok.") : F(" failed."));
  }

  // test();
  // RF24NetworkHeader header3(/*to node*/ highLV_add);
  // network.write(header3, &dataBuffer, sizeof(dataBuffer));
}
void test(){
    delay(3000);
    dataBuffer[0] = 3;
    dataBuffer[1] = 4;
    dataBuffer[2] = 5;
    dataBuffer[3] = 6;
    dataBuffer[4] = 7;
}

void print(){
  for(uint32_t i = 0; i < 5; i++){
    Serial.print(dataBuffer[i]);
    Serial.print(F(" "));
  }
  Serial.println();
}
