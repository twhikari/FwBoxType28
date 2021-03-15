//
// Copyright (c) 2020 Fw-Box (https://fw-box.com)
// Author: CHEN GUANG WU
//
// Description :
//   None
//
// Connections :
//
// Required Library :
//

#include "FwBox.h"
#include "FwBox_PMSX003.h"
#include "SoftwareSerial.h"

#define DEVICE_TYPE 28
#define FIRMWARE_VERSION "1.0.1"

#define ANALOG_VALUE_DEBOUNCING 8

//
// Debug definitions
//
#define FW_BOX_DEBUG 0

#if FW_BOX_DEBUG == 1
  #define DBG_PRINT(VAL) Serial.print(VAL)
  #define DBG_PRINTLN(VAL) Serial.println(VAL)
  #define DBG_PRINTF(FORMAT, ARG) Serial.printf(FORMAT, ARG)
  #define DBG_PRINTF2(FORMAT, ARG1, ARG2) Serial.printf(FORMAT, ARG1, ARG2)
#else
  #define DBG_PRINT(VAL)
  #define DBG_PRINTLN(VAL)
  #define DBG_PRINTF(FORMAT, ARG)
  #define DBG_PRINTF2(FORMAT, ARG1, ARG2)
#endif


SoftwareSerial SerialSensor(13, 15); // RX:D7, TX:D8

FwBox_PMSX003 Pms(&SerialSensor);

unsigned long ReadingTime = 0;

void setup()
{
  Serial.begin(9600);

  //
  // Initialize the fw-box core (early stage)
  //
  fbEarlyBegin(DEVICE_TYPE, FIRMWARE_VERSION);

  //
  // Initialize the fw-box core
  //
  fbBegin(DEVICE_TYPE, FIRMWARE_VERSION);

  //
  // Initialize the PMSX003 Sensor
  //
  Pms.begin();

} // void setup()

void loop()
{
  if((millis() - ReadingTime) > 2000) {
    //
    // Read the sensors
    //
    if(read() == 0) { // Success
      if((Pms.pm1_0() == 0) && (Pms.pm2_5() == 0) && (Pms.pm10_0() == 0)) {
        DBG_PRINTLN("Invalid values");
      }
      else {
        FwBoxIns.setValue(0, Pms.pm1_0());
        FwBoxIns.setValue(1, Pms.pm2_5());
        FwBoxIns.setValue(2, Pms.pm10_0());
      }
    }

    ReadingTime = millis();
  }

  //
  // Run the handle
  //
  fbHandle();

} // END OF "void loop()"

uint8_t read()
{
  //
  // Running readPms before running pm2_5, temp, humi and readDeviceType.
  //
  if(Pms.readPms()) {
    if(Pms.readDeviceType() == FwBox_PMSX003::PMS3003) {
      DBG_PRINTLN("PMS5003 is detected.");
      DBG_PRINT("PM1.0=");
      DBG_PRINTLN(Pms.pm1_0());
      DBG_PRINT("PM2.5=");
      DBG_PRINTLN(Pms.pm2_5());
      DBG_PRINT("PM10=");
      DBG_PRINTLN(Pms.pm10_0());
      return 0; // Success
    }
  }

  DBG_PRINTLN("PMS data format is wrong.");

  return 1; // Error
}
