#include <SPI.h>
#include <Ethernet.h> //Conexion Ethernet con carcasa W5100
#include <PubSubClient.h> //Conexion MQTT

//VARIABLES i objectes de la xarxa i client Mosquitto
byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xFF, 0x08};//mac del arduino
IPAddress ip(192, 168, 68, 27); //Ip fija del arduino
IPAddress server(192, 168, 68, 1); //Ip del server de mosquitto

EthernetClient ethClient; //Interfaz de red ethernet
PubSubClient client(ethClient); //cliente MQTT

const int Sensor11 = 22;    // Sensor1 presencia iman1
const int Sensor12 = 23;     // Sensor1 presencia iman2

const int Sensor21 = 24;    // Sensor2 presencia iman1
const int Sensor22 = 25;    // Sensor2 presencia iman2

const int Sensor31 = 26;     // Sensor3 presencia iman1
const int Sensor32 = 27;    // Sensor3 presencia iman2


const int Sensor41 = 28;    // Sensor4 presencia iman1
const int Sensor42 = 29;     // Sensor4 presencia iman2

const int Sensor51 = 30;    // Sensor5 presencia iman1
const int Sensor52 = 31;    // Sensor5 presencia iman2

const int Sensor61 = 32;     // Sensor6 presencia iman1
const int Sensor62 = 33;    // Sensor6 presencia iman2

const int RelElectro1 = 36; // Pin para cortar corriente Electroiman1
const int RelElectro2 = 37; // Pin para cortar corriente Electroiman2
const int RelElectro3 = 38; // Pin para cortar corriente Electroiman3
const int RelElectro4 = 39; // Pin para cortar corriente Electroiman4
const int RelElectro5 = 40; // Pin para cortar corriente Electroiman5
const int RelElectro6 = 41; // Pin para cortar corriente Electroiman6
const int RelElectro7 = 42; // Pin para cortar corriente Electroiman7

const int ReleLuz1 = 43; // Pin para encender Luz1
const int ReleLuz2 = 44; // Pin para encender Luz2
const int ReleLuz3 = 45; // Pin para encender Luz3
const int ReleLuz4 = 46; // Pin para encender Luz4
const int ReleLuz5 = 47; // Pin para encender Luz5
const int ReleLuz6 = 48; // Pin para encender Luz6
const int ReleLuz7 = 49; // Pin para encender Luz7

const int ReleHumo = 50; //Pin para encender humo makina

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  if (DEBUG) {
    Serial.println(F("Inicia Arduino"));
  }

  //MQTT
  client.setServer(server, 1883);
  client.setCallback(callback);

  //Ethernet.begin(mac);   
  Ethernet.begin(mac,ip);
      
  delay(100); //Cambio de 1000 a 100
  if (DEBUG) {
    Serial.println(F("connectant..."));
    Serial.println(Ethernet.localIP());
    Serial.println(F("Definició dels PINS dels LEDS"));
  }

  pinMode(RelElectro1, OUTPUT);
  pinMode(RelElectro2, OUTPUT);
  pinMode(RelElectro3, OUTPUT);
  pinMode(RelElectro4, OUTPUT);
  pinMode(RelElectro5, OUTPUT);
  pinMode(RelElectro6, OUTPUT);
  pinMode(RelElectro7, OUTPUT);

  pinMode(ReleLuz1, OUTPUT);
  pinMode(ReleLuz2, OUTPUT);
  pinMode(ReleLuz3, OUTPUT);
  pinMode(ReleLuz4, OUTPUT);
  pinMode(ReleLuz5, OUTPUT);
  pinMode(ReleLuz6, OUTPUT);
  pinMode(ReleLuz7, OUTPUT);
  
  pinMode(ReleHumo, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:

}
