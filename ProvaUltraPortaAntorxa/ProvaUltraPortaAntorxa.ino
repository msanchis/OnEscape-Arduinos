  /*  
 * TODO <> 
 *   
 * Arduino NANO (pared magatzem)
 * 
 * 4 reles
 *   1 control porta entrada sala2 PIN DIG 2 (12V)
 *   1 control antorxa (24V) PIN DIGITAL 3 (Funcionen amb els 11,5V de la Font Arduinos)
 *   
 *   1 control llum ultravioleta sala punxos PIN DIG 5 (220V AC)
 *   
 ***********************
 * 
 * Per resetejar el dispositiu
 *  sala2/reset
 *  sala2/resetUltraPortaAntorxa
 * 
 * Per establir dificultat de la sala (futur)
 *  sala2/dificultat
 * 
 * Controla els leds centrals de la sala2
 *  sala2/encenUltraPunxos
 *  sala2/apagaUltraPunxos
 *  
 *  sala2/obriPortaSala2
 *  sala2/tancaPortaSala2
 *  
 * Controla el relé de les antorxes <encendre o apagar-les totes>
 *  sala2/encenAntorxaPunxos
 *  sala2/apagaAntorxaPunxos
 *  sala2/iniciaPunxos (LEDSALA2)
 *  sala2/acordCorrecte (ORGAN)
 *  
 ***********************
 */
//#include <Ethernet.h> //Conexion Ethernet con carcasa W5100
#include <EthernetENC.h> //Conexio Ethernet per al Nano
#include <PubSubClient.h> //Conexio MQTT

//VARIABLES i objectes de la xarxa i client Mosquitto
const byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xEF, 0xB2};//mac del arduino
const IPAddress ip( 192, 168, 68, 210); //Ip fija del arduino
const IPAddress server( 192, 168, 68, 55); //Ip del server de mosquitto

EthernetClient ethClient; //Interfaz de red ethernet
PubSubClient client(ethClient); //cliente MQTT

const int RelPorta = 2; // Pin Rele Porta (12V)
const int RelAntorxa = 3; // Pin Rele AntorxaPuntxos (24V)
const int Rele = 4; // Pin Rele BUIT
const int RelUltra = 5; // Pin Rele UltraVioletaPuntxos (220V)

//#define DEBUG_ULTRA//Comentar quant no s'utilitze el DEBUG per espai en memòria dinàmica

void(* resetFunc) (void) = 0; //declare reset function @ address 0


void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  #ifdef DEBUG_ULTRA
    Serial.println(F("Inicia Arduino"));
  #endif

  //MQTT
  client.setServer(server, 1883);
  client.setCallback(callback);

  //Ethernet.begin(mac);   
  Ethernet.begin(mac,ip);
      
  delay(1000); //Cambio de 1000 a 100
  #ifdef DEBUG_ULTRA
    Serial.println(F("connectant..."));
    Serial.println(Ethernet.localIP());
    Serial.println(F("Definició dels PINS dels LEDS"));
  #endif

  pinMode(RelPorta, OUTPUT);
  pinMode(RelAntorxa, OUTPUT);
  pinMode(Rele, OUTPUT);
  pinMode(RelUltra, OUTPUT);

  digitalWrite(RelPorta,HIGH); //La porta comensa tancada de inici (Oberta de pulsador magatzem)
  digitalWrite(RelAntorxa,HIGH);
  digitalWrite(RelUltra,HIGH);
}

void callback(char* topic, byte* payload, unsigned int length) {
  #ifdef DEBUG_ULTRA
    Serial.print(F("Message arrived"));
    Serial.println(topic);
  #endif
  int res=strcmp(topic,"sala2/reset");
  if (res == 0) {    //RESET para toda la sala     
     resetFunc();
  }
  res=strcmp(topic,"sala2/resetUltraPortaAntorxa");
  if (res == 0) {    //RESET Arduino Nano UltraPortaAntorxa      
     resetFunc();
  }
  
  res=strcmp(topic,"sala2/encenUltraPunxos");
  if (res == 0) {    //Encen Llum Ultravioleta Sala Punxos     
     digitalWrite(RelUltra,LOW);
  }
  res=strcmp(topic,"sala2/apagaUltraPunxos");
  if (res == 0) {    //Apaga Llum Ultravioleta Sala Punxos      
     digitalWrite(RelUltra,HIGH);
  }
  res=strcmp(topic,"sala2/obriPortaSala2");
  if (res == 0) {    //Obri Porta principal Sala2     
      digitalWrite(RelPorta,LOW);
  }
  res=strcmp(topic,"sala2/tancaPortaSala2");
  if (res == 0) {    //Tanca Porta principal Sala2 (OJO! tres interruptors més físics en serie)
     digitalWrite(RelPorta,HIGH);
  }
  res=strcmp(topic,"sala2/encenAntorxaPunxos");
  if (res == 0) {    //Encen la antorxa de la sala de punxos (OJO! un interruptor (relé 24V) en serie)      
      digitalWrite(RelAntorxa,HIGH);
  }
  res=strcmp(topic,"sala2/apagaAntorxaPunxos");
  if (res == 0) {    //Apaga la antorxa de la sala de punxos 
     digitalWrite(RelAntorxa,LOW);
  }
  res=strcmp(topic,"sala2/iniciaPunxos");
  if (res == 0) {    //Quan inicia la baixada dels puntos Apaga la antorxa de la sala de punxos i encen UltraVioleta
     digitalWrite(RelAntorxa,LOW);
     digitalWrite(RelUltra,LOW);
  }
  res=strcmp(topic,"sala2/acordCorrecte");
  if (res == 0) {    //Encen la antorxa de la sala de punxos i apaga UltraVioleta
     digitalWrite(RelAntorxa,HIGH);
     digitalWrite(RelUltra,HIGH);     
  }
  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    #ifdef DEBUG_ULTRA 
      Serial.print(F("Attempting MQTT connection..."));
      Serial.print(Ethernet.localIP());
    #endif
    // Attempt to connect
    if (client.connect("UltraPortaAntorxa")) {
      //Serial.println(F("connected"));      

      #ifdef DEBUG_ULTRA 
        Serial.print(F("Subscribe to reset resetPedestal reiniciPedestal estat0Pedestal estat1Pedestal"));
      #endif
      client.subscribe("sala2/reset"); 
      client.subscribe("sala2/resetUltraPortaAntorxa");
      client.subscribe("sala2/dificultat");
      client.subscribe("sala2/encenUltraPunxos");
      client.subscribe("sala2/apagaUltraPunxos");
      client.subscribe("sala2/obriPortaSala2");
      client.subscribe("sala2/tancaPortaSala2");
      client.subscribe("sala2/encenAntorxaPunxos");
      client.subscribe("sala2/apagaAntorxaPunxos");
      client.subscribe("sala2/iniciaPunxos");
      client.subscribe("sala2/acordCorrecte");

    } else {
      #ifdef DEBUG_ULTRA
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
  if (!client.connected()) {
    reconnect();
  }  
  
  client.loop();

}
