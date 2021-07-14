 /*
 * Arduino UNO (pared trasera cara veritat, darrere bascula)
 * 
 * 4 reles
 *   1 càmera wifi (220V)
 *   1 llum ultravioleta (220V)
 *   1 llum leds ultravioleta (12V)
 *   1 electroiman porta corredera vertical (12V)
 * Llum leds blancs interior bascula
 * Final de carrera (iman) porta corredera
 * Electroiman comporta superior bascula
 * Sensor de pes HX711 (bàscula)
 * 
 ***********************
 * 
 * Per resetejar el dispositiu
 *  sala2/reset
 *  sala2/resetBascula
 * 
 * Per controlar electroiman porta i la llum leds blanca
 * 
 *  sala2/activaBascula (canvia de estat=0 a estat=1)
 *  sala2/basculaObrir
 *  sala2/basculaTancar
 *  sala2/basculaModoManual (canvia manual=0 a manual=1)
 *  sala2/basculaModoAutomatic (canvia manual=1 a manual=0)
 *  sala2/desactivaBascula (canvia de estat=1 a estat=0)
 *  sala2/apagaUltra12
 *  sala2/encenUltra12
 *  sala2/apagaUltra220 
 *  sala2/encenUltra220
 *  sala2/tancaPortaCara
 *  sala2/obriPortaCara
 *  sala2/tancaPortaFinalBascula
 *  sala2/obriPortaFinalBascula
 *  
 * Per canviar a la segona prova bascula 
 *  sala2/detectaCor
 *  
 *  EVENTS QUE ENVIA
 *  
 *  sala2/tancaPortaCaraVeritat
 *  sala2/puzzleFinalBascula 
 *  
 */


#include "HX711.h"

#include <SPI.h>
#include <Ethernet.h> //Conexion Ethernet con carcasa W5100
#include <PubSubClient.h> //Conexion MQTT

//VARIABLES i objectes de la xarxa i client Mosquitto
byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xFF, 0xAA};//mac del arduino
IPAddress ip(192, 168, 68, 200); //Ip fija del arduino
IPAddress server(192, 168, 68, 55); //Ip del server de mosquitto

EthernetClient ethClient; //Interfaz de red ethernet
PubSubClient client(ethClient); //cliente MQTT

//#define DEBUG_HX711

// Parámetro para calibrar el peso y el sensor
#define CALIBRACION 20880.0
//#define CALIBRACION 240000.0

// Pin de datos y de reloj
byte pinData = A0;
byte pinClk = A1;

const int releElectroIman = 5; //encén els leds i tanca electroiman o al revés

const int releImanPorta = 7; //activa el electroiman per tancar porta
const int releUltravioleta12 = 6; // encén la llum ultavioleta 12V
const int releUltravioleta220 = 8; //encén la llum ultravioleta 220V
const int releCameraWifi = 9; // encén la càmera wifi

const int ledRed = 3;
const int ledGreen = 2;
const int ledBlue = 4;

bool iniciaPasillo=false;

int pinFinalCarrera=0;
int relePuzzleFinalBascula=1;

boolean obri = false; //Variable per assignar el estat del electroiman (porta)
boolean manual = false; //Variable per canviar al mode manual i poder obrir i tancar la porta remotament

// Objeto HX711
HX711 bascula;

void(* resetFunc) (void) = 0; //declare reset function @ address 0

int estat = 0; //variable para definir el estado del arduino
                // 0 --> No interactua con nada... Electroiman encendido, led apagado, luces led apagadas
                // 1 --> Prueba1 ... Electroiman y led en función del peso de la báscula
                // 2 --> Prueba2 ... Prova final

