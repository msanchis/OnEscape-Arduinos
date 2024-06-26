#include <SPI.h>
#include <Ethernet.h> //Conexion Ethernet con carcasa W5100
#include <PubSubClient.h> //Conexion MQTT

//VARIABLES i objectes de la xarxa i client Mosquitto
byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xFF, 0x08};//mac del arduino
IPAddress ip(192, 168, 68, 27); //Ip fija del arduino
IPAddress server(192, 168, 68, 1); //Ip del server de mosquitto

EthernetClient ethClient; //Interfaz de red ethernet
PubSubClient client(ethClient); //cliente MQTT

const boolean DEBUG = true;
unsigned long debugTemps=0;
//Variable IMPORTANT que permet la màquina d'estats del pedestal
int estat=0; 

//Variables per a controlar el temps de cada activació de sensor
boolean entra=false;
boolean entra1=false;
boolean entra2=false;
boolean entra3=false;
boolean entra4=false;
boolean entra5=false;
boolean entra6=false;



const int SensorPuzzle = A0; //Sensor puzzle paret
unsigned long marcaTemps0 = 0; //Marca de temps per al puzzle paret

const int Sensor1 = 22;    // Sensor1 presencia iman1
const int Sensor2 = 23;     // Sensor1 presencia iman2
unsigned long marcaTemps1 = 0; //Marca de temps per als sensors 1 i 2

const int Sensor3 = 26;    // Sensor2 presencia iman1
const int Sensor4 = 27;    // Sensor2 presencia iman2
unsigned long marcaTemps2 = 0; //Marca de temps per als sensors 3 i 4

const int Sensor5 = 30;     // Sensor3 presencia iman1
const int Sensor6 = 31;    // Sensor3 presencia iman2
unsigned long marcaTemps3 = 0; //Marca de temps per als sensors 5 i 6

const int Sensor7 = 34;    // Sensor4 presencia iman1
const int Sensor8 = 35;     // Sensor4 presencia iman2
unsigned long marcaTemps4 = 0; //Marca de temps per als sensors 7 i 8

const int Sensor9 = 38;    // Sensor5 presencia iman1
const int Sensor10 = 39;    // Sensor5 presencia iman2
unsigned long marcaTemps5 = 0; //Marca de temps per als sensors 9 i 10

const int Sensor11 = 42;     // Sensor6 presencia iman1
const int Sensor12 = 43;    // Sensor6 presencia iman2
unsigned long marcaTemps6 = 0; //Marca de temps per als sensors 11 i 12

const int RelElectro1 = 2; // Pin para cortar corriente Electroiman1 
const int RelElectro2 = 3; // Pin para cortar corriente Electroiman2 y encender luz4
const int RelElectro3 = 4; // Pin para cortar corriente Electroiman3 y encender luz5
const int RelElectro4 = 5; // Pin para cortar corriente Electroiman4 y encender luz1
const int RelElectro5 = 6; // Pin para cortar corriente Electroiman5 y encender luz2
const int RelElectro6 = 7; // Pin para cortar corriente Electroiman6 y encender luz3
const int RelElectro7 = 8; // Pin para cortar corriente Electroiman7 y encender luz7
const int RelElectro8 = 9; // Pin encender luz6


