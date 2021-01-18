/* TODO 
 *  Revisar codi python Raspberry Pi 4 magatzem (tecla 1,01,2... o tecla1,tecla2...??????)
 * 
 * Crear els estats
 *  estat=0 INICI no fa res -canvia amb el event sala2/activaOrgan-
 *  estat=1 primera prova organ
 *  estat=2 segona prova
 *  estat=3 
 *  estat=4
 *  estat=5 quinta i ultima prova
 *  estat=6 organ desactivat
 * 
 ********************* 
 * 
 * Arduino MEGA (caixo pared magatzem)
 * 
 * 20 entrades (finals de carrera)
 * rele apertura electroiman porta punxos
 * 3 leds (esqueleto)
 **********************
 * 
 * Per reseteja el dispositiu
 *  sala2/reset
 *  sala2/resetOrgan
 *  
 * Per establir dificultat de la sala (futur)
 *  sala2/dificultat
 *  
 * Per activar el organ (canvia estat=0 a estat=1)
 *  sala2/activaOrgan 
 *  
 * Per desactivar organ (canvia estat=1 a estat=0)
 *  sala2/desactivaOrgan
 *  
 * Per establir la prova i obrir porta 
 *  sala2/iniciaPunxos 
 *  sala2/estat2Punxos
 *  sala2/estat3Punxos
 *  sala2/estat4Punxos
 *  sala2/finalPunxos 
 *  
 ***********************  
 *  
 * Events que envia
 *  sala2/tecla1 missatges 1 (tecla pressionada) i 0 (no pressionada)
 *  sala2/tecla2 ...
 *  ....
 *  sala2/tecla20 missatges 1 (tecla pressionada) i 0 (no pressionada)
 *  
 *  sala2/acordCorrecte
 */

#include <SPI.h>
#include <Ethernet.h> //Conexion Ethernet con carcasa W5100
#include <PubSubClient.h> //Conexion MQTT
#include <avr/wdt.h> // Incluir la librería de ATmel per a reiniciar el arduino

//Define per veure els prints al monitor
#define DEBUG_ORGAN  

//Variable per activar o desactivar el organ
boolean actiu=false;

//Variable per seguir el estat de la prova del organ i el sostre de punxos i verificar el acord correcte
int estat=0;

//VARIABLES i objectes de la xarxa i client Mosquitto
byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xFA};//mac del arduino
IPAddress ip(192, 168, 68, 201); //Ip fija del arduino
IPAddress server(192, 168, 68, 56); //Ip del server de mosquitto

EthernetClient ethClient; //Interfaz de red ethernet
PubSubClient client(ethClient); //cliente MQTT 

const int led1 = 3; //Ull dret
const int led2 = 4; //Ull esquerra
const int led3 = 5; //Boca


const int tecla1 = 40; // 40 Definimos el pin de deteccion de llegada
const int tecla2 = 23; //
const int tecla3 = 22; // 22
const int tecla4 = 41; // 41
const int tecla5 = 31; // 31Definimos el pin de deteccion de llegada
const int tecla6 = 29; // 29
const int tecla7 = 30; // 30
const int tecla8 = 28; // 28
const int tecla9 = 36; // 36 Definimos el pin de deteccion de llegada
const int tecla10 = 39; // 39
const int tecla11 = 38; // 38
const int tecla12 = 37; // 37
const int tecla13 = 34; // Definimos el pin de deteccion de llegada
const int tecla14 = 33; // 33
const int tecla15 = 35; // 35
const int tecla16 = 32; // 32
const int tecla17 = 25; // 25 Definimos el pin de deteccion de llegada
const int tecla18 = 27; // 27
const int tecla19 = 26; // 26
const int tecla20 = 24; // 24