void setup() {

  #ifdef DEBUG_HX711
    // Iniciar comunicación serie
    Serial.begin(9600);
    Serial.println("[HX7] Inicio del sensor HX711");
  #endif

   //MQTT
  client.setServer(server, 1883);
  client.setCallback(callback);
  
  Ethernet.begin(mac,ip);
  
  delay(100); //Cambio de 1000 a 100
  #ifdef DEBUG_HX711
    Serial.println(F("connectant..."));
    Serial.println(Ethernet.localIP());
    Serial.println(F("Definició dels PINS dels LEDS i RELE"));
  #endif

  pinMode(ledRed,OUTPUT);
  pinMode(ledGreen,OUTPUT);
  pinMode(ledBlue,OUTPUT);

  digitalWrite(ledRed,HIGH);
  digitalWrite(ledGreen,HIGH);
  digitalWrite(ledBlue,HIGH);
  
  pinMode(releElectroIman,OUTPUT);   
  digitalWrite(releElectroIman,HIGH);
  
  pinMode(releImanPorta,OUTPUT);
  delay(3);
  digitalWrite(releImanPorta,HIGH);
  
  pinMode(releUltravioleta12,OUTPUT);
  delay(3);
  digitalWrite(releUltravioleta12,LOW);
  
  pinMode(releUltravioleta220,OUTPUT);
  digitalWrite(releUltravioleta220,HIGH);
  
  pinMode(releCameraWifi,OUTPUT);
  digitalWrite(releCameraWifi,LOW);

  pinMode(pinFinalCarrera,INPUT_PULLUP);

  pinMode(relePuzzleFinalBascula,OUTPUT); 
  digitalWrite(relePuzzleFinalBascula,HIGH); 

  #ifdef DEBUG_HX711
    Serial.println(F("Abans de bascula.begin"));
  #endif
   
  // Iniciar sensor
  bascula.begin(pinData, pinClk);

  #ifdef DEBUG_HX711
    Serial.println(F("Despres de bascula.begin"));
  #endif
  
  // Aplicar la calibración
  bascula.set_scale(CALIBRACION);

  #ifdef DEBUG_HX711
    Serial.println(F("Abans de bascula.tare"));
  #endif
  
  // Iniciar la tara
  bascula.tare();

  #ifdef DEBUG_HX711
    Serial.println(F("Despres de bascula.tare"));
  #endif
   
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

        digitalWrite(ledRed,HIGH);
        digitalWrite(ledGreen,HIGH);
        digitalWrite(ledBlue,HIGH);        
        
        digitalWrite(releElectroIman,HIGH);     
      }
      obri=true;   
  }
  res=strcmp(topic,"sala2/basculaTancar");
  if (res == 0) {     
     if (obri) {
      #ifdef DEBUG_HX711
        Serial.print("ENTRA en obri");
      #endif

      digitalWrite(ledRed,HIGH);
      digitalWrite(ledGreen,LOW);
      digitalWrite(ledBlue,LOW);
      
      digitalWrite(releElectroIman,LOW);       
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

  res=strcmp(topic,"sala2/activaBascula");
  if (res == 0) {
    #ifdef DEBUG_HX711
      Serial.println(F("Estat = 1 activa Bascula "));
    #endif
    digitalWrite(releElectroIman,LOW);
    estat=1;
  }

  res=strcmp(topic,"sala2/desactivaBascula");
  if (res == 0) {
    #ifdef DEBUG_HX711
      Serial.print(F("Estat = 2 desactiva Bascula "));
    #endif
    digitalWrite(releElectroIman,HIGH);

    digitalWrite(ledRed,LOW);
    digitalWrite(ledGreen,LOW);
    digitalWrite(ledBlue,LOW);
    
    estat=0;
    obri=false;
  }

  res=strcmp(topic,"sala2/tancaPortaCara");
  if (res==0) {
     #ifdef DEBUG_HX711
      Serial.print(F("Entra a Tanca Porta Cara Veritat "));
    #endif
    digitalWrite(releImanPorta,HIGH);    
  }

  res=strcmp(topic,"sala2/obriPortaCara");
  if (res==0) {
     #ifdef DEBUG_HX711
      Serial.print(F("Entra a OBRI Porta Cara Veritat "));
    #endif
    digitalWrite(releImanPorta,LOW);    
  }

  res=strcmp(topic,"sala2/apagaUltra220");
  if (res==0) {
     #ifdef DEBUG_HX711
      Serial.print(F("Entra a Apaga LLum Ultravioleta 220V"));
    #endif
    digitalWrite(releUltravioleta220, HIGH);    
  }

  res=strcmp(topic,"sala2/encenUltra220");
  if (res==0) {
     #ifdef DEBUG_HX711
      Serial.print(F("Entra a Encen Llum Ultravioleta 220V "));
    #endif
    digitalWrite(releUltravioleta220,LOW);    
  }

  res=strcmp(topic,"sala2/apagaUltra12");
  if (res==0) {
     #ifdef DEBUG_HX711
      Serial.print(F("Entra a Apaga LLum Ultravioleta 12V"));
    #endif
    digitalWrite(releUltravioleta12,LOW);    
  }

  res=strcmp(topic,"sala2/encenUltra12");
  if (res==0) {
     #ifdef DEBUG_HX711
      Serial.print(F("Entra a Encen Llum Ultravioleta 12V"));
    #endif
    digitalWrite(releUltravioleta12,HIGH);    
  }

  res=strcmp(topic,"sala2/apagaCamara");
  if (res==0) {
     #ifdef DEBUG_HX711
      Serial.print(F("Entra a Apaga Camara"));
    #endif
    digitalWrite(releCameraWifi,HIGH);    
  }

  res=strcmp(topic,"sala2/encenCamara");
  if (res==0) {
     #ifdef DEBUG_HX711
      Serial.print(F("Entra a Encen Camara"));
    #endif
    digitalWrite(releCameraWifi,LOW);    
  }    

  res=strcmp(topic,"sala2/detectaCor");
  if (res==0) {
     #ifdef DEBUG_HX711
      Serial.print(F("Entra a detectaCor"));
     #endif             
    
    estat=2;

    digitalWrite(ledRed,HIGH);
    digitalWrite(ledGreen,LOW);
    digitalWrite(ledBlue,LOW);            
  }    

  res=strcmp(topic,"sala2/obriPortaFinalBascula");
  if (res==0){
     #ifdef DEBUG_HX711
      Serial.print(F("Entra a obriPortaFinalBascula"));
     #endif      

     digitalWrite(relePuzzleFinalBascula,LOW);     
  }

  res=strcmp(topic,"sala2/tancaPortaFinalBascula");
  if (res==0){
     #ifdef DEBUG_HX711
      Serial.print(F("Entra a tancaPortaFinalBascula"));
     #endif      

     digitalWrite(relePuzzleFinalBascula,HIGH);     
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
      #ifdef DEBUG_HX711 
        Serial.print(F("Subscribe to reset, resetBascula, basculaObrir, basculaTancar, basculaModoManual, basculaModoAutomatica, activaBascula, desactivaBascula, encenUltra220, encenUltra12, encenCamara"));
      #endif
      client.subscribe("sala2/reset");
      client.subscribe("sala2/resetBascula");  
      client.subscribe("sala2/basculaObrir");
      client.subscribe("sala2/basculaTancar");
      client.subscribe("sala2/basculaModoManual");
      client.subscribe("sala2/basculaModoAutomatic");
      client.subscribe("sala2/activaBascula");
      client.subscribe("sala2/desactivaBascula");
      client.subscribe("sala2/encenUltra220");
      client.subscribe("sala2/encenUltra12");
      client.subscribe("sala2/encenCamara");
      client.subscribe("sala2/apagaUltra220");
      client.subscribe("sala2/apagaUltra12");
      client.subscribe("sala2/apagaCamara");
      client.subscribe("sala2/obriPortaCara");
      client.subscribe("sala2/tancaPortaCara");
      client.subscribe("sala2/detectaCor");
      client.subscribe("sala2/obriPortaFinalBascula");
      client.subscribe("sala2/tancaPortaFinalBascula");
      
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

int cont1=0; //DEBUG FinalCarrera
int cont2=0; //Evitar lectures incorrectes FinalCarreraPortaCaraVeritat
unsigned long controlTemps = 0;
float peso = 0;

void loop() {

  switch(estat){
    case 1:
       #ifdef DEBUG_HX711
         Serial.println("Entra antes de bascula.get_units");
       #endif
    
       peso = bascula.get_units(13);  
  
      #ifdef DEBUG_HX711
        Serial.print("[HX7] Leyendo: ");  
        Serial.print(peso);
        Serial.print(" Kg");
        Serial.println();
      #endif

       if ( !manual) {
        if ( peso > -0.40 && peso < 0.40) {
          #ifdef DEBUG_HX711
            Serial.println("ENTRA en peso = -0.40 ... 0.40");
          #endif
          
          if ( !obri) {
            #ifdef DEBUG_HX711
              Serial.println("ENTRA en !obri");
            #endif  
            
            digitalWrite(ledRed,HIGH);
            digitalWrite(ledGreen,HIGH);
            digitalWrite(ledBlue,HIGH);
                        
            digitalWrite(releElectroIman,LOW);     
            //delay(100); //per evitar rebot
          }
          obri=true;
          //Serial.println("Fin del !obri");
        }else {
          #ifdef DEBUG_HX711
            Serial.print("ENTRA en peso <> -0.40 ... 0.40");
          #endif
          
          if (obri) {
            #ifdef DEBUG_HX711
              Serial.print("ENTRA en obri");
            #endif

            digitalWrite(ledRed,HIGH);
            digitalWrite(ledGreen,LOW);
            digitalWrite(ledBlue,LOW);
            
            digitalWrite(releElectroIman,HIGH);       
            //delay(100); //per evitar rebot
          }
          obri=false;
        }
      }
      break;
    case 2: //Segona prova Bascula PES 3.93 KG
      peso = bascula.get_units(15);  
  
      #ifdef DEBUG_HX711
        Serial.print("[HX7] Leyendo 2: ");  
        Serial.print(peso);
        Serial.print(" Kg");
        Serial.println();
      #endif

       if ( !manual) {
        if ( peso > 3.53 && peso < 4.33) { //> 3.53  < 4.33
          #ifdef DEBUG_HX711
            Serial.print("ENTRA en peso > 3.53 Y < 4.33");
          #endif
          
          if ( !obri) {
            #ifdef DEBUG_HX711
              Serial.print("ENTRA en !obri");
            #endif  
            
            digitalWrite(ledRed,HIGH);
            digitalWrite(ledGreen,HIGH);
            digitalWrite(ledBlue,HIGH);
                        
            //FALTA PROVAR
            client.publish("sala2/puzzleFinalBascula","on");
            digitalWrite(relePuzzleFinalBascula,LOW);
           
          }
          obri=true;
        }else {
          #ifdef DEBUG_HX711
            Serial.print("ENTRA en peso 2-2");
          #endif
          
          if (obri) {
            #ifdef DEBUG_HX711
              Serial.print("ENTRA en obri");
            #endif

            digitalWrite(ledRed,HIGH);
            digitalWrite(ledGreen,LOW);
            digitalWrite(ledBlue,LOW);            
            
            digitalWrite(relePuzzleFinalBascula,HIGH);   
            //FALTA PROVAR
          }
          obri=false;          
        }
      }
      
      break;
  }

  boolean tancaPorta = digitalRead(pinFinalCarrera);
  boolean tancaPortaReal=false;
  
  if (!iniciaPasillo && !tancaPorta){
    if (millis()- controlTemps < 150 ) cont2++;
    else cont2=0;
    controlTemps=millis();
    if (cont2 > 12500) { //VALOR DE PROVA 8500 DEFINITIU DE MOMENT 12500
      tancaPortaReal=true;      
    }
  }
  
  #ifdef DEBUG_HX711
    if ( cont1 == 0 || (cont1 % 5000 == 0)){
      Serial.print(F("tancaPorta: ")); 
      Serial.println(tancaPorta);
      Serial.println("***************");
      Serial.print(F("contadorPortaTancada: "));
      Serial.println(cont2);
    }
    cont1++;
  #endif

  if (tancaPortaReal && !iniciaPasillo) {
    #ifdef DEBUG_HX711      
        Serial.println(F("[FinalCarrera] tanca Porta Cara Veritat"));
    #endif
    //Activem l'electroiman
    client.publish("sala2/tancaPortaCaraVeritat","on");
    
    digitalWrite(releUltravioleta12,HIGH); //Activem la llum ultravioleta
    digitalWrite(releUltravioleta220,LOW); //Activem la llum ultravioleta
    iniciaPasillo=true;   //Per a que no entre de nou en aquest if
  }
 
  if (!client.connected()) {
    reconnect();
  }  
  
  client.loop();  
  
}
