/*  
 * TODO <> 
 *   
 * Arduino NANO (baix holograma)
 * 
 * 1 rele que controla electroiman clau prova final (cor)
 *  
 * 1 PIR que detecta quan es tira el cor pel forat  
 * 
 ***********************
 * 
 * Per resetejar el dispositiu
 *  sala2/reset
 *  sala2/resetCor
 * 
 * Per establir dificultat de la sala (futur)
 *  sala2/dificultat
 * 
 * Controla el rele electroiman
 *  sala2/obriClauCor
 *  sala2/tancaClauCor
 *  
 * Activa el sensor quan apleguen aquests events (estat=1)
 *  sala2/activaCor
 *  sala2/finalPedestal
 *  
 * Activa relé bomba aigua
 *  sala2/activaBombaAigua
 *  sala2/desactivaBombaAigua
 *  
 * Desactiva el sensor (estat=0)
 *  sala2/desactivaCor
 *  
 ***********************
 * Quan detecta alguna cosa el PIR
 *  sala2/detectaCor
 *
 */

#include <EthernetENC.h> //Conexio Ethernet per al Nano
//#include <Ethernet.h> //Conexio Ethernet per W5100
#include <PubSubClient.h> //Conexio MQTT

//VARIABLES i objectes de la xarxa i client Mosquitto
 const byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xEE, 0x1D};//mac del arduino
const IPAddress ip( 192, 168, 68, 209); //Ip fija del arduino PRODUCCIO
const IPAddress server( 192, 168, 68, 55); //Ip del server de mosquitto PROD
//const IPAddress ip( 192, 168, 1, 20); //Ip fija del arduino DEBUG
//const IPAddress server( 192, 168, 1, 69); //Ip del server de mosquitto DEBUG

EthernetClient ethClient; //Interfaz de red ethernet
PubSubClient client(ethClient); //cliente MQTT

#define DEBUG_LED //Comentar quant no s'utilitze el DEBUG per espai en memòria dinàmica

#define BOMBA 8 //Pin 8 per a relé Bomba Aigua REVISAR NO FUNCIONA
#define PIR 4 //Estableix el pin 4 per al sensor PIR
#define RELE 7 //Estableix el pin 7 per al relé Electroiman COR REVISAR NO FUNCIONA

int tiempo = 5000; //Tiempo antes de soltar la llave
boolean sensor; //Variable que almacena el estado del sensor (activado/desactivado)

int estat = 0; //Estat del sensor 0=apagat 1=activat - Sactiva en event FinalPedestal o ActivaCor

unsigned long temps = 0; //Guarda el moment en el que detecta alguna cosa el PIR

boolean entra=false; //Ha detectat alguna cosa el PIR

void(* resetFunc) (void) = 0; //declare reset function @ address 0

void callback(char* topic, byte* payload, unsigned int length) {
#ifdef DEBUG_LED
  Serial.print(F("Message arrived"));
  Serial.println(topic);
#endif
  int res = strcmp(topic, "sala2/reset");
  if (res == 0) {    //RESET para toda la sala
    // wdt_enable(WDTO_15MS); // Configuramos el contador de tiemp o para que se reinicie en 15ms
    resetFunc(); //llamada a la funcio de reseteig

  }
  res = strcmp(topic, "sala2/resetCor");
  if (res == 0) {    //RESET para toda la sala
    //wdt_enable(WDTO_15MS); // Configuramos el contador de tiempo para que se reinicie en 15ms
    resetFunc(); //llamada a la funcio de reset
  }

  res = strcmp(topic, "sala2/obriClauCor");
  if (res == 0) {    //obrir Rele per deixar caure la clau
    digitalWrite(RELE, HIGH);
  }

  res = strcmp(topic, "sala2/tancaClauCor");
  if (res == 0) {    //tancar Rele per imantar la clau
    analogWrite(RELE, LOW);
  }

  res = strcmp(topic, "sala2/finalPedestal");
  if (res == 0) {   //final Pedestal estat=1
    estat = 1;
  }

  res = strcmp(topic, "sala2/activaCor");
  if (res == 0) {   //activa Cor estat=1
    estat = 1;
  }

  res = strcmp(topic, "sala2/desactivaCor");
  if (res == 0) {   //activa Cor estat=1
    estat = 0;
    temps = 0;
    entra = false;
  }

  res = strcmp(topic,"sala2/activaBombaAigua");
  if (res == 0){
    digitalWrite(BOMBA,HIGH);
  }

  res = strcmp(topic,"sala2/desactivaBombaAigua");
  if (res == 0){
    digitalWrite(BOMBA,LOW);
  }

}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
#ifdef DEBUG_LED
    Serial.print(F("Attempting MQTT connection... IP: "));
    Serial.print(Ethernet.localIP());
    //Serial.print(Ethernet.linkStatus());
#endif
    // Attempt to connect
    if (client.connect("CorPir")) {
      //Serial.println(F("connected"));

#ifdef DEBUG_LED
      Serial.print(F("Subscribe to reset resetCor obriClauCor tancaClauCor"));
#endif
      client.subscribe("sala2/reset");
      client.subscribe("sala2/resetCor");
      client.subscribe("sala2/obriClauCor");
      client.subscribe("sala2/tancaClauCor");
      client.subscribe("sala2/activaCor");
      client.subscribe("sala2/desactivaCor");
      client.subscribe("sala2/activaBombaAigua");
      client.subscribe("sala2/desactivaBombaAigua");
      client.subscribe("sala2/finalPedestal");

    } else {
#ifdef DEBUG_LED
      Serial.print(F(" failed, rc="));
      Serial.print(client.state());
      Serial.println(F(" try again in 1 second"));
#endif
      // Wait 1 seconds before retrying
      delay(1000);
    }
  }
}

void setup()
{

#ifdef DEBUG_LED
  // Iniciar comunicación serie
  Serial.begin(9600);
  Serial.println(F("Inicio del controlador Corazon"));
#endif

  //MQTT
  client.setServer(server, 1883);
  client.setCallback(callback);

  Ethernet.begin(mac, ip);
 
  pinMode(PIR, INPUT); //Establece el pin del sensor como entrada
  pinMode(RELE, OUTPUT); //Establece el pin del relé electroiman llave como salida
  digitalWrite(RELE, LOW);
  pinMode(BOMBA, OUTPUT);  //Bomba salida
  digitalWrite(BOMBA, LOW);

#ifdef DEBUG_LED
  Serial.println(F("connectant..."));
  Serial.println(Ethernet.localIP());
  Serial.println(F("Definició del PIN del Cor"));
#endif
}


void loop()
{
  sensor = digitalRead(PIR); //Guarda el estado del sensor en la variable
  if (estat == 1) { //Per activar el sensor ha de aplegar event finalPedestal o activaCor
    //Serial.println(sensor);
    if (sensor == HIGH && !entra){
      client.publish("sala2/detectaCor","on");
      digitalWrite(RELE,LOW); //Desconnecta l'eletrcoiman i Solta la clau
      temps=millis();     
      entra=true; 
    }

    if (temps > 0 && millis() - temps > 8000 ){      
      digitalWrite(RELE,HIGH); //Connecta l'electroiman
      estat=0; //Es desactiva el sensor
      temps=0;
      entra=false;
    }  
  }
  if (!client.connected()) {
    reconnect();
  }

  client.loop();
}
