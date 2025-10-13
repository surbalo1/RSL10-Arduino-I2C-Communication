#include <Wire.h>  // library for I2C communication

#define SLAVE_ADDRESS 0x55  // I2C address for the Arduino slave

volatile uint8_t lastReceived = 0;  // stores the last received byte

void setup() {
  Serial.begin(9600);  // start serial communication for debugging
  delay(1000);  // short delay to stabilize
  
  Serial.println("\n=== Arduino I2C Slave - WORKING ===");
  
  Wire.begin(SLAVE_ADDRESS);  // start Arduino as I2C slave
  Wire.setClock(10000);  // match RSL10’s low I2C speed
  Wire.onReceive(receiveEvent);  // function called when data is received
  Wire.onRequest(requestEvent);  // function called when master requests data
  
  pinMode(LED_BUILTIN, OUTPUT);  // set built-in LED as output
  
  Serial.print("Listening on 0x");
  Serial.println(SLAVE_ADDRESS, HEX);  // print the I2C address
  Serial.println("Ready!\n");
}

void loop() {
  delay(100);  // main loop does nothing, just keeps the program running
}

void receiveEvent(int bytes) {
  Serial.print("RX: ");
  
  // read all bytes sent by the master
  while(Wire.available()) {
    byte b = Wire.read();  // read one byte
    lastReceived = b;  // save it as the last received value
    Serial.print("0x");
    if(b < 0x10) Serial.print("0");  // format output nicely
    Serial.print(b, HEX);
    Serial.print(" (");
    Serial.print(b);
    Serial.print(") ");
  }
  Serial.println();
  
  // blink the LED to indicate data was received
  digitalWrite(LED_BUILTIN, HIGH);
  delay(50);
  digitalWrite(LED_BUILTIN, LOW);
}

void requestEvent() {
  // when the master requests data, send back lastReceived + 1
  uint8_t response = lastReceived + 1;
  Wire.write(response);
  
  Serial.print("TX: 0x");
  if(response < 0x10) Serial.print("0");
  Serial.print(response, HEX);
  Serial.println();
}
