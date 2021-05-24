/*
 * Stazione metereologica
 * 
 * Nicolò Valenza, Cassinelli Paolo, Polenta Marco
 * 18/05/2021 - 24/05/2021
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
  { 255,0,0,117,0,99,0,11,39,0,
  68,49,52,-1,66,34,8,233,76,117,
  109,105,110,111,115,105,116,195,160,0,
  68,49,-14,0,66,33,8,164,84,101,
  109,112,101,114,97,116,117,114,97,0,
  68,50,-15,33,67,31,8,36,135,80,
  114,101,115,115,105,111,110,101,32,97,
  115,115,111,108,117,116,97,0,80,114,
  101,115,115,105,111,110,101,32,114,101,
  108,97,116,105,118,97,0,67,1,66,
  45,20,5,2,26,101 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // output variables
  float graphData2;
  float graphData1;
  float graphData3_var1;
  float graphData3_var2;
  char text_1[101];  // string UTF8 end zero 

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;
#pragma pack(pop)

/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////

//inizio setup neopixel ring
#include <Adafruit_NeoPixel.h>
#define PIN        4 //definizione pin neopixel
#define NUMPIXELS 16 //numero led cerchio
Adafruit_NeoPixel cerchio_led(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
//fine setup neopixel ring

//inizio setup sensore temperatura
#include <OneWire.h> 
#include <DallasTemperature.h> 
#define ONE_WIRE_BUS 11 //definizione del pin del sensore
OneWire oneWire(ONE_WIRE_BUS);  
DallasTemperature sensore_temperatura(&oneWire); 
//fine setup sensore di temperatura

//inizio setup sensore luminosità
int fotoresistenza_Pin = A0; //definizione pin sensore
//fine setup sensore luminosità


//inizio setup sensore di pressione
#include <SFE_BMP180.h>
#include <Wire.h>
SFE_BMP180 sensore_pressione;
#define ALTITUDINE 120.00 //definizione altitudine di Milano, necessaria per eseguire la corretta misura della pressione
int sensore_pressione_stato = 0;//variabile sullo sttao del sensore della pressione
//fine setup sensore di pressione

void setup() 
{
  Serial.begin(9600); //velocità di comunicazione con il Serial Monitor
  RemoteXY_Init (); //inizializzazione remotexy per bluetooth
  cerchio_led.begin(); //inizializzazione Neopixel ring
  sensore_temperatura.begin(); //inizializzazione sensore temperatura 
  if (sensore_pressione.begin()) //inizializzazione sensore pressione
  {
    sensore_pressione_stato = 1; //se l'inizializzazione ha successo la varibaile viene impostata a 1
  }
  else 
  {
    sensore_pressione_stato = 0; //se c'è un problema invece viene impostata a 0
  }
}

void loop() 
{ 
  RemoteXY_Handler ();
  int bluetooth_connesso = RemoteXY_isConnected(); //controllo connessione bluetooth
  
  sensore_temperatura.requestTemperatures();
  float temperatura = sensore_temperatura.getTempCByIndex(0); //lettura temperatura 

  float luminosita = analogRead(fotoresistenza_Pin); //lettura sensore luminosità

  float pressione;
  float pressione_mare;
  if (sensore_pressione_stato == 1) //se il sensore si è avviato la variabile è uguale a 1
  {
    /*il sensore di pressione restituisce i dati nel formato double, il formato double è
     * molto simile al formato float
     */
    char pressione_status;
    double Pressure;
    double Pressure_mare;
    double Pressure_temp = (double) temperatura; //conversione temperatura in double
    pressione_status = sensore_pressione.startPressure(2);
  if (pressione_status != 0){
    delay(pressione_status);//aspetta che la misura venga eseguita
    pressione_status = sensore_pressione.getPressure(Pressure, Pressure_temp);//lettura sensore pressione
    if (pressione_status != 0) {
      Pressure_mare = sensore_pressione.sealevel(Pressure, ALTITUDINE);//conversione in press. assoluta
      pressione = (float) Pressure;//conversione dati in float
      pressione_mare = (float) Pressure_mare;
    }
  }
  }

  int led_da_accendere = temperatura / 2.5; //numero dei led che verranno accesi
  cerchio_led.clear(); //si spengono tutti i led
  for(int n=0; n<led_da_accendere; n++) //per ogni led da accendere
  {
    cerchio_led.setPixelColor(n, cerchio_led.Color(0, 255, 0));//imposta il led su verde 
  }
  cerchio_led.show(); //applica le nuove impostazioni

  char testo[100]; //definizione variabile char di caratteri 70
  strcpy(testo, "");
  if (temperatura > 25.00) 
  {
    strcat(testo, "Fa caldo\n");
  }
  else if (temperatura < 15.00)
  {
    strcat(testo, "Fa freddo\n");
  }
  if (luminosita < 450.00)
  {
    strcat(testo, "è giorno\n");
  }
  else if (luminosita >= 450.00)
  {
    strcat(testo, "è notte\n");
  }
  if (pressione_mare < 1014.00) //quando la pressione è minore c'è più probabilità che piova
  {
    strcat(testo, "attento che oggi c'è il rischio di pioggia,\nricordati di prendere l'ombrello");
  }
  else if (pressione_mare > 1014.00) 
  {
    strcat(testo, "probabilmente oggi ci sarà bel tempo");
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
    strcpy(RemoteXY.text_1, testo);
  }
  else //altimenti usa il serial monitor
  {
    Serial.print("la temperatura è: ");
    Serial.print(temperatura);
    Serial.println(" C°");
    Serial.print("la luminosità è: ");
    Serial.println(luminosita);
    if (pressione)
    {
      Serial.print("la pressione relativa è: ");
      Serial.print(pressione);
      Serial.println(" millibar");
      Serial.print("la pressione assoluta è: ");
      Serial.print(pressione_mare);
      Serial.println(" millibar");
      Serial.println(testo);
    }
    for (int n = 0; n <3; n++) 
    {
    Serial.println();
    }
  }
  delay(1000);
}
