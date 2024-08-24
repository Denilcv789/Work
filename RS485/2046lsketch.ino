#include <HardwareSerial.h>

#define DE_PIN 4 //Driver enable
#define RE_PIN 5 //Receiver enable
HardwareSerial MySerial(1); 

void setup() {

  Serial.begin(115200);
  MySerial.begin(9600, SERIAL_8N1, 16, 17); // RX, TX

  // Set the DE and RE pin as output
  pinMode(DE_PIN, OUTPUT);
  pinMode(RE_PIN, OUTPUT);
  digitalWrite(DE_PIN, LOW); 
  digitalWrite(RE_PIN, LOW); // Set to receive mode

  Serial.println("Setup complete");
}

void loop() {
  
  Serial.println("Sending command to 2046L");
  digitalWrite(DE_PIN, HIGH); 
  digitalWrite(RE_PIN, HIGH); // Enable transmit mode
  MySerial.println("Command to 2046L");
  delay(100);
  digitalWrite(DE_PIN, LOW); 
  digitalWrite(RE_PIN, LOW); // Enable receive mode
  Serial.println("Command sent, waiting for response");

  if (MySerial.available()) {
    String receivedData = MySerial.readString();
    Serial.println("Received data: " + receivedData);
  } else {
    Serial.println("No data received");
  }

  delay(1000);
}
