#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include "printf.h"

//setting
#define CE_PIN 4
#define CSN_PIN 5

RF24 radio(CE_PIN, CSN_PIN); // CE, CSN

RF24Network network(radio);

//initialize var
//address
const uint16_t this_add = 0; // address of this node
const uint16_t lowLV_add[] = {}; //low lv add

//time
const unsigned long interval_UC = 30000; //ms //time for updating clock
const unsigned long interval_L = 10000; //ms //one loop time
uint32_t timeBetweenPackets = 0;
unsigned long last_update; //last send

//data 
uint16_t dataRev[6]; // to receive
uint16_t dataSen[6] = {0,0,0,0,0,0}; // to send
uint16_t c_loop = 0; // count loop

bool start = true; // check start
uint8_t trying = 3; // try 3 times for sending

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
  //update clock
  unsigned long now = millis();
  if (start || now - last_update >= interval_UC){
    start = false;
    //add time to data
    byte z = 3;
    while(now){
      dataSen[z] = now%10000;
      now = now/10000;
      z++;
    }

    //send
    unsigned long size_add = sizeof(lowLV_add)/sizeof(lowLV_add[0]);
    for (byte j = 0; j<size_add; j++){
      dataSen[1] = (interval_L - interval_L%size_add)/size_add*j;
      dataSen[2] = (interval_L - interval_L%size_add)/size_add*(j+1);

      RF24NetworkHeader header(/*to node*/ lowLV_add[j]); // maybe error
      for (byte i = 0; i<trying; i++){
        bool ok = network.write(header, &dataSen, sizeof(dataSen));
        if (ok) break;
        Serial.println("No connect, trying ");
        Serial.println(i+1);
        Serial.println(" time");
      }
    }
  }

  //receive data
  while (network.available()) {
    RF24NetworkHeader header;                        
    uint16_t payloadSize = network.peek(header);     
    network.read(header, &dataRev, payloadSize);  
    Serial.print("Received packet, size ");          
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
    Serial.print(dataRev[i]);
    Serial.print(F(" "));
  }
  Serial.println();
}




