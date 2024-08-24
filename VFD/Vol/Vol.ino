#include <ModbusRTU.h>
#include <HardwareSerial.h>

// Define the Modbus object
ModbusRTU mb;

// RS485 module connections
#define TX_PIN 17
#define RX_PIN 16
#define RTS_PIN 4

// VFD settings
#define VFD_ID 1          // VFD MODBUS ID
#define VFD_ADDRESS 40002 // Register address for output voltage

HardwareSerial RS485Serial(1);

void setup() {
  // Start the serial monitor
  Serial.begin(115200);
  
  // Start RS485 serial communication
  RS485Serial.begin(19200, SERIAL_8N1, RX_PIN, TX_PIN);

  // Set up Modbus communication
  mb.begin(&RS485Serial, RTS_PIN);
  mb.master();

  // Delay to ensure everything is initialized
  delay(1000);
}

void loop() {
  uint16_t voltage;

  // Read the VFD output voltage
  if (mb.readHreg(VFD_ID, VFD_ADDRESS, &voltage, 1)) {
    // Print the voltage value
    Serial.print("VFD Output Voltage: ");
    Serial.print(voltage);
    Serial.println(" V");
  } else {
    Serial.println("Failed to read VFD voltage");
  }

  // Delay before the next read
  delay(1000);
}

