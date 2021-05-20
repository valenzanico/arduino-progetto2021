/*
   -- New project --
   
   This source code of graphical user interface 
   has been generated automatically by RemoteXY editor.
   To compile this code using RemoteXY library 2.4.3 or later version 
   download by link http://remotexy.com/en/library/
   To connect using RemoteXY mobile app by link http://remotexy.com/en/download/                   
     - for ANDROID 4.7.12 or later version;
     - for iOS 1.4.7 or later version;
    
   This source code is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.    
*/

//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// RemoteXY select connection mode and include library 
#define REMOTEXY_MODE__SOFTSERIAL
#include <SoftwareSerial.h>

#include <RemoteXY.h>

// RemoteXY connection settings 
#define REMOTEXY_SERIAL_RX 2
#define REMOTEXY_SERIAL_TX 3
#define REMOTEXY_SERIAL_SPEED 9600


// RemoteXY configurate  
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =
  { 255,0,0,26,0,50,0,11,39,0,
  130,0,2,18,26,25,109,130,0,38,
  18,25,25,109,130,1,70,19,28,24,
  17,68,17,70,19,28,24,8,164,67,
  5,5,28,20,5,164,26,11,67,5,
  40,28,20,5,164,26,11 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // output variables
  float graphData1;
  char value1[11];  // string UTF8 end zero 
  char value2[11];  // string UTF8 end zero 

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;
#pragma pack(pop)

/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////

//librerie sensore temperatura
#include <OneWire.h> 
#include <DallasTemperature.h> 
// Il filo dati è collegato al pin 2 su Arduino 
#define ONE_WIRE_BUS 2 
// Configura un'istanza oneWire per comunicare con qualsiasi dispositivo OneWire (non solo  IC di temperatura Maxim / Dallas) 
OneWire oneWire(ONE_WIRE_BUS); 
// Passa il nostro riferimento oneWire a Dallas Temperature.  
DallasTemperature sensors(&oneWire); 

void setup() 
{
  Serial.begin(9600);
  RemoteXY_Init (); 
  sensors.begin();
  
  // TODO you setup code
  
}

void loop() 
{ 
  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);
  RemoteXY_Handler ();
  char display_temp;
  dtostrf(temperature,7, 3, display_temp);
  RemoteXY.value1 = display_temp;
  
  // TODO you loop code
  // use the RemoteXY structure for data transfer
  // do not call delay() 
  delay(1000);
}
