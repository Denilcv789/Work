#include <HardwareSerial.h>

#define DE_PIN 4 
#define RE_PIN 5 
HardwareSerial MySerial(1); 

void setup() {
  // Initialize serial communication with the PC
  Serial.begin(115200);
  
  // Initialize UART1 for RS-485 communication
  MySerial.begin(9600, SERIAL_8N1, 16, 17); 

  // Set the DE and RE pin as output
  pinMode(DE_PIN, OUTPUT);
  pinMode(RE_PIN, OUTPUT);

  // Enable transmit mode on the RS-485 transceiver
  digitalWrite(DE_PIN, HIGH); 
  digitalWrite(RE_PIN, HIGH); 

  Serial.println("Setup complete, starting transmission...");
}

void loop() {
  // Transmit data to RS-485
  String data = "Hello World";
  Serial.println("Sending: " + data);
  MySerial.println(data);

  delay(3000); 
}

