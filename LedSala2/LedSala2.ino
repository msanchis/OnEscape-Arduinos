    /*
 * TODO canviar delays per millis en les sequencies de llamp i trona
 * 
 * Arduino NANO (pared magatzem)
 * 
 * 4 reles
 *   3 control pistó (12V) 
 *      PIN DIGITAL 12V 5 IMPORTANT
 *      PIN DIGITAL POSITIU 4 
 *      PIN DIGITAL NEGATIU 3 
 *   1 control antorxes (24V) PIN DIGITAL 7 (Funcionen amb els 11,5V de la Font Arduinos)
 *   
 * Leds sala central PIN DIGITAL 9
 * Electroiman Sala Punxos PIN DIGITAL 2
 * 
 * Final de carrera (Reliquia 5) PIN DIGITAL 11
 * 
 ***********************
 * 
 * Per resetejar el dispositiu
 *  sala2/reset
 *  sala2/resetLed
 * 
 * Per establir dificultat de la sala (futur)
 *  sala2/dificultat
 * 
 * Controla els leds centrals de la sala2
 *  sala2/encenLeds
 *  sala2/apagaLeds
 *  
 *  sala2/trona
 *  sala2/llamp
 *  
 * Controla el relé de les antorxes <encendre o apagar-les totes>
 *  sala2/encenAntorxes
 *  sala2/apagaAntorxes
 *  
 * Controla els sostre per pujar i baixar -12V i +12 V
 *  sala2/pujaSostre
 *  sala2/baixaSostre
 *  sala2/paraSostre
 *  
 *  Controla el electroiman de la porta de la sala de puntos
 *  sala2/obriPortaPunxos
 *  sala2/tancaPortaPunxos

 ***********************
 *  
 * Events que envia
 *  
 * Per controlar el event de la baixada de punxos
 *  sala2/iniciaPunxos 
 *  sala2/estat2Punxos
 *  sala2/estat3Punxos
 *  sala2/estat4Punxos
 *  sala2/estat5Punxos
 *  sala2/finalPunxos 
 * 
 */

#include <EthernetENC.h> //Conexio Ethernet per al Nano
#include <PubSubClient.h> //Conexio MQTT

//VARIABLES i objectes de la xarxa i client Mosquitto
byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xEF, 0x12};//mac del arduino
IPAddress ip( 192, 168, 68, 203); //Ip fija del arduino
IPAddress server( 192, 168, 68, 56); //Ip del server de mosquitto

EthernetClient ethClient; //Interfaz de red ethernet
PubSubClient client(ethClient); //cliente MQTT

#define DEBUG_LED

float llum=0;
const int PinLeds= 9; //Pin per encendre i apagar els leds de la sala central

boolean iniciaSostre=false;
const int FinalCarrera = 8;  //Final de carrera de la reliquia 5 (comença la seqüencia del sostre i el organ)

const int RELE_ANTORXES=7;

const int RELE_ALIMENTACIO = 5;    //activació del relé simple y suministre de corrent del pistó
const int RELE_INVERSOR1 = 4;       //activació del relé del positiu y canvi de polaritat del pistó
const int RELE_INVERSOR2 = 3;       //activació del relé del negatiu y canvi de polaritat del pistó

const int RELE_ELECTROIMAN = 2;   //electroiman porta sala punxos

int estat=0; //Variable per determinar el estat de  la baixada del sostre
             //estat = 0 (no ha comensat)
             //estat = 1 (baixa 12 segs i para 23 segs)
             //estat = 2 (baixa 12 segs i para 23 segs)
             //.....
             //estat = 5 (baixa 12 segs i para definitivament)


//Variable de temps per a la sequencia 
unsigned long marcaTemps1 = 0;
unsigned long marcaTemps2 = 0;
unsigned long marcaTemps3 = 0;
unsigned long marcaTemps4 = 0;
unsigned long marcaTemps5 = 0;



void(* resetFunc) (void) = 0; //declare reset function @ address 0

void baixaPisto(){
    digitalWrite(RELE_ALIMENTACIO,LOW);
    digitalWrite(RELE_INVERSOR1,LOW);
    digitalWrite(RELE_INVERSOR2,LOW);  
}

void pujaPisto(){
  digitalWrite(RELE_ALIMENTACIO,LOW);
  digitalWrite(RELE_INVERSOR1,HIGH);
  digitalWrite(RELE_INVERSOR2,HIGH);
}

void paraPisto(){
  digitalWrite(RELE_ALIMENTACIO,HIGH);
  digitalWrite(RELE_INVERSOR1,HIGH);
  digitalWrite(RELE_INVERSOR2,HIGH);
}


