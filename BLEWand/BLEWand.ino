#include <ArduinoBLE.h>
#include <Arduino_LSM6DS3.h>
#include <SPI.h>
#include "DW1000Ranging.h"

// Accelerometer
float x, y, z;
float range = 100;
int prev_prev_xi = -90;
int prev_xi = -90;
bool homing = false;
bool setDWM = false;

BLEService posService("0e913955-814a-4f74-81b9-0d4c309837d1"); // create service

// create switch characteristic and allow remote device to read and write
BLECharacteristic posCharacteristic("c2f1cb79-43f3-4bfc-9e4c-c18a4798ef82", BLERead , "010,010,010");

void setup() {
  Serial.begin(9600);
  delay(1000);
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

  posCharacteristic.writeValue("010,010,010");

  // start advertising
  BLE.advertise();

  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
  if(homing && setDWM){
    BLE.poll();
    loop_DWM();
  } else if (homing){
    BLE.poll();
    setup_DWM();
    
    char buffer[16];
    sprintf(buffer, "%i,%i,%i, ", 0, 0, 0);
    Serial.println(buffer);
    posCharacteristic.writeValue(buffer, 16, true);

    Serial.println("HOMING ACTIVATED");
    Serial.println("Started DWM");

    setDWM = true;
  } else {
    // poll for BLE events
    BLE.poll();
    char buffer[16];
    
    if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(x, y, z);

      int xi = map(x*100, -100, 97, -90, 90);
      int yi = map(y*100, -100, 97, -90, 90);
      int zi = map(z*100, -100, 97, -90, 90);

      if (xi > 70) {
        homing = true;
      }
      int xi_avg = (xi + prev_xi + prev_prev_xi) / 3;
      prev_prev_xi = prev_xi;
      prev_xi = xi;

      sprintf(buffer, "%i,%i,%i, ", xi_avg, yi, zi);
      Serial.println(buffer);
      

      posCharacteristic.writeValue(buffer, 16, true);
      
    }
  }

}