boolean AlarmaTecla1; // Definimos el nombre de la variable donde vamos a grabar las lecturas
boolean AlarmaTecla2;
boolean AlarmaTecla3;
boolean AlarmaTecla4;
boolean AlarmaTecla5; // Definimos el nombre de la variable donde vamos a grabar las lecturas
boolean AlarmaTecla6;
boolean AlarmaTecla7;
boolean AlarmaTecla8;
boolean AlarmaTecla9; // Definimos el nombre de la variable donde vamos a grabar las lecturas
boolean AlarmaTecla10;
boolean AlarmaTecla11;
boolean AlarmaTecla12;
boolean AlarmaTecla13; // Definimos el nombre de la variable donde vamos a grabar las lecturas
boolean AlarmaTecla14;
boolean AlarmaTecla15;
boolean AlarmaTecla16;
boolean AlarmaTecla17; // Definimos el nombre de la variable donde vamos a grabar las lecturas
boolean AlarmaTecla18;
boolean AlarmaTecla19;
boolean AlarmaTecla20;


boolean EstadoTecla1=0;
boolean EstadoTecla2=0;
boolean EstadoTecla3=0;
boolean EstadoTecla4=0;
boolean EstadoTecla5=0;
boolean EstadoTecla6=0;
boolean EstadoTecla7=0;
boolean EstadoTecla8=0;
boolean EstadoTecla9=0;
boolean EstadoTecla10=0;
boolean EstadoTecla11=0;
boolean EstadoTecla12=0;
boolean EstadoTecla13=0;
boolean EstadoTecla14=0;
boolean EstadoTecla15=0;
boolean EstadoTecla16=0;
boolean EstadoTecla17=0;
boolean EstadoTecla18=0;
boolean EstadoTecla19=0;
boolean EstadoTecla20=0;


unsigned long tiempoTecla1=0;
unsigned long tiempoTecla2=0;
unsigned long tiempoTecla3=0;
unsigned long tiempoTecla4=0;
unsigned long tiempoTecla5=0;
unsigned long tiempoTecla6=0;
unsigned long tiempoTecla7=0;
unsigned long tiempoTecla8=0;
unsigned long tiempoTecla9=0;
unsigned long tiempoTecla10=0;
unsigned long tiempoTecla11=0;
unsigned long tiempoTecla12=0;
unsigned long tiempoTecla13=0;
unsigned long tiempoTecla14=0;
unsigned long tiempoTecla15=0;
unsigned long tiempoTecla16=0;
unsigned long tiempoTecla17=0;
unsigned long tiempoTecla18=0;
unsigned long tiempoTecla19=0;
unsigned long tiempoTecla20=0;

void setup ()
{
  pinMode (led1, OUTPUT) ; // Definimos el pin led ojo derecho
  pinMode (led2, OUTPUT) ;  //Definimos el pin led ojo izquierdo
  pinMode (led3, OUTPUT) ;  //Definimos el pin led boca
  pinMode (tecla1, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla2, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla3, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla4, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla5, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla6, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla7, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla8, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla9, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla10, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla11, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla12, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla13, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla14, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla15, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla16, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla17, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla18, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla19, INPUT) ; // Definimos el pin de detección de la pulsación
  pinMode (tecla20, INPUT) ; // Definimos el pin de detección de la pulsación
  
  
  #ifdef DEBUG_ORGAN
       Serial.begin(9600);
       Serial.println(F("Inicia Arduino"));
  #endif
  
  //MQTT
  client.setServer(server, 1883);
  client.setCallback(callback);


  //Ethernet.begin(mac);   
  Ethernet.begin(mac,ip);
  //delay(1000);

  #ifdef DEBUG_ORGAN
    Serial.println(F("connectant..."));
    Serial.println(Ethernet.localIP());
    Serial.println(F("Definició dels PINS de les tecles"));
  #endif
}


void acordInicial(){
  client.publish("sala2/tecla1","0");
  delay(3);
  client.publish("sala2/tecla5","0");
  delay(3);
  client.publish("sala2/tecla9","0");
  delay(2000);
  client.publish("sala2/tecla1","1");
  delay(3);
  client.publish("sala2/tecla5","1");
  delay(3);
  client.publish("sala2/tecla9","1");
  delay(1000);
  client.publish("sala2/tecla14","0");
  delay(3);
  client.publish("sala2/tecla17","0");
  delay(3);
  client.publish("sala2/tecla20","0");
  delay(2500);
  client.publish("sala2/tecla14","1");
  delay(3);
  client.publish("sala2/tecla17","1");
  delay(3);
  client.publish("sala2/tecla20","1");
}
   
