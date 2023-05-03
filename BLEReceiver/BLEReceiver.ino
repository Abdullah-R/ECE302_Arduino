/**
 * A BLE client example that is rich in capabilities.
 * There is a lot new capabilities implemented.
 * author unknown
 * updated by chegewara
 */

#include <ArduinoBLE.h>
#include <Wire.h>
//#include "BLEScan.h"

// The remote service we wish to connect to.
static char* serviceUUID = "0e913955-814a-4f74-81b9-0d4c309837d1";
// The characteristic of the remote service we are interested in.
static char* charUUID = "c2f1cb79-43f3-4bfc-9e4c-c18a4798ef82";

void updateSpeedOverI2C(char* buffer){ 
  int x = atoi(strtok(buffer,","));
  int y = atoi(strtok(NULL,","));
  int z = atoi(strtok(NULL,","));
  uint8_t speed;
  if (y < 0) {
    speed = 54;
  }
  else {
    speed = (uint8_t)map(y,0,90,54,72);
  }
   
  Serial.println(speed);
  
  Wire.beginTransmission(8); // transmit to device #4
  Wire.write(speed);              // sends one byte  
  Wire.endTransmission();    // stop transmitting
  
}

void setup() {
  Serial.begin(9600);
  //while (!Serial);

  Serial.println("Starting Arduino BLE Client application...");

  // initialize BLE
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  BLE.scanForUuid(serviceUUID);

  Wire.begin(); // join i2c bus (address optional for master)
  Serial.println("Setup Complete");

} // End of setup.

void loop() {
  BLEDevice wand = BLE.available();
  if (wand) {
    // discovered a peripheral, print out address, local name, and advertised service
    Serial.print("Found ");
    Serial.print(wand.localName());
    Serial.println();
    BLE.stopScan();
  }
  wand.connect();
  wand.discoverAttributes();
  while (wand)
    {
      char buffer[16];
      BLEService service = wand.service(serviceUUID);
      Serial.println("Service Acceleration");
      BLECharacteristic accel = service.characteristic(charUUID);
      accel.readValue(buffer, 16);
      updateSpeedOverI2C(buffer);
      delay(50);
    }

} // End of loop
