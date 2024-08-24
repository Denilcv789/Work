#include <ModbusRTU.h>

// RS485 module connections
#define RS485_RXD 16  // RXD pin on the RS485 module
#define RS485_TXD 17  // TXD pin on the RS485 module
#define RS485_DE 4   // DE (Data Enable) pin on the RS485 module

ModbusRTU mb;

uint16_t voltage;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize RS485 module
  Serial1.begin(9600, SERIAL_8N1, RS485_RXD, RS485_TXD);
  pinMode(RS485_DE, OUTPUT);
  digitalWrite(RS485_DE, LOW);

  // Setup ModbusRTU
  mb.begin(&Serial1, RS485_DE);
  mb.master();

  Serial.println("Setup complete");
}

void loop() {
  if (!mb.slave()) {
    // Read holding register (address: 40002)
    if (mb.readHreg(1, 40012, &voltage, 1)) {
      // Wait for the transaction to complete
      while (mb.slave()) {
        mb.task();
      }
      
      // Display the voltage value
      Serial.print("Output Voltage: ");
      Serial.println(voltage);
    }
  }

  // Handle Modbus communication tasks
  mb.task();

  // Delay before the next read
  delay(1000);
}
