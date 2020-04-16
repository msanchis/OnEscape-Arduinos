/*
 * Actualitzat a dia 11/11/19
   
   Telephone Dial Interpreter and Game
  
  Comunicacion entre MEGA y Wemos en la misma placa a través de Serial3
    'A' -->  client.publish("sala1/abrirsecreta", "ON");
    'C' -->  client.publish("sala1/cerrarsecreta", "ON");
    '-123456' --> client.publish("sala1/telefono", "123456");

  Comunicacion entre Wemos y MEGA para reproducir audio en el teléfono a través de Serial
    'O' -->  client.subscribe("sala1/audioabrirsecreta");
    'T' -->  client.subscribe("sala1/audiotrofeo");    
    (char)c[0] == '-'  client.publish("sala1/telefono", cod.c_str());
    
    
*/
#include <ESP8266WiFi.h>   //Conexion wifi                      
#include <PubSubClient.h> //Conexion Mosquitto 
//Variables per a la conexió Wifi & Mosquitto
/*
const char* ssid = "OntitelFTTH_5493";
const char* password = "mm45WQ9u";
const char* mqtt_server = "192.168.1.7";
*/

const char* ssid = "OnEscape";
const char* password = "M1cky&S1ny0";
const char* mqtt_server = "192.168.69.1";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
char value[50];

void setup(){
  Serial.begin(115200);
  //Serial.println("INICIO");    
   //Conexio Wifi & Mosquitto
  connectToWifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  value[0]='C';
  
}

void loop(){
  if (Serial.available()){ 
    String c = Serial.readString() ;  
    
    if ( (char)c[0] == 'A' ) { //Puerta Abierta value=1       
        client.publish("sala1/pasecreta", "ON");        
    }else if ( (char)c[0] == 'C' ){ //Puerta Cerrada value=0        
        client.publish("sala1/pcsecreta", "ON");
    }else if ( (char)c[0] == '-' ){ //Código erróneo introducido        
        String cod = c.substring(1);
        client.publish("sala1/telefono", cod.c_str());
    }
   }
  
  if (!client.connected()) {
    reconnect();
  }  
  
  client.loop();

}

//Funcions per a la comunicació WIFI && Mosquitto
void connectToWifi() {
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.println(WiFi.status());
  }
   client.subscribe("sala1/abrirsecreta");
   client.subscribe("sala1/cerrarsecreta");
   client.subscribe("sala1/audioabrirsecreta");
   client.subscribe("sala1/audiotrofeo");
   client.subscribe("sala1/reset"); 
   client.subscribe("sala1/resetRotatori");
   

  //Serial.print("Connected as "+WiFi.localIP());
}


 
void callback(char* topic, byte* payload, unsigned int length) {
  //Serial.print(F("Message arrived ["));
  //Serial.print(topic);
  int res=strcmp(topic,"sala1/abrirsecreta");
  if (res == 0) {
    Serial.print("A");
  }
  res=strcmp(topic,"sala1/cerrarsecreta");
  if (res == 0) {
    Serial.print("C");    
  }
  res=strcmp(topic,"sala1/audioabrirsecreta");
  if (res == 0) {
    Serial.print("O");    
  }
  res=strcmp(topic,"sala1/audiotrofeo");
  if (res == 0) {
    Serial.print("T");    
  }
  res=strcmp(topic,"sala1/reset"); //PROVAR Si FUNCIONA
  int resu=strcmp(topic,"sala1/resetRotatori");
  if (res == 0 || resu == 0) {
    //Serial.print("R");
    ESP.restart();
  }
  
  //Serial.print(F("] "));
  //for (int i=0;i<length;i++) {
  //  Serial.print((char)payload[i]);
  //}
//  Serial.println(freeRam());
  Serial.flush();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    //Serial.print(F("Attempting MQTT connection..."));
    // Attempt to connect
    if (client.connect("WemosRotatori")) {
      //Serial.println(F("connected"));      

      client.subscribe("sala1/abrirsecreta");
      client.subscribe("sala1/cerrarsecreta");
      client.subscribe("sala1/audioabrirsecreta");
      client.subscribe("sala1/audiotrofeo");
      client.subscribe("sala1/reset");
      client.subscribe("sala1/resetRotatori");
   
    } else {
      //Serial.print(F("failed, rc="));
      //Serial.print(client.state());
      //Serial.println(" try again in 5 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}
