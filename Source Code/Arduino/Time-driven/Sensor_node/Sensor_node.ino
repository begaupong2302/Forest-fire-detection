#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include "DHT.h"
#include "printf.h"

//setting
#define CE_PIN 7
#define CSN_PIN 8

RF24 radio(CE_PIN, CSN_PIN); // CE, CSN

RF24Network network(radio);

DHT dht22(3, DHT22); // if node have dth22 sensor

//initialize var
//address
const uint16_t this_add = 100;
const uint16_t highLV_add = 10; // all address of higher 1 lv node

//time
const unsigned long interval_L = 10000;
unsigned long last_sent; 
long extra_time = 0; // time for waiting first time update

//data to send
uint8_t dataRev[6];
uint16_t c_loop = 0;

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
  while (network.available()){
    //receive
    RF24NetworkHeader header;                        // If so, grab it and print it out
    uint16_t payloadSize = network.peek(header);     // Use peek() to get the size of the payload
    network.read(header, &dataRev, payloadSize);  // Get the data
    print();

    if (dataRev[0] == 0){
      update_clock();
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
    dataRev[0] = this_add;
    dataRev[1] = ++c_loop;
    dataRev[2] = dht22.readHumidity()*100;
    dataRev[3] = dht22.readTemperature()*100;
    dataRev[4] = analogRead(A0);
    dataRev[5] = analogRead(A1);
}

void print(){
  for(uint32_t i = 0; i < 5; i++){
    Serial.print(dataRev[i]);
    Serial.print(F(" "));
  }
  Serial.println();
}
