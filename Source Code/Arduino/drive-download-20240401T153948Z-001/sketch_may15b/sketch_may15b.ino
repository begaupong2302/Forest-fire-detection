#include "printf.h"
#include <RF24.h>
#include <RF24Network.h>
#include "DHT.h"
 
DHT dht22(3, DHT22);

RF24 radio(7, 8);  // nRF24L01(+) radio attached using Getting Started board
 
RF24Network network(radio);  // Network uses that radio
 
const uint16_t this_node = 01;   // Address of our node in Octal format
const uint16_t other_node = 00;  // Address of the other node in Octal format
 
const unsigned long interval = 2000;  //ms  // How often to send 'hello world to the other unit
 
unsigned long last_sent;  // When did we last send?
 
/**** Create a large array for data to be sent ****
* MAX_PAYLOAD_SIZE is defined in RF24Network_config.h
* Payload sizes of ~1-2 KBytes or more are practical when radio conditions are good
*/
uint8_t dataBuffer[5];
 
void setup(void) {
  Serial.begin(115200);
  dht22.begin();
  while (!Serial) {
    // some boards need this because of native USB capability
  }
  Serial.println(F("RF24Network/examples/helloworld_tx_advanced/"));
  printf_begin();  // needed for RF24* libs' internal printf() calls
 
  if (!radio.begin()) {
    Serial.println(F("Radio hardware not responding!"));
    while (1) {
      // hold in infinite loop
    }
  }
  radio.setChannel(90);
  network.begin(/*node address*/ this_node);
  radio.printDetails();
 
  // Load our data buffer with numbered data
}
uint16_t sizeofSend = 5; 
 
void loop() {
 
  //User input anything via Serial to stop/start data transmission

  network.update();  // Check the network regularly
 
  unsigned long now = millis();  // If it's time to send a message, send it!
  if (now - last_sent >= interval) {
    last_sent = now;
    Serial.print(F("Sending size "));
    Serial.print(sizeofSend);
    dataBuffer[0] = this_node;
    dataBuffer[1] = dht22.readHumidity();
    dataBuffer[2] = dht22.readTemperature();
    dataBuffer[3] = analogRead(A0)/10;
    dataBuffer[4] = analogRead(A1)/10;
    print();
    // Fragmentation/reassembly is transparent. Just send payloads as usual.
    RF24NetworkHeader header(/*to node*/ other_node);
    bool ok = network.write(header, &dataBuffer, sizeofSend);
  
    // If the size of data to be sent is larger than max payload size, reset at 0
    if (sizeofSend > MAX_PAYLOAD_SIZE) {
      sizeofSend = 0;
    }
 
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
