//WeMos D1
//RFID-RC522
//4 Relay Module

/*
 * 3.3V  ---- RFID-RC522
 * 5V -------- 4 Relay Module VCC
 * 2 x GND ----- 2 boards
 * SS/D8 – GPIO15 ------ RFID SDA pin1
 * MOSI/D7- GPIO13 -----RFID MOSI  pin3
 * MISO/D6 – GPIO12 -----RFID MISO pin4
 * SCK/D5 ---GPIO14 ---- RFID SCK  pin2
 * D0 --- GPIO16 ---------- RFID RST pin 7
 * --------------------RFID pin5 VACIO
 */


#include <ESP8266WiFi.h>   //Conexion wifi                      
#include <PubSubClient.h> //Conexion Mosquitto 

#include <SPI.h>
#include <MFRC522.h>

//#define RST_PIN         16          // Configurable, see typical pin layout above   D0
//#define SS_PIN          15         // Configurable, see typical pin layout above    D8

#define RST_PIN         5
#define SS_PIN          4
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

//Variables per a la conexió Wifi & Mosquitto
/*
const char* ssid = "OntitelFTTH_5493";
const char* password = "mm45WQ9u";r
const char* mqtt_server = "192.168.1.6";
*/
const char* ssid = "OnEscape";
const char* password = "M1cky&S1ny0";
const char* mqtt_server = "192.168.69.1";

WiFiClient espClient;
PubSubClient client(espClient);

bool DEBUG = true; //Variable per veure el que pasa

byte validKey1[4] = { 0x86, 0x75, 0x27, 0x1F };  // Clave valida
byte resetKey[4] =  { 0x97, 0x92, 0x55, 0xD3 };  // Clave reset

int cont=0;

const int RELE_BLANC = 0;
const int RELE_VERD = 2;
const int RELE_ROIG = 4;
const int RELE_PORTA = 5;

unsigned long obrir=0;
unsigned long obrirPorta=0;
boolean entra=false;
boolean sale=false;

long tempsFinal = 6000;
long tempsObrirPorta = 44000;

bool enviaRFID = false;

//Función para imprimir el contenido de un vector
void printArray(byte *buffer, byte bufferSize) {
   for (byte i = 0; i < bufferSize; i++) {
      Serial.print(buffer[i] < 0x10 ? " 0" : " ");
      Serial.print(buffer[i], HEX);
   }
}
 
//Función para comparar dos vectores
bool isEqualArray(byte* arrayA, byte* arrayB, int length)
{
  for (int index = 0; index < length; index++)
  {
    if (arrayA[index] != arrayB[index]) return false;
  }
  return true;
}

//Funcions per a la comunicació WIFI && Mosquitto
void connectToWifi() {
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //if (DEBUG) Serial.println(WiFi.status());
  }

  if (DEBUG) Serial.println(F("Subscribe to abrirfinal cerrarfinal reinici reset resetPortaFinal"));
  client.subscribe("sala1/obrirportafinal");
  client.subscribe("sala1/tancarportafinal");
  client.subscribe("sala1/simularfid");
  client.subscribe("sala1/reinici");   
  client.subscribe("sala1/reset"); 
  client.subscribe("sala1/resetPortaFinal");  

  if (DEBUG)  {
    Serial.print(F("Connected as "));
    Serial.println(WiFi.localIP());
  }
} 


 
void callback(char* topic, byte* payload, unsigned int length) {
  if (DEBUG) {
    Serial.print(F("Message arrived"));
    Serial.println(topic);
  }
  int res=strcmp(topic,"sala1/obrirportafinal");
  if (res == 0) {    //OBRIR PORTA
    digitalWrite(RELE_ROIG, LOW);
    digitalWrite(RELE_BLANC,LOW);
    digitalWrite(RELE_VERD, HIGH);        
    digitalWrite(RELE_PORTA, LOW);        
  }
  res=strcmp(topic,"sala1/simularfid");
  if (res == 0) { //SIMULA RFID Detectat
    Serial.println("ENTRA en simularfid");
     digitalWrite(RELE_ROIG, LOW);
     digitalWrite(RELE_BLANC, LOW);
     //digitalWrite(RELE_VERD,HIGH);
     
     obrir = millis();
     entra=true;
     client.publish("sala1/rfid","on");
  } 
  res=strcmp(topic,"sala1/reset");
  int resu = strcmp(topic,"sala1/resetPortaFinal");  
  if (res == 0 || resu == 0) {
    //Serial.print("R");
    ESP.restart();
  }  
  res=strcmp(topic,"sala1/tancarportafinal");
  resu = strcmp(topic,"sala1/reinici");  
  if (res == 0 || resu == 0) {    //TANCAR
    digitalWrite(RELE_ROIG, HIGH);
    digitalWrite(RELE_BLANC,HIGH);
    digitalWrite(RELE_VERD, LOW);        
    digitalWrite(RELE_PORTA, HIGH);        
    entra=false;
    obrir=0;
  }  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    if (DEBUG) Serial.print(F("Attempting MQTT connection..."));
    // Attempt to connect
    if (client.connect("WemosPortaFinal")) {
      //Serial.println(F("connected"));      

      if (DEBUG) Serial.print(F("Subscribe to abrirfinal cerrarfinal reinici reset resetPortaFinal"));
      client.subscribe("sala1/obrirportafinal");
      client.subscribe("sala1/tancarportafinal");
      client.subscribe("sala1/simularfid");
      client.subscribe("sala1/reinici");   
      client.subscribe("sala1/reset"); 
      client.subscribe("sala1/resetPortaFinal");  
    
    } else {
      if (DEBUG) {
        Serial.print(F("failed, rc="));
        Serial.print(client.state());
        Serial.println(" try again in 2 seconds");
      }
      // Wait 1 seconds before retrying
      delay(1000);
    }
  }
}

