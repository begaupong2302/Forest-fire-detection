#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include "printf.h"

#define CE_PIN 7
#define CSN_PIN 8

RF24 radio(CE_PIN, CSN_PIN); // CE, CSN

RF24Network network(radio);

//address
//edit with other CH
const uint16_t this_add = 10; // address of this node
const uint16_t lowLV_add[2] = {3, 4}; // address of this node
const uint16_t highLV_add = 0;

//data
uint16_t dataBuffer[5];

//token check
byte token_pos;
bool wait = true;

//time
const unsigned long interval_wait = 5000; //wait for 5s
unsigned long time_start_wait;

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
  network.update();

  if (!wait){
    wait = true;
    dataBuffer[0] = 0;
    RF24NetworkHeader header(/*to node*/ lowLV_add[token_pos]);
    bool ok = network.write(header, &dataBuffer, sizeof(dataBuffer));
    // time_start_wait = now;
  }

  while (network.available()){
    //receive
    RF24NetworkHeader header;                        // If so, grab it and print it out
    uint16_t payloadSize = network.peek(header);     // Use peek() to get the size of the payload
    network.read(header, &dataBuffer, payloadSize);  // Get the data
    print();

    if (dataBuffer[0] = 0){
      if (!wait){
        wait = false;
      }
    }else{
      RF24NetworkHeader header2(/*to node*/ highLV_add);
      bool ok = network.write(header2, &dataBuffer, sizeof(dataBuffer));
      if (dataBuffer[0] == lowLV_add[token_pos]){
        wait = false;
        token_pos++;
        if (token_pos >= sizeof(lowLV_add)/sizeof(lowLV_add[0])){
          token_pos = 0;
          collect_data();
          RF24NetworkHeader header(/*to node*/ highLV_add);
          bool ok = network.write(header, &dataBuffer, sizeof(dataBuffer));
        }
      }
    }
  }

  // test();
  // RF24NetworkHeader header3(/*to node*/ highLV_add);
  // network.write(header3, &dataBuffer, sizeof(dataBuffer));
}
void collect_data(){
    dataBuffer[0] = this_add;
    // dataBuffer[1] = dht22.readHumidity()*100;
    dataBuffer[1] = 1;
    // dataBuffer[2] = dht22.readTemperature()*100;
    dataBuffer[2] = 2;
    dataBuffer[3] = analogRead(A0);
    dataBuffer[4] = analogRead(A1);
}

void print(){
  for(uint32_t i = 0; i < 5; i++){
    Serial.print(dataBuffer[i]);
    Serial.print(F(" "));
  }
  Serial.println();
}
