#include <ArduinoBLE.h>
#include <Wire.h>
#include <SPI.h>
#include "DW1000Ranging.h"

// The remote service we wish to connect to.
static char* serviceUUID = "0e913955-814a-4f74-81b9-0d4c309837d1";
// The characteristic of the remote service we are interested in.
static char* charUUID = "c2f1cb79-43f3-4bfc-9e4c-c18a4798ef82";

void updateSpeedOverI2C(char* buffer){ 
  int x = atoi(strtok(buffer,","));
  int y = atoi(strtok(NULL,","));
  int z = atoi(strtok(NULL,","));
  uint8_t speed;
  if (x > 100) {
    speed = 0;
  }
  else if (x > 20) {
    speed = 20;
  }
  else if (x > 0) {
    speed = (uint8_t)map(x, 0, 20, 60, 30);
  }
  else if (x >= -93) {
    speed = (uint8_t)map(x,-90,0,45,60);
  }

  Serial.print("X: ");Serial.println(x);
  Serial.print("Speed: ");Serial.println(speed);
  Wire.beginTransmission(8); // transmit to device #4
  Wire.write(speed);              // sends one byte  
  Wire.endTransmission();    // stop transmitting
  
}

void setup() {
  Serial.begin(9600);
  //while(!Serial);
  delay(1000);

  Serial.println("Starting Arduino BLE Client application...");

  // initialize BLE
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  BLE.scanForUuid(serviceUUID);

  Wire.begin(); // join i2c bus (address optional for master)

  // Initialize DWM Module
  SPI.begin();
  setup_DWM();

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
    BLECharacteristic accel = service.characteristic(charUUID);
    accel.readValue(buffer, 16);
    updateSpeedOverI2C(buffer);
    loop_DWM();
  }


} // End of loop