void setup(){
  Serial.begin(9600);
  if (DEBUG)  Serial.println("INICIO ");    
  
   //Conexio Wifi & Mosquitto  
  connectToWifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  delay(40);       // Optional delay. Some board do need more time after init to be ready, see Readme
  
  if (DEBUG) {
    mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
    Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));    
  }

  //Ponemos los 4 pines en modo OUTPUT
  pinMode(RELE_BLANC,OUTPUT);
  pinMode(RELE_ROIG,OUTPUT);
  pinMode(RELE_VERD,OUTPUT);
  pinMode(RELE_PORTA,OUTPUT);

  digitalWrite(RELE_ROIG, HIGH);
  digitalWrite(RELE_VERD, LOW);
  digitalWrite(RELE_BLANC, HIGH);
  digitalWrite(RELE_PORTA, HIGH);
  
}

void loop() {

  cont++;
  if (cont % 10 == 0 && DEBUG) {
    Serial.println(F("Funciona "));
  }
  
  // Detectar tarjeta  
  if (mfrc522.PICC_IsNewCardPresent()){
    if (DEBUG) Serial.println(F("ENTRA"));
    //Seleccionamos una tarjeta
    if (mfrc522.PICC_ReadCardSerial()) {

      if (DEBUG) {
        Serial.print(F("Card UID:"));
        printArray(mfrc522.uid.uidByte, mfrc522.uid.size);
        Serial.println();
      }
      // Comparar ID con las claves válidas
      if (isEqualArray(mfrc522.uid.uidByte, validKey1, 4)) {
        if (DEBUG) Serial.println(F("Tarjeta valida"));
        if (!enviaRFID) {
          digitalWrite(RELE_ROIG, LOW);
          digitalWrite(RELE_BLANC, LOW);
          obrir = millis();
          entra=true;        
          if (DEBUG) Serial.println(F("Envia sala1/RFID"));
          client.publish("sala1/rfid","on");
          enviaRFID=true;
        }               
        
      }else if (isEqualArray(mfrc522.uid.uidByte, resetKey, 4)){
          if (DEBUG) Serial.println(F("Tarjeta reset") );
          digitalWrite(RELE_ROIG, HIGH);
          digitalWrite(RELE_BLANC,HIGH);
          digitalWrite(RELE_VERD, LOW);        
          digitalWrite(RELE_PORTA, HIGH);        
          entra=false;
          sale=false;
          obrir=0;
          obrirPorta=0;
          
      }else if (DEBUG) {
        Serial.println(F("Tarjeta invalida"));
        mfrc522.PICC_HaltA();
      } else {
      // Finalizar lectura actual
      mfrc522.PICC_HaltA();
      }
    }
  }
  if (entra && (millis() - obrir) > tempsFinal) {
    Serial.println(F("ENTRA en obrir porta relé verd"));
    client.publish("sala1/portafinal","on");
    digitalWrite(RELE_VERD, HIGH);        
    //digitalWrite(RELE_PORTA, LOW);
    entra=false;
    sale=true;    
    enviaRFID=false;
    obrirPorta=millis();
 }else if (sale && (millis() - obrirPorta) > tempsObrirPorta){
    digitalWrite(RELE_PORTA,LOW);    
 }
  
  
  if (!client.connected()) {
    reconnect();
  }  
  
  client.loop();
  delay(100);
  
}
 