void setup() {

  #ifdef DEBUG_LED
    // Iniciar comunicación serie
    Serial.begin(9600);
    Serial.println(F("[LEDs] Inicio del controlador LED"));
  #endif

   //MQTT
  client.setServer(server, 1883);
  client.setCallback(callback);

  Ethernet.begin(mac,ip);
      
  //delay(1000); //Cambio de 1000 a 100

  pinMode(PinLeds,OUTPUT);  

  pinMode(RELE_ALIMENTACIO, OUTPUT);           // Pin relé alimentación como salida
  pinMode(RELE_INVERSOR1, OUTPUT);              // Pin relé inversor1 como salida
  pinMode(RELE_INVERSOR2, OUTPUT);              // Pin relé inversor2 como salida
  digitalWrite(RELE_ALIMENTACIO,HIGH);
  digitalWrite(RELE_INVERSOR1,HIGH);
  digitalWrite(RELE_INVERSOR2,HIGH);

  pinMode(RELE_ANTORXES,OUTPUT);                // Pin relé antorxes sala
  digitalWrite(RELE_ANTORXES,LOW);

  pinMode(RELE_ELECTROIMAN,OUTPUT);             //Pin relé electroiman Porta Puntxos
  digitalWrite(RELE_ELECTROIMAN ,HIGH);

  pinMode(FinalCarrera, INPUT_PULLUP); //Declaramos el pin como entrada
  
  #ifdef DEBUG_LED
    Serial.println(F("connectant..."));
    Serial.println(Ethernet.localIP());
    Serial.println(F("Definició del PIN dels LEDS"));
  #endif
   
}

