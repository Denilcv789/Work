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

  // Enable receive mode on the RS-485 transceiver
  digitalWrite(DE_PIN, LOW); 
  digitalWrite(RE_PIN, LOW); 

  Serial.println("Receiver setup complete, waiting for data...");
}

void loop() {
  if (MySerial.available()) {
    String receivedData = MySerial.readStringUntil('\n'); 
    Serial.println("Received data: " + receivedData);
  } else {
    Serial.println("No data received");
  }

  delay(1000); // Reduced delay for faster reads
}


