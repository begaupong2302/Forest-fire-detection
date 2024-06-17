#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include "printf.h"

#define CE_PIN 4
#define CSN_PIN 5

RF24 radio(CE_PIN, CSN_PIN); // CE, CSN

RF24Network network(radio);

//address
const uint16_t this_add = 0; // address of this node

uint8_t dataBuffer[5];
uint32_t timeBetweenPackets = 0;

void setup() {
  Serial.begin(115200);
  Serial.println(F("RF24Network/BS"));
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
  while (network.available()) {
    RF24NetworkHeader header;                        // If so, grab it and print it out
    uint16_t payloadSize = network.peek(header);     // Use peek() to get the size of the payload
    network.read(header, &dataBuffer, payloadSize);  // Get the data
    Serial.print("Received packet, size ");          // Print info about received data
    Serial.print(payloadSize);
    Serial.print("(");
    Serial.print(millis() - timeBetweenPackets);
    Serial.println("ms since last)");
    print();
    timeBetweenPackets = millis();
  }
}

void print(){
  for(uint32_t i = 0; i < 5; i++){
    Serial.print(dataBuffer[i]);
    Serial.print(F(" "));
  }
  Serial.println();
}