void callback(char* topic, byte* payload, unsigned int length) {
  #ifdef DEBUG_LED
    Serial.print(F("Message arrived"));
    Serial.println(topic);
  #endif
  int res=strcmp(topic,"sala2/reset");
  if (res == 0) {    //RESET para toda la sala
     // wdt_enable(WDTO_15MS); // Configuramos el contador de tiempo para que se reinicie en 15ms      
     resetFunc(); //llamada a la funcio de reseteig
     
  }
  res=strcmp(topic,"sala2/resetLeds");
  if (res == 0) {    //RESET para toda la sala
     //wdt_enable(WDTO_15MS); // Configuramos el contador de tiempo para que se reinicie en 15ms           
     resetFunc(); //llamada a la funcio de reset
  }
 
  res=strcmp(topic,"sala2/encenLeds");
  if (res == 0) {    //encendre llum blanca
      analogWrite(PinLeds,255);
  }
  res=strcmp(topic,"sala2/apagaLeds");
  if (res == 0) {    //apagar llum
      analogWrite(PinLeds,0);
  }

  res=strcmp(topic,"sala2/encenAntorxes");
  if (res == 0) {    //activar antorxes
      digitalWrite(RELE_ANTORXES,LOW);
  }
  res=strcmp(topic,"sala2/apagaAntorxes");
  if (res == 0) {    //apagar antorxes
      digitalWrite(RELE_ANTORXES,HIGH);
  }

  res=strcmp(topic,"sala2/llamp");
  if (res == 0) {    //ejecuta secuencia luces tormenta
      
      analogWrite(PinLeds, 255);   
      delay(300);    
      analogWrite(PinLeds, 100);    
      delay(100);
      analogWrite(PinLeds, 255);   
      delay(200);    
      analogWrite(PinLeds,0);       
  }
  
  res=strcmp(topic,"sala2/trona");
  if (res == 0) {    //ejecuta secuencia luces tormenta
      
      analogWrite(PinLeds, 100);    
      delay(100);     
      analogWrite(PinLeds, 255);    
      delay(200);    
      analogWrite(PinLeds,0);
      delay(1200);
      analogWrite(PinLeds, 255);    
      delay(100);    
      analogWrite(PinLeds, 100);    
      delay(200);
      analogWrite(PinLeds,0);        
      delay(3000);
      analogWrite(PinLeds, 100);    
      delay(100);     
      analogWrite(PinLeds, 255);    
      delay(200);    
      analogWrite(PinLeds,0);
      
  }

  res=strcmp(topic,"sala2/acordCorrecte");
  if (res == 0) {    //final de la prova del sostre
     #ifdef DEBUG_LED
        Serial.println(F("Entra a ACORD CORRECTE PARA Pisto i ObriPortaPunxos"));
     #endif

    
     digitalWrite(RELE_ELECTROIMAN,LOW);
     paraPisto();
     estat=6;    
  }

  res=strcmp(topic,"sala2/pujaSostre");
  if (res == 0) {    //controlador Pisto
     #ifdef DEBUG_LED
        Serial.println(F("Entra a puja Sostre "));
     #endif

     pujaPisto();     
  }

  res=strcmp(topic,"sala2/baixaSostre");
  if (res == 0) {    //controlador Pisto
     #ifdef DEBUG_LED
        Serial.println(F("Entra a baixa Sostre "));
     #endif

     baixaPisto();     
  }

  res=strcmp(topic,"sala2/paraSostre");
  if (res == 0) {    //controlador Pisto
     #ifdef DEBUG_LED
        Serial.print(F("Entra a para Sostre "));
     #endif

     paraPisto();     
  }

  res=strcmp(topic,"sala2/obriPortaPuntxos");
  if (res == 0) {    //controlador Pisto
     #ifdef DEBUG_LED
        Serial.print(F("Entra a obriPortaPUntxos "));
     #endif

     digitalWrite(RELE_ELECTROIMAN,LOW);
  }

  res=strcmp(topic,"sala2/tancaPortaPuntxos");
  if (res == 0) {    //controlador Pisto
     #ifdef DEBUG_LED
        Serial.print(F("Entra a tancaPortaPUntxos "));
     #endif

     digitalWrite(RELE_ELECTROIMAN,HIGH);
  }
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    #ifdef DEBUG_LED
      Serial.print(F("Attempting MQTT connection... IP: "));
      Serial.print(Ethernet.localIP());
      Serial.print(Ethernet.linkStatus());
    #endif
    // Attempt to connect
    if (client.connect("LEDSala2")) {
      //Serial.println(F("connected"));      

      #ifdef DEBUG_LED
        Serial.print(F("Subscribe to reset resetLeds encenLeds apagaLeds llamp trona"));
      #endif
      client.subscribe("sala2/reset"); 
      client.subscribe("sala2/resetLeds");  
      client.subscribe("sala2/encenLeds");
      client.subscribe("sala2/apagaLeds");
      client.subscribe("sala2/trona");      
      client.subscribe("sala2/llamp");    
      client.subscribe("sala2/acordCorrecte");
      client.subscribe("sala2/apagaAntorxes");
      client.subscribe("sala2/encenAntorxes");
      client.subscribe("sala2/pujaSostre");
      client.subscribe("sala2/baixaSostre");
      client.subscribe("sala2/paraSostre");
      client.subscribe("sala2/obriPortaPuntxos");
      client.subscribe("sala2/tancaPortaPuntxos");
      
           
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

boolean estat1=false;
boolean estat2=false;
boolean estat3=false;
boolean estat4=false;
boolean estat5=false;

void loop() {

  boolean existeReliquia = digitalRead(FinalCarrera);

  if (existeReliquia && !iniciaSostre) {
    #ifdef DEBUG_LED
    // Inicia Sostre -Final Carrera Reliquia 5-     
      Serial.println(F("[FinalCarrera] Estat=1 Baixa Pisto Sostre"));
    #endif
    client.publish("sala2/iniciaPunxos","on");
    iniciaSostre=true;  
    marcaTemps1=millis();
    estat=1;
    baixaPisto();
  }

  if (estat > 0){
  
    switch(estat){    
  
      case 1:
              if (!estat1 && millis()-marcaTemps1 > 20000){ //NO CANVIAR 20000
                #ifdef DEBUG_LED
                  // Final 12 segs baixant
                  Serial.println(F("Para Pisto Estat1=true "));
                #endif
                paraPisto();
                estat1=true;              
              }
              if (estat1 && millis()-marcaTemps1 > 35000){ 
                #ifdef DEBUG_LED
                // Inicia Sostre -Final Carrera Reliquia 5-     
                  Serial.println(F("Canvia Estat=2 Baixa Pisto Sostre"));
                #endif
                marcaTemps2=millis();
                estat=2;
                client.publish("sala2/estat2Punxos","on");
                baixaPisto();              
              }
      break;
      
      case 2:
              if (!estat2 && millis()-marcaTemps2 > 13000){  //NO CANVIAR 13000
                paraPisto();
                estat2=true;              
              }
              if (estat2 && millis()-marcaTemps2 > 35000){
                marcaTemps3=millis();
                estat=3;
                client.publish("sala2/estat3Punxos","on");
                baixaPisto();              
              }
      break;
      case 3:
              if (!estat3 && millis()-marcaTemps3 > 13000){  //NO CANVIAR 13000
                paraPisto();
                estat3=true;              
              }
              if (estat2 && millis()-marcaTemps3 > 35000){
                marcaTemps4=millis();
                estat=4;
                client.publish("sala2/estat4Punxos","on");
                baixaPisto();              
              }
      break;
      case 4:
              if (!estat4 && millis()-marcaTemps4 > 13000){  //NO CANVIAR 13000
                client.publish("sala2/finalPunxos","on");
                paraPisto();
                estat4=true;              
              }
             /* if (estat4 && millis()-marcaTemps4 > 35000){
                marcaTemps5=millis();
                estat=5;
                client.publish("sala2/estat5Punxos","on");
                baixaPisto();              
              }
      break;
      case 5:
              if (!estat5 && millis()-marcaTemps5 > 12000){
                client.publish("sala2/finalPunxos","on");
                paraPisto();
                estat5=true;              
              }*/
      break;
    }
  }//Final if estat > 0
   
  if (!client.connected()) {
    reconnect();
  }  
  
  client.loop();
  
}
