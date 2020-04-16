/*
 * Aplicació per a encendre i apagar les llums de la caixa d'acces a la porta
 * Esta conectat per I2C (pins A4 i A5) a Arduino Nano
 * Arduino Nano enviarà una senyal quan s'arrime la tarjeta RFID al lector RC522
 * 
 * Transmision por I2C
 * byte 8 --> ABRIR PUERTA (LUZ VERDE Y BLANCA)
 * byte 4 --> CERRAR PUERTA (LUZ ROJA Y BLANCA)
 */

#include <Ethernet.h> //Conexion Ethernet con carcasa W5100
#include <PubSubClient.h> //Conexion MQTT
#include <Wire.h>
#include <avr/wdt.h> // Incluir la librería de ATmel

const int RELE_BLANC = 3;
const int RELE_ROIG = 4;
const int RELE_VERD = 2;

const int RELE_PORTA = 5;
unsigned long obrir=0;
boolean entra=false;

long tempsFinal = 5000;

EthernetClient ethClient; //Interfaz de red ethernet
PubSubClient client(ethClient); //cliente MQTT

byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xF8, 0xF8};//mac del arduino
//IPAddress ip(192, 168, 68, 41); //Ip fija del arduino
IPAddress server(192, 168, 68, 1); //Ip del server de mosquitto
//IPAddress server(192, 168, 1, 5); //Ip del server de mosquitto
//byte DNS[] = {8, 8, 8, 8};
//byte gateway[] = {192, 168, 1, 1};
//byte subnet[] = {255, 255, 255, 0};



void reconnect() {

  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print(F("Attempting MQTT connection..."));
    // Attempt to connect
    if (client.connect("PortaFinal")) {
      Serial.println(F("connected"));      
      client.subscribe("sala1/obrirportafinal");
      client.subscribe("sala1/tancarportafinal");
      client.subscribe("sala1/simularfid");
      client.subscribe("sala1/reinici");   
      client.subscribe("sala1/reset"); 
      client.subscribe("sala1/resetPortaFinal");  
      
    } else {
      Serial.print(F("failed, rc="));
      Serial.print(client.state());
      //Serial.println(" try again in 5 seconds");
      // Wait 3 seconds before retrying
      delay(3000);
    }
  }

}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print(F("Message arrived ["));
  Serial.print(topic);
  Serial.print(F("] "));
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
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
     digitalWrite(RELE_ROIG, LOW);
     digitalWrite(RELE_BLANC, LOW);
     obrir = millis();
     entra=true;
     client.publish("sala1/rfid","on");
  } 
  res=strcmp(topic,"sala1/reset");
  int resu = strcmp(topic,"sala1/resetPortaFinal");  
  if (res == 0 || resu == 0) { //RESET PLACA
     wdt_enable(WDTO_15MS); // Configuramos el contador de tiempo para que se reinicie en 15ms
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

// Función que se ejecuta siempre que se reciben datos del master
// siempre que en el master se ejecute la sentencia endTransmission
// recibirá toda la información que hayamos pasado a través de la sentencia Wire.write
void receiveEvent(int howMany) {
 
  int pinOut = 0;
  int estado = 0;
  
  // Si hay un byte disponible
  if (Wire.available() == 1)
  {
    estado = Wire.read();
    Serial.print(F("Estado "));
    Serial.println(estado);

    if (estado == 8) { //OBRIR PORTA
        digitalWrite(RELE_ROIG, LOW);
        digitalWrite(RELE_BLANC, LOW);
        obrir = millis();
        entra=true;
        client.publish("sala1/rfid","on");
    }else if (estado == 4) { //TANCAR PORTA -RESET-
        digitalWrite(RELE_ROIG, HIGH);
        digitalWrite(RELE_BLANC,HIGH);
        digitalWrite(RELE_VERD, LOW);  
        digitalWrite(RELE_PORTA, HIGH);
        obrir=0;
        entra=false;
    }    
  }
}

void setup() {
  
  Serial.begin(9600); // Iniciar serial
  Serial.println("INICIA");
 
  //Ponemos los 4 pines en modo OUTPUT
  pinMode(RELE_BLANC,OUTPUT);
  pinMode(RELE_ROIG,OUTPUT);
  pinMode(RELE_VERD,OUTPUT);
  pinMode(RELE_PORTA,OUTPUT);

  digitalWrite(RELE_ROIG, HIGH);
  digitalWrite(RELE_VERD, LOW);
  digitalWrite(RELE_BLANC, HIGH);
  digitalWrite(RELE_PORTA, HIGH);

  //MQTT
  client.setServer(server, 1883);
  client.setCallback(callback);

   Ethernet.begin(mac);   
   //Ethernet.begin(mac,ip);
   //Ethernet.begin(mac, ip, DNS, gateway, subnet);    
   delay(10000); //Espera per xarxa i per I2C
   Serial.println(F("connecting..."));
   Serial.println(Ethernet.localIP());

  // Unimos este dispositivo al bus I2C con dirección 1 -ESCLAU-
  Wire.begin(1);
 
  // Registramos el evento al recibir datos
  Wire.onReceive(receiveEvent);
  
}
 
void loop() {


 if (entra && (millis() - obrir) > tempsFinal) {
    client.publish("sala1/portafinal","on");
    digitalWrite(RELE_VERD, HIGH);        
    digitalWrite(RELE_PORTA, LOW);
    entra=false;
 }
 if (!client.connected()) {
     reconnect();
 }
 client.loop();
 delay(100);
}
