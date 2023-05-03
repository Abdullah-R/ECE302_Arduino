#include <ArduinoBLE.h>
#include <Arduino_LSM6DS3.h>


float x, y, z;


BLEService posService("0e913955-814a-4f74-81b9-0d4c309837d1"); // create service

// create switch characteristic and allow remote device to read and write
BLECharacteristic posCharacteristic("c2f1cb79-43f3-4bfc-9e4c-c18a4798ef82", BLERead , "000,000,000");

void setup() {
  Serial.begin(9600);
  //while (!Serial);

  // initialize BLE
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }
  // initialize IMU
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");

    while (1);
  }

  // set the local name peripheral advertises
  BLE.setLocalName("MagicWand");
  BLE.setDeviceName("MagicWand");
  // set the UUID for the service this peripheral advertises:
  BLE.setAdvertisedService(posService);

  // add the characteristics to the service
  posService.addCharacteristic(posCharacteristic);

  // add the service
  BLE.addService(posService);

  posCharacteristic.writeValue("000,000,000");

  // start advertising
  BLE.advertise();

  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
  // poll for BLE events
  BLE.poll();
  char buffer[16];
  
  if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(x, y, z);
  
      int xi = map(x*100, -100, 97, -90, 90);
      int zi = map(y*100, -100, 97, -90, 90);
      int yi = map(z*100, -100, 97, -90, 90);

      sprintf(buffer, "%i,%i,%i, ", xi, yi, zi);

      Serial.println(buffer);
      posCharacteristic.writeValue(buffer, 16, true);
      
    }

}
