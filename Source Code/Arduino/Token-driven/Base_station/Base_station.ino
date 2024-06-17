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
const uint16_t lowLV_add[2] = {1, 2}; // address of this node

//data
uint16_t dataBuffer[5];
uint16_t c_loop = 0;

//token check
bool check_token[2] = {false, false};
byte token_pos; // token position now
bool wait = false; // waiting or not

//time
const unsigned long interval_wait = 5000; //wait for 5s
unsigned long time_start_wait; 

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
  network.update();
  
  unsigned long now = millis();
  if (!wait){
    wait = true;
    dataBuffer[0] = 0;
    RF24NetworkHeader header(/*to node*/ lowLV_add[token_pos]);
    bool ok = network.write(header, &dataBuffer, sizeof(dataBuffer));
    time_start_wait = now;
  }
  if (now - time_start_wait >= interval_wait){
    wait = false;
    token_pos++;
    if (token_pos >= sizeof(lowLV_add)/sizeof(lowLV_add[0])){
      token_pos = 0;
      c_loop++;
      delay(10000);
    }
  }
  //co can thong bao den sensor qua time gui ko can gui nua khong

  while (network.available()) {
    RF24NetworkHeader header;                        // If so, grab it and print it out
    uint16_t payloadSize = network.peek(header);     // Use peek() to get the size of the payload
    network.read(header, &dataBuffer, payloadSize);  // Get the data
    print();

    if (dataBuffer[0] == lowLV_add[token_pos]){
      wait = false;
      token_pos++;
      if (token_pos >= sizeof(lowLV_add)/sizeof(lowLV_add[0])){
        token_pos = 0;
        c_loop++;
        delay(10000);
      }
    }
  }
}

void print(){
  for(uint32_t i = 0; i < 5; i++){
    Serial.print(dataBuffer[i]);
    Serial.print(F(" "));
  }
  Serial.println();
}





