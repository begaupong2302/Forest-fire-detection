#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include "printf.h"

//setting
#define CE_PIN 7
#define CSN_PIN 8

RF24 radio(CE_PIN, CSN_PIN); // CE, CSN

RF24Network network(radio);

//initialize var
//address
const uint16_t this_add = 10; // address of this node
const uint16_t lowLV_add[] = {}; //low lv add
const uint16_t highLV_add = 0;

//data
uint16_t dataRev[6]; // to receive
uint16_t dataSen[6]; // to send
uint16_t c_loop = 0; // count loop

//time
const unsigned long interval_L = 10000;
long extra_time = 0; // time for waiting first time update
unsigned long last_sent;

bool start = true; // check start
uint8_t trying = 3; // try 3 times for sending
//change these var for each sensor node

void setup() {
  Serial.begin(115200);
  Serial.println(F("RF24Network/CH"));
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
  network.update(); // Check the network regularly

  while (network.available()){
    //receive
    RF24NetworkHeader header;                        // If so, grab it and print it out
    uint16_t payloadSize = network.peek(header);     // Use peek() to get the size of the payload
    network.read(header, &dataRev, payloadSize);  // Get the data
    print();

    if (dataRev[0] == 0){
      update_clock();

      //send to lower
      unsigned long size_add = sizeof(lowLV_add)/sizeof(lowLV_add[0]);
      for (byte j = 0; j<size_add; j++){
        RF24NetworkHeader header(/*to node*/ lowLV_add[j]); // maybe error
        for (byte i = 0; i<trying; i++){
          bool ok = network.write(header, &dataRev, sizeof(dataRev));
          if (ok) break;
          Serial.println("No connect, trying ");
          Serial.println(i+1);
          Serial.println(" time");
        }
      }
    }else{
      //send to higher node
      RF24NetworkHeader header(/*to node*/ highLV_add); // maybe error
      for (byte i = 0; i<trying; i++){
        bool ok = network.write(header, &dataRev, sizeof(dataRev));
        if (ok) break;
        Serial.println("No connect, trying ");
        Serial.println(i+1);
        Serial.println(" time");
      }
    }
  }

  unsigned long now = millis();
  if (now - last_sent >= interval_L + extra_time){
    collect_data();

    last_sent = now;
    extra_time = 0;
    RF24NetworkHeader header(/*to node*/ highLV_add);
    for (byte i = 0; i<trying; i++){
      bool ok = network.write(header, &dataRev, sizeof(dataRev));
      if (ok) break;
      Serial.println("No connect, trying ");
      Serial.println(i+1);
      Serial.println(" time");
    }
  }
}

void update_clock(){
  unsigned long time_update = dataRev[3] + 10000*(dataRev[4] + 10000*dataRev[5]);
  extra_time = millis() - time_update;
}

void collect_data(){
    dataRev[0] = 3;
    dataRev[1] = 4;
    dataRev[2] = 5;
    dataRev[3] = 6;
    dataRev[4] = 7;
    dataRev[5] = 8;
}

void print(){
  for(uint32_t i = 0; i < 6; i++){
    Serial.print(dataRev[i]);
    Serial.print(F(" "));
  }
  Serial.println();
}