void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  if (DEBUG) {
    Serial.println(F("Inicia Arduino"));
  }

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

  pinMode(SensorPuzzle, INPUT_PULLUP);
  
  pinMode(Sensor1, INPUT_PULLUP);
  pinMode(Sensor2, INPUT_PULLUP);
  pinMode(Sensor3, INPUT_PULLUP);
  pinMode(Sensor4, INPUT_PULLUP);
  pinMode(Sensor5, INPUT_PULLUP);
  pinMode(Sensor6, INPUT_PULLUP);
  pinMode(Sensor7, INPUT_PULLUP);
  pinMode(Sensor8, INPUT_PULLUP);
  pinMode(Sensor9, INPUT_PULLUP);
  pinMode(Sensor10, INPUT_PULLUP);
  pinMode(Sensor11, INPUT_PULLUP);
  pinMode(Sensor12, INPUT_PULLUP);

  pinMode(RelElectro1, OUTPUT);
  pinMode(RelElectro2, OUTPUT);
  pinMode(RelElectro3, OUTPUT);
  pinMode(RelElectro4, OUTPUT);
  pinMode(RelElectro5, OUTPUT);
  pinMode(RelElectro6, OUTPUT);
  pinMode(RelElectro7, OUTPUT);
  pinMode(RelElectro8, OUTPUT);


  digitalWrite(RelElectro1,LOW);
  digitalWrite(RelElectro2,LOW);
  digitalWrite(RelElectro3,LOW);
  digitalWrite(RelElectro4,LOW);
  digitalWrite(RelElectro5,LOW);
  digitalWrite(RelElectro6,LOW);
  digitalWrite(RelElectro7,LOW);
  digitalWrite(RelElectro8,LOW);
  
 debugTemps = millis();
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
}

 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    if (DEBUG) Serial.print(F("Attempting MQTT connection..."));
    // Attempt to connect
    if (client.connect("Pedestal")) {
      //Serial.println(F("connected"));      

      if (DEBUG) Serial.print(F("Subscribe to abrirfinal cerrarfinal reinici reset resetPortaFinal"));
      client.subscribe("sala2/reset"); 
      client.subscribe("sala1/resetPedestal");  
    
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

  int value  = digitalRead(SensorPuzzle);
  int value1 = digitalRead(Sensor1);
  int value2 = digitalRead(Sensor2);
  int value3 = digitalRead(Sensor3);
  int value4 = digitalRead(Sensor4);
  int value5 = digitalRead(Sensor5);
  int value6 = digitalRead(Sensor6);
  int value7 = digitalRead(Sensor7);
  int value8 = digitalRead(Sensor8);
  int value9 = digitalRead(Sensor9);
  int value10 = digitalRead(Sensor10);
  int value11 = digitalRead(Sensor11);
  int value12 = digitalRead(Sensor12);


  if (DEBUG && (millis() > (debugTemps + 500))) {
    debugTemps=millis();
    Serial.print("ESTAT ");
    Serial.println(estat);
    Serial.print(value);
    Serial.print(" ");
    Serial.print(value1);
    Serial.print(" ");
    Serial.print(value2);
    Serial.print(" ");
    Serial.print(value3);
    Serial.print(" ");
    Serial.print(value4);
    Serial.print(" ");
    Serial.print(value5);
    Serial.print(" ");
    Serial.print(value6);
    Serial.print(" ");
    Serial.print(value7);
    Serial.print(" ");
    Serial.print(value8);
    Serial.print(" ");
    Serial.print(value9);
    Serial.print(" ");
    Serial.print(value10);
    Serial.print(" ");
    Serial.print(value11);
    Serial.print(" ");
    Serial.print(value12);
    Serial.println(" ");
    
  }

 switch(estat){    

    case 0: //Esperant puzzle1
      //if (DEBUG) Serial.println("ESTAT 0");
      if (value == 0 && !entra ){ //Comprovem si han superat el puzzle de la paret
        marcaTemps0=millis();
        entra=true;
      }
      if (value == 1 ){
        marcaTemps0=0;
        entra=false;
      }
      if (marcaTemps0 > 0 && (millis()-marcaTemps0 > 300)){
        digitalWrite(RelElectro1,HIGH); // Desactivem el electroiman1 per a que obriga comporta
        estat=1;        
      }      
      break;
      
    case 1: //Esperant relíquia1
      //if (DEBUG) Serial.println("ESTAT 1");
      if ((value1 == 0 || value2 == 0) && !entra1){
        marcaTemps1=millis();
        entra1=true;
      }
      if (value1 == 1 && value2 == 1){
        marcaTemps1=0;
        entra1=false;
      }
      if (marcaTemps1 > 0 && (millis()-marcaTemps1 > 300)){
        digitalWrite(RelElectro2,HIGH); //Desactivem electroiman2 i encenem llum 1
        estat=2;
      }      
      break;
      
    case 2: //Esperant relíquia4
      if (DEBUG) Serial.println("ESTAT 2");
      if ((value7 == LOW || value8 == LOW) && !entra4){
        marcaTemps4=millis();
        entra4=true;
      }
      if (value7 == HIGH && value8 == HIGH){
        marcaTemps4=0;
        entra4=false;
      }
      if (marcaTemps4 > 0 && (millis()-marcaTemps4 > 300)){
        digitalWrite(RelElectro2,HIGH); //Desactivem electroiman2 i encenem llum 4
        estat=3;
      }
      
      break;
   case 3: //Esperant relíquia2
      if (DEBUG) Serial.println("ESTAT 3");
      if ((value3 == LOW || value4 == LOW) && !entra2){
        marcaTemps2=millis();
        entra2=true;
      }
      if (value3 == HIGH && value4 == HIGH){
        marcaTemps2=0;
        entra2=false;
      }
      if (marcaTemps2 > 0 && (millis()-marcaTemps2 > 300)){
        digitalWrite(RelElectro5,HIGH); //Desactivem electroiman5 i encenem llum 2
        estat=4;
      }
      
      break;
   case 4: //Esperant relíquia5
      if (DEBUG) Serial.println("ESTAT 4");
      if ((value9 == LOW || value10 == LOW) && !entra5){
        marcaTemps5=millis();
        entra5=true;
      }
      if (value9 == HIGH && value10 == HIGH){
        marcaTemps5=0;
        entra5=false;
      }
      if (marcaTemps5 > 0 && (millis()-marcaTemps5 > 300)){
        digitalWrite(RelElectro3,HIGH); //Desactivem electroiman3 i encenem llum 5
        estat=5;
      }
      
      break;
   case 5: //Esperant relíquia3
      if (DEBUG) Serial.println("ESTAT 5");
      if ((value5 == LOW || value6 == LOW) && !entra3){
        marcaTemps3=millis();
        entra3=true;
      }
      if (value5 == HIGH && value6 == HIGH){
        marcaTemps3=0;
        entra3=false;
      }
      if (marcaTemps3 > 0 && (millis()-marcaTemps3 > 300)){
        digitalWrite(RelElectro3,HIGH); //Desactivem electroiman6 i encenem llum 3
        estat=6;
      }
      
      break;
   case 6: //Esperant relíquia6
      if (DEBUG) Serial.println("ESTAT 6");
      if ((value11 == LOW || value12 == LOW) && !entra6){
        marcaTemps6=millis();
        entra6=true;
      }
      if (value11 == HIGH && value12 == HIGH){
        marcaTemps6=0;
        entra6=false;
      }
      if (marcaTemps6 > 0 && (millis()-marcaTemps6 > 300)){
        digitalWrite(RelElectro8,HIGH); //Encenem llum 6
        estat=7;
      }
      
      break;
   case 7:
      if (DEBUG) Serial.println("ESTAT 7");
      break;

  }
  /*
  if (!client.connected()) {
    reconnect();
  }  
  
  client.loop();
  */
  
 
}