void encenLedsInicial(){
  digitalWrite(led1,HIGH);
  digitalWrite(led2,HIGH);
  digitalWrite(led3,HIGH);
  delay(1000);
  digitalWrite(led1,LOW);
  digitalWrite(led2,LOW);
  digitalWrite(led3,LOW);
  
}

void encenUllDret(){
     digitalWrite(led1,HIGH);  
}

void apagaUllDret(){
    digitalWrite(led1,LOW);
}

void encenUllEsquerra(){
   digitalWrite(led2,HIGH);
}

void apagaUllEsquerra(){
  digitalWrite(led2,LOW);
}

void encenBoca(){
  digitalWrite(led3,HIGH);  
}

void apagaBoca(){
  digitalWrite(led3,LOW);
}

void callback(char* topic, byte* payload, unsigned int length) {
  char msg[length];
  
  #ifdef DEBUG_ORGAN
    Serial.print(F("Message arrived ["));
    Serial.print(topic);    
    Serial.print(F("] "));
    
    for (int i=0;i<length;i++) {
      msg[i]=(char)payload[i];
      Serial.print((char)payload[i]);
    }  
    Serial.println();
  #endif
 
  int res=strcmp(topic,"sala2/reset");
  int resu = strcmp(topic,"sala2/resetOrgano");  
  if (res == 0 || resu == 0) { //RESET PLACA
      #ifdef DEBUG_ORGAN
        Serial.print(F("ENTRA a reset o resetOrgano"));
      #endif
     wdt_enable(WDTO_15MS); // Configuramos el contador de tiempo para que se reinicie en 15ms
  }

  res=strcmp(topic,"sala2/encenUllDret");
  if (res == 0 ){
     #ifdef DEBUG_ORGAN
        Serial.print(F("ENTRA a encenUlldret"));
     #endif

     encenUllDret();
  }

  res=strcmp(topic,"sala2/apagaUllDret");
  if (res == 0 ){
     #ifdef DEBUG_ORGAN
        Serial.print(F("ENTRA a apagaUlldret"));
     #endif

     apagaUllDret();
  }

  res=strcmp(topic,"sala2/encenUllEsquerra");
  if (res == 0 ){
     #ifdef DEBUG_ORGAN
        Serial.print(F("ENTRA a encenUllesquerra"));
     #endif

     encenUllEsquerra();
  }

  res=strcmp(topic,"sala2/apagaUllEsquerra");
  if (res == 0 ){
     #ifdef DEBUG_ORGAN
        Serial.print(F("ENTRA a apagaUllesquerra"));           
     #endif

     apagaUllEsquerra();
  }

  res=strcmp(topic,"sala2/encenBoca");
  if (res == 0){
     #ifdef DEBUG_ORGAN
        Serial.print(F("ENTRA a encenUllesquerra"));           
     #endif

     encenBoca();
  }

  res=strcmp(topic,"sala2/apagaBoca");
  if (res == 0){
     #ifdef DEBUG_ORGAN
        Serial.print(F("ENTRA a apagaBoca"));           
     #endif

     apagaBoca();
  }

  

  res=strcmp(topic,"sala2/activaOrgan");
  if (res == 0) {
     
     #ifdef DEBUG_ORGAN
        Serial.print(F("ENTRA a activaOrgan"));           
     #endif

    encenLedsInicial();
    acordInicial();
    //Activem el teclat de l'organ
    actiu=true; 
  }
    
  res=strcmp(topic,"sala2/desactivaOrgan");
  if (res == 0) {
     actiu=false;
     #ifdef DEBUG_ORGAN
        Serial.print(F("ENTRA a DESactivaOrgan"));        
     #endif
  }

  res=strcmp(topic,"sala2/iniciaPunxos");
  if (res == 0) {     
     #ifdef DEBUG_ORGAN
        Serial.print(F("ENTRA a iniciaPunxos"));                
     #endif
     estat=1;
  }

  res=strcmp(topic,"sala2/estat2Punxos");
  if (res == 0) {     
     #ifdef DEBUG_ORGAN
        Serial.print(F("ENTRA a estat2Punxos"));                
     #endif
     estat=2;
     digitalWrite(led1,HIGH);
     digitalWrite(led2,LOW);
     digitalWrite(led3,LOW);
  }

  res=strcmp(topic,"sala2/estat3Punxos");
  if (res == 0) {     
     #ifdef DEBUG_ORGAN
        Serial.print(F("ENTRA a estat3Punxos"));                
     #endif
     estat=3;
     digitalWrite(led1,LOW);
     digitalWrite(led2,HIGH);
     digitalWrite(led3,LOW);
  }

  res=strcmp(topic,"sala2/estat4Punxos");
  if (res == 0) {     
     #ifdef DEBUG_ORGAN
        Serial.print(F("ENTRA a estat4Punxos"));                
     #endif
     estat=4;
     digitalWrite(led1,LOW);
     digitalWrite(led2,LOW);
     digitalWrite(led3,HIGH);
  }

  res=strcmp(topic,"sala2/finalPunxos");
  if (res == 0) {     
     #ifdef DEBUG_ORGAN
        Serial.print(F("ENTRA a finalPunxos"));                
     #endif
     estat=5;
     digitalWrite(led1,HIGH);
     digitalWrite(led2,HIGH);
     digitalWrite(led3,LOW);
  }
}

