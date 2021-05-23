/*
 * descrizione
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
#define REMOTEXY_ACCESS_PASSWORD "1234"


// RemoteXY configurate  
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =
  { 255,0,0,8,0,43,0,11,39,0,
  68,49,3,11,43,40,8,164,84,101,
  109,112,101,114,97,116,117,114,97,0,
  68,49,53,11,45,40,8,164,76,117,
  109,105,110,111,115,105,116,195,160,0 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // output variables
  float graphData1;
  float graphData2;

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;
#pragma pack(pop)

/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////

//inizio setup sensore temperatura
#include <OneWire.h> 
#include <DallasTemperature.h> 
// Il filo dati è collegato al pin 2 su Arduino 
#define ONE_WIRE_BUS 11
// Configura un'istanza oneWire per comunicare con qualsiasi dispositivo OneWire (non solo  IC di temperatura Maxim / Dallas) 
OneWire oneWire(ONE_WIRE_BUS); 
// Passa il nostro riferimento oneWire a Dallas Temperature.  
DallasTemperature sensors(&oneWire); 
//fine setup sensore di temperatura

//inizio setup sensore luminosità
int fotoresistenza_Pin = A0 ;
//fine setup sensore luminosità

void setup() 
{
  Serial.begin(9600);
  RemoteXY_Init (); 
  sensors.begin();
}

void loop() 
{ 
  RemoteXY_Handler ();
  int bluetooth_connesso = RemoteXY_isConnected();
  
  sensors.requestTemperatures();
  float temperatura = sensors.getTempCByIndex(0);

  float luminosita = alogRead(fotoresistenza_Pin);
  
  if (bluetooth_connesso == 1) {
    RemoteXY.graphData1 = temperatura;
    RemoteXY.graphData2 = luminosita;
  }
  else {
    Serial.print("la temperatura è: ");
    Serial.println(temperatura);
    Serial.print("la luminosità è: ");
    Serial.println(luminosita);
  }

  delay(1000);
}
