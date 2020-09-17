#include "HX711.h"

#include <SPI.h>
#include <Ethernet.h> //Conexion Ethernet con carcasa W5100
#include <PubSubClient.h> //Conexion MQTT

//VARIABLES i objectes de la xarxa i client Mosquitto
byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xFF, 0x08};//mac del arduino
IPAddress ip(192, 168, 68, 27); //Ip fija del arduino
IPAddress server(192, 168, 68, 1); //Ip del server de mosquitto

EthernetClient ethClient; //Interfaz de red ethernet
PubSubClient client(ethClient); //cliente MQTT

#define DEBUG_HX711

// Parámetro para calibrar el peso y el sensor
#define CALIBRACION 20880.0
//#define CALIBRACION 240000.0

// Pin de datos y de reloj
byte pinData = A0;
byte pinClk = A1;

int releElectro = 5;

int ledVerd = 6;
int ledRoig = 7;

boolean abre = false;

// Objeto HX711
HX711 bascula;

void setup() {

#ifdef DEBUG_HX711
  // Iniciar comunicación serie
  Serial.begin(9600);
  Serial.println("[HX7] Inicio del sensor HX711");
#endif

   //MQTT
  /*client.setServer(server, 1883);
  client.setCallback(callback);

  //Ethernet.begin(mac);   
  Ethernet.begin(mac,ip);
      
  delay(100); //Cambio de 1000 a 100
  if (DEBUG) {
    Serial.println(F("connectant..."));
    Serial.println(Ethernet.localIP());
    Serial.println(F("Definició dels PINS dels LEDS"));
  }*/
  
  pinMode(ledVerd,OUTPUT);
  pinMode(ledRoig,OUTPUT);
  pinMode(releElectro,OUTPUT);  

  // Iniciar sensor
  bascula.begin(pinData, pinClk);
  // Aplicar la calibración
  bascula.set_scale(CALIBRACION);
  // Iniciar la tara
  // No tiene que haber nada sobre el peso
  bascula.tare();
}

/*
void callback(char* topic, byte* payload, unsigned int length) {
  if (DEBUG) {
    Serial.print(F("Message arrived"));
    Serial.println(topic);
  }
  int res=strcmp(topic,"sala2/reset");
  if (res == 0) {    //RESET para toda la sala
     wdt_enable(WDTO_15MS); // Configuramos el contador de tiempo para que se reinicie en 15ms      
  }
  res=strcmp(topic,"sala2/resetBascula");
  if (res == 0) {    //RESET para toda la sala
     wdt_enable(WDTO_15MS); // Configuramos el contador de tiempo para que se reinicie en 15ms      
  }
  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    if (DEBUG) Serial.print(F("Attempting MQTT connection..."));
    // Attempt to connect
    if (client.connect("Pedestal")) {
      //Serial.println(F("connected"));      

      if (DEBUG) Serial.print(F("Subscribe to reset resetBascula"));
      client.subscribe("sala2/reset"); 
      client.subscribe("sala2/resetBascula");  
    
    } else {
      if (DEBUG) {
        Serial.print(F("failed, rc="));
        Serial.print(client.state());
        Serial.println(" try again in 1 second");
      }
      // Wait 1 seconds before retrying
      delay(1000);
    }
  }
}

*/


void loop() {

  float peso = bascula.get_units(12);  
  //if (peso == -(0.0)) peso = 0.0;

#ifdef DEBUG_HX711
  Serial.print("[HX7] Leyendo: ");  
  Serial.print(peso);
  Serial.print(" Kg");
  Serial.println();
#endif



  if ( peso > 4.0 && peso < 4.5) {
    #ifdef DEBUG_HX711
      Serial.print("ENTRA en peso = 4.0 - 4.5");
    #endif
    
    if ( !abre) {
      #ifdef DEBUG_HX711
        Serial.print("ENTRA en !abre");
      #endif  
      digitalWrite(ledVerd,HIGH);
      digitalWrite(ledRoig,LOW);
      digitalWrite(releElectro,LOW);     
    }
    abre=true;
  }else {
    #ifdef DEBUG_HX711
      Serial.print("ENTRA en peso <> 4.0 - 4.5");
    #endif
    
    if (abre) {
      #ifdef DEBUG_HX711
        Serial.print("ENTRA en abre");
      #endif
      digitalWrite(ledVerd,LOW);
      digitalWrite(ledRoig,HIGH);
      digitalWrite(releElectro,HIGH);       
    }
    abre=false;
  }


 /*
  if (!client.connected()) {
    reconnect();
  }  
  
  client.loop();
  */
  
}
