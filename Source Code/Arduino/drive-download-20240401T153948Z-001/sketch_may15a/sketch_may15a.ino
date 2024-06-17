
#include "printf.h"
#include <RF24.h>
#include <RF24Network.h>

RF24 radio(7, 8);  // nRF24L01(+) radio attached using Getting Started board

RF24Network network(radio);      // Network uses that radio
const uint16_t this_node = 03;   // Address of our node in Octal format ( 04,031, etc)
const uint16_t other_node = 02;  // Address of the other node in Octal format

/**** Create a large array for data to be received ****
* MAX_PAYLOAD_SIZE is defined in RF24Network_config.h
* Payload sizes of ~1-2 KBytes or more are practical when radio conditions are good
*/
uint8_t dataBuffer[MAX_PAYLOAD_SIZE];  //MAX_PAYLOAD_SIZE is defined in RF24Network_config.h

void setup(void) {

  Serial.begin(115200);
  while (!Serial) {
    // some boards need this because of native USB capability
  }
  Serial.println(F("RF24Network/examples/helloworld_rx_advanced/"));

  if (!radio.begin()) {
    Serial.println(F("Radio hardware not responding!"));
    while (1) {
      // hold in infinite loop
    }
  }
  radio.setChannel(90);
  network.begin(/*node address*/ this_node);

  printf_begin();        // needed for RF24* libs' internal printf() calls
  radio.printDetails();  // requires printf support
  for (uint16_t i = 0; i < MAX_PAYLOAD_SIZE; i++) {
    dataBuffer[i] = i % 256;  //Ensure the max value is 255
  }
}

// Variable for calculating how long between RX
uint32_t timeBetweenPackets = 0;
const unsigned long interval = 2000;  //ms  // How often to send 'hello world to the other unit
unsigned long last_sent;  // When did we last send?

void loop(void) {

  network.update();  // Check the network regularly

  while (network.available()) {  // Is there anything ready for us?

    RF24NetworkHeader header;                        // If so, grab it and print it out
    uint16_t payloadSize = network.peek(header);     // Use peek() to get the size of the payload
    network.read(header, &dataBuffer, payloadSize);  // Get the data
    Serial.print("Received packet, size ");          // Print info about received data
    Serial.print(payloadSize);
    Serial.print("(");
    Serial.print(millis() - timeBetweenPackets);
    Serial.println("ms since last)");
    timeBetweenPackets = millis();

    // Uncomment below to print the entire payload
    /*
    for(uint32_t i = 0; i < payloadSize; i++) {
      Serial.print(dataBuffer[i]);
      Serial.print(F(": "));
      if(i % 50 == 49) {
        //Add a line break every 50 characters
        Serial.println();
      }
    }
    Serial.println();
    */
  }
  unsigned long now = millis(); 
  if (now - last_sent >= interval){
    last_sent = now;
      RF24NetworkHeader header2(/*to node*/ other_node);
      bool ok = network.write(header2, &dataBuffer, 5);
      Serial.println(ok ? F(" ok.") : F(" failed."));
  }

}