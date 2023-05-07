#include <ArduinoBLE.h>
#include <Wire.h>
#include <SPI.h>
#include "DW1000Ranging.h"

// The remote service we wish to connect to.
static char* serviceUUID = "0e913955-814a-4f74-81b9-0d4c309837d1";
// The characteristic of the remote service we are interested in.
static char* charUUID = "c2f1cb79-43f3-4bfc-9e4c-c18a4798ef82";

//range
float range = 100;

int vals[3] = {-90,-90,-90};

//STATE Variables
bool homing = false;
bool setDWM = false;
bool scanning = false;

void updateSpeedOverI2C(char* buffer){ 
  int x = atoi(strtok(buffer,","));
  int y = atoi(strtok(NULL,","));
  int z = atoi(strtok(NULL,","));
  vals[0] = x;
  vals[1] = y;
  vals[2] = z;
  uint8_t speed;
  if (x > 20) {
    speed = 30;
  }
  else if (x > 0) {
    speed = (uint8_t)map(x, 0, 30, 60, 30);
  }
  else if (x >= -93) {
    speed = (uint8_t)map(x,-93,0,35,60);
  }

  Serial.print("X: ");Serial.println(x);
  Serial.print("Speed: ");Serial.println(speed);
  Wire.beginTransmission(8); // transmit to device #4
  Wire.write(speed);              // sends one byte  
  Wire.endTransmission();    // stop transmitting
  //delay(10);
}

void updateSpeedOverI2CDWM(){ 
  uint8_t speed;
  if(range < 2) speed = 0;
  else speed = 30;

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

  Wire.begin(); // join i2c bus (address optional for master)

  Serial.println("Setup Complete");

} // End of setup.

void loop() {
  Serial.println("Loop");
  if(homing && setDWM) {
    loop_DWM();
    updateSpeedOverI2CDWM();
    Serial.println("A DWM");
  }
  else if(homing){
    // Initialize DWM Module
    SPI.begin();
    setup_DWM();
    Serial.println("HOMING ACTIVATED");
    Serial.println("Started DWM");
    setDWM = true;
  }else{

    if(!scanning){
      BLE.scanForUuid(serviceUUID);
      scanning = true;
    }

    BLEDevice wand = BLE.available();
    if (wand && scanning) {
      // discovered a peripheral, print out address, local name, and advertised service
      Serial.print("Found ");
      Serial.print(wand.localName());
      Serial.println();
      BLE.stopScan();
      scanning = false;
    }
    wand.connect();
    wand.discoverAttributes();

    BLEService service = wand.service(serviceUUID);
    BLECharacteristic accel = service.characteristic(charUUID);
    while (wand)
    {
      Serial.println(0);
      char buffer[16];
      int check = accel.readValue(buffer, 16);
      Serial.println(1);
      updateSpeedOverI2C(buffer);
      Serial.println(2);
      sprintf(buffer, "%i,%i,%i, ", vals[0], vals[1], vals[2]);
      Serial.println(buffer);
      Serial.println(3);

      if(vals[0] == 0 && vals[1] == 0 && vals[2] == 0){
        homing = true;
        break;
      }
      if (!check) {
        break;
        scanning = false;
      }
    }
  }
} // End of loop