void reconnect() {

  // Loop until we're reconnected
  while (!client.connected()) {
    #ifdef DEBUG_ORGAN
      Serial.print(F("Intentant connecció MQTT..."));
    #endif
    
    // Attempt to connect
    if (client.connect("Organo")) {
      #ifdef DEBUG_ORGAN
        Serial.println(F("connectat MQTT"));      
      #endif

      client.subscribe("sala2/reset"); 
      client.subscribe("sala2/dificultat");
      client.subscribe("sala2/resetOrgano");
      client.subscribe("sala2/activaOrgan"); 
      client.subscribe("sala2/desactivaOrgan"); 
      
      client.subscribe("sala2/iniciaPunxos");
      client.subscribe("sala2/estat2Punxos");
      client.subscribe("sala2/estat3Punxos");
      client.subscribe("sala2/estat4Punxos");
      client.subscribe("sala2/estat5Punxos");      

      client.subscribe("sala2/encenUllDret");
      client.subscribe("sala2/apagaUllDret");
      client.subscribe("sala2/encenUllEsquerra");
      client.subscribe("sala2/apagaUllEsquerra");
      client.subscribe("sala2/encenBoca");      
      client.subscribe("sala2/apagaBoca");      
      
    } else {
      #ifdef DEBUG_ORGAN
        Serial.print(F("FALLA, rc="));
        Serial.print(client.state());
      #endif
      //Serial.println(" try again in 5 seconds");
      // Wait 1 second before retrying
      delay(1000);
    }
  }

}



