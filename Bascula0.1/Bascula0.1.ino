#include "HX711.h"

#include <SPI.h>
#include <Ethernet.h> //Conexion Ethernet con carcasa W5100
#include <PubSubClient.h> //Conexion MQTT
//#include <avr/wdt.h> // Incluir la librería de ATmel para poder reiniciar remotamente

//VARIABLES i objectes de la xarxa i client Mosquitto
byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xFF, 0x08};//mac del arduino
IPAddress ip(192, 168, 68, 227); //Ip fija del arduino
IPAddress server(192, 168, 68, 55); //Ip del server de mosquitto

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

int ledBlanc = 6;
int ledRoig = 7;

boolean obri = false; //Variable per assignar el estat del electroiman (porta)
boolean manual = false; //Variable per canviar al mode manual i poder obrir i tancar la porta remotament

// Objeto HX711
HX711 bascula;

void(* resetFunc) (void) = 0; //declare reset function @ address 0

void setup() {

  #ifdef DEBUG_HX711
    // Iniciar comunicación serie
    Serial.begin(9600);
    Serial.println("[HX7] Inicio del sensor HX711");
  #endif

   //MQTT
  client.setServer(server, 1883);
  client.setCallback(callback);

  //Ethernet.begin(mac);   
  Ethernet.begin(mac,ip);
  
  delay(100); //Cambio de 1000 a 100
  #ifdef DEBUG_HX711
    Serial.println(F("connectant..."));
    Serial.println(Ethernet.localIP());
    Serial.println(F("Definició dels PINS dels LEDS i RELE"));
  #endif
  
  pinMode(ledBlanc,OUTPUT);
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


void callback(char* topic, byte* payload, unsigned int length) {
  #ifdef DEBUG_HX711
    Serial.print(F("Message arrived"));
    Serial.println(topic);
  #endif
  int res=strcmp(topic,"sala2/reset");
  if (res == 0) {    //RESET para toda la sala
     //wdt_enable(WDTO_15MS); // Configuramos el contador de tiempo para que se reinicie en 15ms      
     resetFunc();
  }
  res=strcmp(topic,"sala2/resetBascula");
  if (res == 0) {    //RESET para toda la sala
     //wdt_enable(WDTO_15MS); // Configuramos el contador de tiempo para que se reinicie en 15ms      
     resetFunc();
  }
  res=strcmp(topic,"sala2/basculaObrir");
  if (res == 0) {    //OBRIR comporta, encendre llum blanca i apagar roja
      if (!obri) {
        #ifdef DEBUG_HX711
          Serial.print("ENTRA en !obri");
        #endif  
        digitalWrite(ledBlanc,HIGH);
        digitalWrite(ledRoig,LOW);
        digitalWrite(releElectro,LOW);     
      }
      obri=true;   
  }
  res=strcmp(topic,"sala2/basculaTancar");
  if (res == 0) {     
     if (obri) {
      #ifdef DEBUG_HX711
        Serial.print("ENTRA en obri");
      #endif
      digitalWrite(ledBlanc,LOW);
      digitalWrite(ledRoig,HIGH);
      digitalWrite(releElectro,HIGH);       
    }
    obri=false;
  }
  res=strcmp(topic,"sala2/basculaModoManual");
  if (res == 0) {
    manual=true;
  }
  res=strcmp(topic,"sala2/basculaModoAutomatic");
  if (res == 0) {
    manual=false;
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    #ifdef DEBUG_HX711
      Serial.print(F("Attempting MQTT connection..."));
    #endif
    // Attempt to connect
    if (client.connect("Bascula")) {
      //Serial.println(F("connected"));      
      #ifdef DEBUG_HX711 
        Serial.print(F("Subscribe to reset resetBascula"));
      #endif
      client.subscribe("sala2/reset");
      client.subscribe("sala2/resetBascula");  
      client.subscribe("sala2/basculaObrir");
      client.subscribe("sala2/basculaTancar");
      client.subscribe("sala2/basculaModoManual");
      client.subscribe("sala2/basculaModoAutomatic");
    
    } else {
      #ifdef DEBUG_HX711
        Serial.print(F("failed, rc="));
         Serial.print(client.state());
        Serial.println(" try again in 1 second");
      #endif
      // Wait 1 seconds before retrying
      delay(1000);
    }
  }
}




void loop() {

  float peso = bascula.get_units(15);  
  
  #ifdef DEBUG_HX711
    Serial.print("[HX7] Leyendo: ");  
    Serial.print(peso);
    Serial.print(" Kg");
    Serial.println();
  #endif

// El pes exacte de la relíquia és 4.21 Kg <> ¿4.24?
// MODO NO FUNCIONA EL RESET
/*
  if ( !manual) {
    if ( peso > 3.9 && peso < 4.4) {
      #ifdef DEBUG_HX711
        Serial.print("ENTRA en peso = 3.9 - 4.4");
      #endif
      
      if ( !obri) {
        #ifdef DEBUG_HX711
          Serial.print("ENTRA en !obri");
        #endif  
        digitalWrite(ledBlanc,HIGH);
        digitalWrite(ledRoig,LOW);
        digitalWrite(releElectro,LOW);     
      }
      obri=true;
    }else {
      #ifdef DEBUG_HX711
        Serial.print("ENTRA en peso <> 3.9 - 4.4");
      #endif
      
      if (obri) {
        #ifdef DEBUG_HX711
          Serial.print("ENTRA en obri");
        #endif
        digitalWrite(ledBlanc,LOW);
        digitalWrite(ledRoig,HIGH);
        digitalWrite(releElectro,HIGH);       
      }
      obri=false;
    }
  }
*/

  if ( !manual) {
    if ( peso > -0.25 && peso < 0.25) {
      #ifdef DEBUG_HX711
        Serial.print("ENTRA en peso = -0.25 ... 0.25");
      #endif
      
      if ( !obri) {
        #ifdef DEBUG_HX711
          Serial.print("ENTRA en !obri");
        #endif  
        digitalWrite(ledBlanc,HIGH);
        digitalWrite(ledRoig,LOW);
        digitalWrite(releElectro,LOW);     
      }
      obri=true;
    }else {
      #ifdef DEBUG_HX711
        Serial.print("ENTRA en peso <> -0.25 ... 0.25");
      #endif
      
      if (obri) {
        #ifdef DEBUG_HX711
          Serial.print("ENTRA en obri");
        #endif
        digitalWrite(ledBlanc,LOW);
        digitalWrite(ledRoig,HIGH);
        digitalWrite(releElectro,HIGH);       
      }
      obri=false;
    }
  }
 
  if (!client.connected()) {
    reconnect();
  }  
  
  client.loop();
  
  
}
