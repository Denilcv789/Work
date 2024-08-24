  #include <Wire.h>
  #include <Adafruit_ADS1015.h>
  #include <WiFi.h>
  #include <ESPmDNS.h>
  #include <WiFiUdp.h>
  #include <ArduinoOTA.h>
  #include <HTTPClient.h>

  Adafruit_ADS1115 ads;  // Create an instance of the ADS1115 ADC
  float V;
  float P = 0.0;
  float D = 0.0;
  float voltageReadings[30];
  int readingsIndex = 0;
  float voltageSum = 0.0;
  int mosfetpin = 2;  // Pin connected to the MOSFET gate
  bool mosfetState = false;  // Flag variable to track the state of the MOSFET

  const char* ssid = "Mmm";
  const char* password = "asdfghjkl";
  const char* server = "api.thingspeak.com";
  const String apiKey = "LP9ZWK9KVNJ9M6CH";
  const int sensorPin = 32;

  float calibration = 10.2; // Change this value to calibrate pH sensor
  const int analogInPin = 12;
  int sensorValue = 0;
  unsigned long int avgValue;
  float b;
  int buf[10], temp;

  // Function to connect to Wi-Fi
  void connectWiFi() {
    Serial.print("Connecting to Wi-Fi");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nWiFi connected");
  }

  void setup() {
    Serial.begin(115200);
    Wire.begin();
    ads.setGain(GAIN_TWOTHIRDS); // Set the gain to 2/3, which corresponds to a range of ±6.144V
    ads.begin();                // Initialize the ADS1115
    pinMode(mosfetpin, OUTPUT);  // Set the MOSFET pin as an output
    Serial.println("ADS1115 Example");

    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);
    while (!Serial);

    Serial.println("Hydrostatic Pressure Sensor Reading:");

    connectWiFi();

    // Setup OTA
    ArduinoOTA.setHostname("ESP32OTA");
    ArduinoOTA.begin();
    pinMode(mosfetpin,OUTPUT);
  }

  void loop() {
    ArduinoOTA.handle();

    // Read pressure, distance, and control MOSFET
    int16_t adc0 = ads.readADC_SingleEnded(0);
    float voltage = (adc0 * 0.1875) / 1000.0;

    // Store the current voltage reading in the array
    voltageReadings[readingsIndex] = voltage;

    // Increment the readings index
    readingsIndex++;

    // If the index reaches 30, wrap it back to 0
    if (readingsIndex >= 30) {
      readingsIndex = 0;
    }

    // Calculate the sum of the last 30 readings
    voltageSum = 0.0;
    for (int i = 0; i < 30; i++) {
      voltageSum += voltageReadings[i];
    }
    
    // Calculate the average voltage
    float averageVoltage = voltageSum / 30;

    Serial.print("AIN0: ");
    Serial.print(adc0);
    Serial.print("\tAverage Voltage: ");
    Serial.print(averageVoltage, 3);
    Serial.println();

    P = averageVoltage * 400;
    D = (P * 9.80665) - 1938;

    Serial.print("\tADC0: ");
    Serial.println(adc0);
    Serial.print("\tVoltage: ");
    Serial.print(voltage, 3); // Print voltage with 3 decimal places
    Serial.println(" V");
    Serial.print("\tPressure: ");
    Serial.println(P, 2);
    Serial.print("\tDistance: ");
    Serial.println(D, 2);
    Serial.println();

    // Read pH sensor
    for (int i = 0; i < 10; i++) {
      buf[i] = analogRead(analogInPin);
      delay(30);
    }
    for (int i = 0; i < 9; i++) {
      for (int j = i + 1; j < 10; j++) {
        if (buf[i] > buf[j]) {
          temp = buf[i];
          buf[i] = buf[j];
          buf[j] = temp;
        }
      }
    }
    avgValue = 0;
    for (int i = 2; i < 8; i++)
      avgValue += buf[i];
    float pHVol = (float)avgValue * 5.0 / 1024 / 6;
    float phValue = -5.70 * pHVol + calibration;

    Serial.print("pH sensor = ");
    Serial.println(phValue);
    delay(500);

    // Control MOSFET based on a condition
    if (D > 20.0 && !mosfetState) { // Distance greater than 20 and MOSFET is not already HIGH
      digitalWrite(mosfetpin, HIGH);  // Turn on the MOSFET
      mosfetState = true;  // Update MOSFET state
    } else if (D < 15.0 && mosfetState) { // Distance less than 15 and MOSFET is not already LOW
      digitalWrite(mosfetpin, LOW);  // Turn off the MOSFET
      mosfetState = false;  // Update MOSFET state
    }

    // Send data to ThingSpeak using HTTP POST
    if (WiFi.isConnected()) {
      HTTPClient http;
      String url = "http://";
      url += server;
      url += "/update?api_key=";
      url += apiKey;
      url += "&field1=";
      url += String(voltage, 3); // Send average voltage with 3 decimal places
      url += "&field2=";
      url += String(P, 2); // Send pressure with 2 decimal places
      url += "&field3=";
      url += String(D, 2); // Send distance with 2 decimal places
      url += "&field4=";
      url += String(phValue, 2); // Send pH value with 2 decimal places

      http.begin(url);

      int httpCode = http.GET(); // Use HTTP GET to send data

      if (httpCode > 0) {
        Serial.printf("HTTP Status: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          Serial.println(payload);
        }
      } else {
        Serial.println("HTTP request failed.");
      }

      http.end();
    } else {
      Serial.println("WiFi not connected. Trying to reconnect...");
      connectWiFi();
    }

    delay(3000); // Wait for 3 seconds
  }