void loop ()
{

  if (actiu) {
    
    AlarmaTecla1= digitalRead (tecla1) ; // Leemos el estado del pin asociado a la pulsación  
    AlarmaTecla2= digitalRead (tecla2) ; // Leemos el estado del pin asociado a la pulsación
    AlarmaTecla3= digitalRead (tecla3) ; // Leemos el estado del pin asociado a la pulsación
    AlarmaTecla4= digitalRead (tecla4) ; // Leemos el estado del pin asociado a la pulsación
    AlarmaTecla5= digitalRead (tecla5) ; // Leemos el estado del pin asociado a la pulsación  
    AlarmaTecla6= digitalRead (tecla6) ; // Leemos el estado del pin asociado a la pulsación
    AlarmaTecla7= digitalRead (tecla7) ; // Leemos el estado del pin asociado a la pulsación
    AlarmaTecla8= digitalRead (tecla8) ; // Leemos el estado del pin asociado a la pulsación
    AlarmaTecla9= digitalRead (tecla9) ; // Leemos el estado del pin asociado a la pulsación  
    AlarmaTecla10= digitalRead (tecla10) ; // Leemos el estado del pin asociado a la pulsación
    AlarmaTecla11= digitalRead (tecla11) ; // Leemos el estado del pin asociado a la pulsación
    AlarmaTecla12= digitalRead (tecla12) ; // Leemos el estado del pin asociado a la pulsación
    AlarmaTecla13= digitalRead (tecla13) ; // Leemos el estado del pin asociado a la pulsación  
    AlarmaTecla14= digitalRead (tecla14) ; // Leemos el estado del pin asociado a la pulsación
    AlarmaTecla15= digitalRead (tecla15) ; // Leemos el estado del pin asociado a la pulsación
    AlarmaTecla16= digitalRead (tecla16) ; // Leemos el estado del pin asociado a la pulsación
    AlarmaTecla17= digitalRead (tecla17) ; // Leemos el estado del pin asociado a la pulsación  
    AlarmaTecla18= digitalRead (tecla18) ; // Leemos el estado del pin asociado a la pulsación
    AlarmaTecla19= digitalRead (tecla19) ; // Leemos el estado del pin asociado a la pulsación
    AlarmaTecla20= digitalRead (tecla20) ; // Leemos el estado del pin asociado a la pulsación
    
    if (AlarmaTecla1) {    
      if (!EstadoTecla1 && (millis()-tiempoTecla1) > 300 ) {
        client.publish("sala2/tecla1","1");        
        Serial.println("tecla1 cambio de estado a 1");      
        EstadoTecla1=1;
      }
    } else {
      if (EstadoTecla1) {
        client.publish("sala2/tecla1","0");        
        Serial.println("tecla1 cambio de estado a 0");
        EstadoTecla1=0;   
        tiempoTecla1=millis();
      }
    }
  
   if (AlarmaTecla2) {    
      if (!EstadoTecla2 && (millis()-tiempoTecla2) > 300) {
        client.publish("sala2/tecla2","1");        
        Serial.println("tecla2 cambio de estado a 1");      
        EstadoTecla2=1;
      }
    } else {
      if (EstadoTecla2) {
        client.publish("sala2/tecla2","0");        
        Serial.println("tecla2 cambio de estado a 0");
        EstadoTecla2=0;   
        tiempoTecla2=millis();
      }
    }
  
   if (AlarmaTecla3 && (millis()-tiempoTecla3) > 300) {    
      if (!EstadoTecla3) {
        client.publish("sala2/tecla3","1");        
        Serial.println("tecla3 cambio de estado a 1");      
        EstadoTecla3=1;
      }
    } else {
      if (EstadoTecla3) {
        client.publish("sala2/tecla3","0");        
        Serial.println("tecla3 cambio de estado a 0");
        EstadoTecla3=0;   
        tiempoTecla3=millis();
      }
    }
  
    if (AlarmaTecla4 && (millis()-tiempoTecla4) > 300) {    
      if (!EstadoTecla4) {
        client.publish("sala2/tecla4","1");        
        Serial.println("tecla4 cambio de estado a 1");      
        EstadoTecla4=1;
      }
    } else {
      if (EstadoTecla4) {
        client.publish("sala2/tecla4","0");        
        Serial.println("tecla4 cambio de estado a 0");
        EstadoTecla4=0;   
        tiempoTecla4=millis();
      }
    }
  
    if (AlarmaTecla5) {    
      if (!EstadoTecla5 && (millis()-tiempoTecla5) > 300 ) {
        client.publish("sala2/tecla5","1");        
        Serial.println("tecla5 cambio de estado a 1");      
        EstadoTecla5=1;
      }
    } else {
      if (EstadoTecla5) {
        client.publish("sala2/tecla5","0");        
        Serial.println("tecla5 cambio de estado a 0");
        EstadoTecla5=0;   
        tiempoTecla5=millis();
      }
    }
  
   if (AlarmaTecla6) {    
      if (!EstadoTecla6 && (millis()-tiempoTecla6) > 300) {
        client.publish("sala2/tecla6","1");        
        Serial.println("tecla6 cambio de estado a 1");      
        EstadoTecla6=1;
      }
    } else {
      if (EstadoTecla6) {
        client.publish("sala2/tecla6","0");        
        Serial.println("tecla6 cambio de estado a 0");
        EstadoTecla6=0;   
        tiempoTecla6=millis();
      }
    }
  
   if (AlarmaTecla7 && (millis()-tiempoTecla7) > 300) {    
      if (!EstadoTecla7) {
        client.publish("sala2/tecla7","1");        
        Serial.println("tecla7 cambio de estado a 1");      
        EstadoTecla7=1;
      }
    } else {
      if (EstadoTecla7) {
        client.publish("sala2/tecla7","0");        
        Serial.println("tecla7 cambio de estado a 0");
        EstadoTecla7=0;   
        tiempoTecla7=millis();
      }
    }
  
    if (AlarmaTecla8 && (millis()-tiempoTecla8) > 300) {    
      if (!EstadoTecla8) {
        client.publish("sala2/tecla8","1");        
        Serial.println("tecla8 cambio de estado a 1");      
        EstadoTecla8=1;
      }
    } else {
      if (EstadoTecla8) {
        client.publish("sala2/tecla8","0");        
        Serial.println("tecla8 cambio de estado a 0");
        EstadoTecla8=0;   
        tiempoTecla8=millis();
      }
    }
  
    if (AlarmaTecla9) {    
      if (!EstadoTecla9 && (millis()-tiempoTecla9) > 300 ) {
        client.publish("sala2/tecla9","1");        
        Serial.println("tecla9 cambio de estado a 1");      
        EstadoTecla9=1;
      }
    } else {
      if (EstadoTecla9) {
        client.publish("sala2/tecla9","0");        
        Serial.println("tecla9 cambio de estado a 0");
        EstadoTecla9=0;   
        tiempoTecla9=millis();
      }
    }
  
   if (AlarmaTecla10) {    
      if (!EstadoTecla10 && (millis()-tiempoTecla10) > 300) {
        client.publish("sala2/tecla10","1");        
        Serial.println("tecla10 cambio de estado a 1");      
        EstadoTecla10=1;
      }
    } else {
      if (EstadoTecla10) {
        client.publish("sala2/tecla10","0");        
        Serial.println("tecla10 cambio de estado a 0");
        EstadoTecla10=0;   
        tiempoTecla10=millis();
      }
    }
  
   if (AlarmaTecla11 && (millis()-tiempoTecla11) > 300) {    
      if (!EstadoTecla11) {
        client.publish("sala2/tecla11","1");        
        Serial.println("tecla11 cambio de estado a 1");      
        EstadoTecla11=1;
      }
    } else {
      if (EstadoTecla11) {
        client.publish("sala2/tecla11","0");        
        Serial.println("tecla11 cambio de estado a 0");
        EstadoTecla11=0;   
        tiempoTecla11=millis();
      }
    }
  
    if (AlarmaTecla12 && (millis()-tiempoTecla12) > 300) {    
      if (!EstadoTecla12) {
        client.publish("sala2/tecla12","1");        
        Serial.println("tecla12 cambio de estado a 1");      
        EstadoTecla12=1;
      }
    } else {
      if (EstadoTecla12) {
        client.publish("sala2/tecla12","0");        
        Serial.println("tecla12 cambio de estado a 0");
        EstadoTecla12=0;   
        tiempoTecla12=millis();
      }
    }
  
    if (AlarmaTecla13) {    
      if (!EstadoTecla13 && (millis()-tiempoTecla13) > 300 ) {
        client.publish("sala2/tecla13","1");        
        Serial.println("tecla13 cambio de estado a 1");      
        EstadoTecla13=1;
      }
    } else {
      if (EstadoTecla13) {
        client.publish("sala2/tecla13","0");        
        Serial.println("tecla13 cambio de estado a 0");
        EstadoTecla13=0;   
        tiempoTecla13=millis();
      }
    }
  
   if (AlarmaTecla14) {    
      if (!EstadoTecla14 && (millis()-tiempoTecla14) > 300) {
        client.publish("sala2/tecla14","1");        
        Serial.println("tecla14 cambio de estado a 1");      
        EstadoTecla14=1;
      }
    } else {
      if (EstadoTecla14) {
        client.publish("sala2/tecla14","0");        
        Serial.println("tecla14 cambio de estado a 0");
        EstadoTecla14=0;   
        tiempoTecla14=millis();
      }
    }
  
   if (AlarmaTecla15 && (millis()-tiempoTecla15) > 300) {    
      if (!EstadoTecla15) {
        client.publish("sala2/tecla15","1");        
        Serial.println("tecla15 cambio de estado a 1");      
        EstadoTecla15=1;
      }
    } else {
      if (EstadoTecla15) {
        client.publish("sala2/tecla15","0");        
        Serial.println("tecla15 cambio de estado a 0");
        EstadoTecla15=0;   
        tiempoTecla15=millis();
      }
    }
  
    if (AlarmaTecla16 && (millis()-tiempoTecla16) > 300) {    
      if (!EstadoTecla16) {
        client.publish("sala2/tecla16","1");        
        Serial.println("tecla16 cambio de estado a 1");      
        EstadoTecla16=1;
      }
    } else {
      if (EstadoTecla16) {
        client.publish("sala2/tecla16","0");        
        Serial.println("tecla16 cambio de estado a 0");
        EstadoTecla16=0;   
        tiempoTecla16=millis();
      }
    }
  
    if (AlarmaTecla17) {    
      if (!EstadoTecla17 && (millis()-tiempoTecla17) > 300 ) {
        client.publish("sala2/tecla17","1");        
        Serial.println("tecla17 cambio de estado a 1");      
        EstadoTecla17=1;
      }
    } else {
      if (EstadoTecla17) {
        client.publish("sala2/tecla17","0");        
        Serial.println("tecla17 cambio de estado a 0");
        EstadoTecla17=0;   
        tiempoTecla17=millis();
      }
    }
  
   if (AlarmaTecla18) {    
      if (!EstadoTecla18 && (millis()-tiempoTecla18) > 300) {
        client.publish("sala2/tecla18","1");      
        Serial.println("tecla18 cambio de estado a 1");      
        EstadoTecla18=1;
      }
    } else {
      if (EstadoTecla18) {
        client.publish("sala2/tecla18","0");        
        Serial.println("tecla18 cambio de estado a 0");
        EstadoTecla18=0;   
        tiempoTecla18=millis();
      }
    }
  
   if (AlarmaTecla19 && (millis()-tiempoTecla19) > 300) {    
      if (!EstadoTecla19) {
        client.publish("sala2/tecla19","1");        
        Serial.println("tecla19 cambio de estado a 1");      
        EstadoTecla19=1;
      }
    } else {
      if (EstadoTecla19) {
        client.publish("sala2/tecla19","0");        
        Serial.println("tecla19 cambio de estado a 0");
        EstadoTecla19=0;   
        tiempoTecla19=millis();
      }
    }
  
    if (AlarmaTecla20 && (millis()-tiempoTecla20) > 300) {    
      if (!EstadoTecla20) {
        client.publish("sala2/tecla20","1");      
        Serial.println("tecla20 cambio de estado a 1");      
        EstadoTecla20=1;
      }
    } else {
      if (EstadoTecla20) {
        client.publish("sala2/tecla20","0");        
        Serial.println("tecla20 cambio de estado a 0");
        EstadoTecla20=0;   
        tiempoTecla20=millis();
      }
    }

    switch(estat){
      case 2:
          if (EstadoTecla1 && EstadoTecla2 && EstadoTecla3){
            client.publish("sala2/acordCorrecte","on");
          }
      break;
      case 3:
          if (EstadoTecla4 && EstadoTecla5 && EstadoTecla6){
            client.publish("sala2/acordCorrecte","on");
          }
      break;
      case 4:
           if (EstadoTecla7 && EstadoTecla8 && EstadoTecla9){
            client.publish("sala2/acordCorrecte","on");
          }
      break;
      case 5:
           if (EstadoTecla10 && EstadoTecla11 && EstadoTecla12){
            client.publish("sala2/acordCorrecte","on");
          }
      break;
      /*case 5:
           if (EstadoTecla13 && EstadoTecla14 && EstadoTecla15){
            client.publish("sala2/acordCorrecte","on");
          }
      break;
      */
    }
    
  } //IF si esta actiu 
  
  if (!client.connected()) {
      reconnect();
  }
  client.loop();
  
} 
