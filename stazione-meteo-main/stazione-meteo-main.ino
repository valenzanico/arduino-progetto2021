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
  { 255,0,0,38,0,108,0,11,39,0,
  68,49,52,-12,49,44,8,233,76,117,
  109,105,110,111,115,105,116,195,160,0,
  68,49,-3,-3,53,35,8,164,84,101,
  109,112,101,114,97,116,117,114,97,0,
  68,50,-1,33,52,31,8,36,135,80,
  114,101,115,115,105,111,110,101,32,97,
  115,115,111,108,117,116,97,0,80,114,
  101,115,115,105,111,110,101,32,114,101,
  108,97,116,105,118,97,0,67,5,64,
  38,20,5,2,26,11,67,5,64,50,
  20,5,2,26,11 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // output variables
  float graphData2;
  float graphData1;
  float graphData3_var1;
  float graphData3_var2;
  char text_1[11];  // string UTF8 end zero 
  char text_2[11];  // string UTF8 end zero 

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
DallasTemperature sensore_temperatura(&oneWire); 
//fine setup sensore di temperatura

//inizio setup sensore luminosità
int fotoresistenza_Pin = A0 ;
//fine setup sensore luminosità


//inizio setup sensore di pressione
#include <SFE_BMP180.h>
#include <Wire.h>
SFE_BMP180 sensore_pressione;
#define ALTITUDINE 120.00
int sensore_pressione_stato = 0;
//fine setup sensore di pressione

void setup() 
{
  Serial.begin(9600);
  RemoteXY_Init (); //inizializzazzione remotexy per bluetooth
  sensore_temperatura.begin();//inizializzazzione sensore temperatura 
  if (sensore_pressione.begin()) //inizializzazzione sensore pressione
  {
    sensore_pressione_stato = 1;//se l'inizializzazzione ha successo la varibail va a 1
  }
  else 
  {
    sensore_pressione_stato = 0;//se c'è un problema invce va a 0
  }
}

void loop() 
{ 
  RemoteXY_Handler ();
  int bluetooth_connesso = RemoteXY_isConnected();//controllo connessione bluetooth
  
  sensore_temperatura.requestTemperatures();
  float temperatura = sensore_temperatura.getTempCByIndex(0); //lettura temperatura 

  float luminosita = analogRead(fotoresistenza_Pin);//lettura sensore luminosità

  float pressione;
  float pressione_mare;
  if (sensore_pressione_stato == 1) //se il sensore si è avviato
  {
    char pressione_status;
    double Pressure;
    double Pressure_mare;
    double Pressure_temp = (double) temperatura;//conversione temperatura in double
    pressione_status = sensore_pressione.startPressure(2);
  if (pressione_status != 0){
    delay(pressione_status);
    pressione_status = sensore_pressione.getPressure(Pressure, Pressure_temp);//lettura sesnore pressione
    if (pressione_status != 0) {
      Pressure_mare = sensore_pressione.sealevel(Pressure, ALTITUDINE);//conversione in press. assoluta
      pressione = (float) Pressure;//conversione dati in float
      pressione_mare = (float) Pressure_mare;
    }
  }
  }

  
  
  if (bluetooth_connesso == 1) //se il chip bluetooth è connesso
  {//invia i dati allo smartphone
    RemoteXY.graphData1 = temperatura;
    RemoteXY.graphData2 = luminosita;
    if (pressione)
    {
     RemoteXY.graphData3_var1 = pressione;
     RemoteXY.graphData3_var2 = pressione_mare;
    }
    strcpy(RemoteXY.text_1, "test :)");
  }
  else //altimenti usa il serial monitor
  {
    Serial.print("la temperatura è: ");
    Serial.println(temperatura);
    Serial.print("la luminosità è: ");
    Serial.println(luminosita);
    if (pressione)
    {
      Serial.print("la pressione relativa è: ");
      Serial.println(pressione);
      Serial.print("la pressione assoluta è: ");
      Serial.println(pressione_mare);
    }
    for (int n = 0; n <3; n++) 
    {
    Serial.println();
    }
  }
  delay(1000);
}
