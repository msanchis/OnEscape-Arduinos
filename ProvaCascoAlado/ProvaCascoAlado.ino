/*
 * Es poden arribar a cada estat directament amb els següents missatges 
 * PUJA/BAIXA <-- al rebre "sala1/casco1"
 * PUJA/BAIXA <-- al rebre "sala1/casco2"
 * ...
 * PUJA/BAIXA = 5 <-- al rebre "sala1/casco5" 
 * 
 * Per baixar el pistó
 * RELE_ALIMENTACIO = LOW
 * RELE_INVERSOR1 = HIGH
 * RELE_INVERSOR2 = HIGH
 * 
 * Per pujar el pistó
 * RELE_ALIMENTACIO = LOW
 * RELE_INVERSOR1 = LOW  
 * RELE_INVERSOR2 = LOW
 * 
 * Parat
 * RELE_ALIMENTACIO = HIGH
 * RELE_INVERSOR1 = HIGH
 * RELE_INVERSOR2 = HIGH
 
 * 
 */
#include <SPI.h>
#include <Ethernet.h> //Conexion Ethernet con carcasa W5100
#include <PubSubClient.h> //Conexion MQTT
//#include <avr/wdt.h> // Incluir la librería de ATmel per a reiniciar el arduino
#include <EEPROM.h> // Per a guardar un valor boolean en la memoria no volatil (comensa)



int estat=0; //La VARIABLE més important de la sala... determina l'estat en que es troba el sistema
boolean para=false; //Per a enviar un event MQTT només
int cont=0; //Variable per a enviar sala1/finalcarrera cada 100 deteccions

unsigned long marcaTemps1 = 0;
unsigned long marcaTemps2 = 0;
unsigned long marcaTemps3 = 0;
unsigned long marcaTemps4 = 0;
unsigned long marcaTemps5 = 0;
unsigned long marcaTemps6 = 0; //reinici (puja el pisto i variables a false)
unsigned long marcaTemps7 = 0; //baixa el pisto del tot

unsigned long marcaTemps8 = 0; //baixa un escalo (tempsTotal)

unsigned long tiempoCerrada=0; //per medir el temps de la porta secreta tancada abans de començar video

boolean entra1 = false;
boolean entra2 = false;
boolean entra3 = false;
boolean entra4 = false;
boolean entra5 = false;

boolean escalo = false;

int pulsador = A0; //Definimos el puerto del final de carrera lógico
boolean comensa = false;
int puerta = 7; //Definimos el puerto para el sensor de puerta abierto o cerrado

// DECLARACION DE VARIABLES PARA PINES
const int RELE_ALIMENTACIO = 2;    //activación del relé simple y suministro de corriente del pistón
const int RELE_INVERSOR1 = 3;       //activación del relé del positivo y cambio de polaridad del pistón
const int RELE_INVERSOR2 = 4;       //activación del relé del negativo y cambio de polaridad del pistón
const int FINAL_CARRERA1 = 5;        //detección del estado del final de carrera 1 

//const int tempsTotal = 60000; //50-60 segs per a completar el recorregut
//const int tempsEscalo = 11200; //11 segs per baixar un escalo

//Variable per veure els prints al monitor
boolean DEBUG = true;

//VARIABLES i objectes de la xarxa i client Mosquitto
byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0x2F};//mac del arduino
IPAddress ip(192, 168, 68, 33); //Ip fija del arduino
IPAddress server(192, 168, 68, 1); //Ip del server de mosquitto

EthernetClient ethClient; //Interfaz de red ethernet
PubSubClient client(ethClient); //cliente MQTT 

void(* resetFunc) (void) = 0; //declare reset function @ address 0

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  if (DEBUG) {
    Serial.println(F("Inicia Arduino"));
  }
  //MQTT
  client.setServer(server, 1883);
  client.setCallback(callback);

 // EEPROM.write(0,0); //Per inicialitzar la variable
  EEPROM.get(0,comensa); //Llegim el valor de la variable bool

  //Ethernet.begin(mac);   
  Ethernet.begin(mac,ip);
      
  //delay(2000);
  if (DEBUG) {
    Serial.println(F("connectant..."));
    Serial.println(Ethernet.localIP());
    Serial.println(F("Definició dels PINS dels RELEs i Final de Carrera"));
  }
  
  // set the digital pins as outputs
  pinMode(RELE_ALIMENTACIO, OUTPUT);           // Pin relé alimentación como salida
  pinMode(RELE_INVERSOR1, OUTPUT);              // Pin relé inversor1 como salida
  pinMode(RELE_INVERSOR2, OUTPUT);              // Pin relé inversor2 como salida
  pinMode(FINAL_CARRERA1, INPUT);               // Pin final carrera1 como entrada

  pinMode(pulsador, INPUT); //Declaramos el interruptor como entrada
  
  digitalWrite(RELE_ALIMENTACIO,HIGH);
  digitalWrite(RELE_INVERSOR1,HIGH);
  digitalWrite(RELE_INVERSOR2,HIGH);

}

void callback(char* topic, byte* payload, unsigned int length) {
  char msg[length];
  if (DEBUG) {
    Serial.print(F("Message arrived ["));
    Serial.print(topic);    
    Serial.print(F("] "));
    
    for (int i=0;i<length;i++) {
      msg[i]=(char)payload[i];
      Serial.print((char)payload[i]);
    }  
    Serial.println();
  }
  int res=strcmp(topic,"sala1/reinici");
  if (res == 0) {    
    if (DEBUG) {
      Serial.print(F("ENTRA a reinici"));
    }            
    estat=6;
    reinici();        
  }
  
  res=strcmp(topic,"sala1/casco1");
  if (res == 0) {    
    if (DEBUG) {
      Serial.print(F("ENTRA a casco1"));
    }            
    estat=1;
    transicio1();        
  }
  
  res=strcmp(topic,"sala1/casco2");
  if (res == 0) {
      if (DEBUG) {
        Serial.print(F("ENTR A a posta2"));
      }
      estat=2;
      transicio2();          
  }
  
  res=strcmp(topic,"sala1/casco3");
  if (res == 0){      
      if (DEBUG) {
        Serial.print(F("ENTRA a posta3"));
      }
      estat=3;
      transicio3();          
  }
  
  res=strcmp(topic,"sala1/casco4");
  if (res ==0) {      
      if (DEBUG) {
        Serial.print(F("ENTRA a posta4"));
      }
      estat=4;
      transicio4();
  }
  
  res=strcmp(topic,"sala1/casco5");
  if (res == 0) {                     
      if (DEBUG) {
        Serial.print(F("ENTRA a posta5"));
      }
      estat=5;
      transicio5();
  }
  
  res=strcmp(topic,"sala1/error");
  if (res == 0) {                     
      if (DEBUG) {
        Serial.print(F("ENTRA a error"));
      }
  }
  
  res=strcmp(topic,"sala1/comensa");
  if (res == 0 ) {
    EEPROM.update(0,true);
    comensa=true;
  }
  
  res=strcmp(topic,"sala1/reset");
  int resu = strcmp(topic,"sala1/resetCascoAlado");  
  if (res == 0 || resu == 0) { //RESET PLACA
      if (DEBUG) {
        Serial.print(F("ENTRA a reset o resetCascoAlado"));
      }
      resetFunc();
     //wdt_enable(WDTO_15MS); // Configuramos el contador de tiempo para que se reinicie en 15ms
  }

  res=strcmp(topic,"sala1/baixaPlataforma");
  if (res == 0) {
     if (DEBUG) {
        Serial.print(F("ENTRA en baixaPlataforma"));
     }
     estat=7;
     baixaDelTot();
  }  

  res=strcmp(topic,"sala1/baixaEscalo");
   if (res == 0) {
      if (DEBUG) {
        Serial.print(F("ENTRA en baixaEscalo"));
      }
      estat=8;      
      baixaUnEscalo();      
  }
  
}

void transicio1(){
  if (DEBUG) {
    Serial.print(F("ENTRA a transicio1"));
  }
  marcaTemps1=millis();  
}

void transicio2(){
  if (DEBUG) {
    Serial.print(F("ENTRA a transicio2"));
  }
  marcaTemps2=millis();  
}

void transicio3(){
  if (DEBUG) {
    Serial.print(F("ENTRA a transicio3"));
  }
  marcaTemps3=millis();  
}

void transicio4(){
  if (DEBUG) {
    Serial.print(F("ENTRA a transicio4"));
  }
  marcaTemps4=millis();  
}

void transicio5(){
  if (DEBUG) {
    Serial.print(F("ENTRA a transicio5"));
  }
  marcaTemps5=millis();  
}


void baixaDelTot(){
  if (DEBUG) {
    Serial.print(F("ENTRA a baixaDelTot"));
  }
  marcaTemps7=millis();  
  entra1=true;
  entra2=true;
  entra3=true;
  entra4=true;
  entra5=true;  
}

void baixaUnEscalo(){
  if (DEBUG) {
    Serial.print(F("ENTRA a baixaUnEscalo"));
  }
  marcaTemps8=millis(); 
}

void reinici(){
  if (DEBUG) {
    Serial.print(F("ENTRA a reinici"));
  }
  marcaTemps6=millis();
  entra1=false;
  entra2=false;
  entra3=false;
  entra4=false;
  entra5=false;
  para=false;
}

void reiniciError(){
  if (DEBUG) {
    Serial.print(F("ENTRA a reiniciError"));
  }
  marcaTemps6=millis();
}

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

void reconnect() {

  //Loop until we're reconnected
  while (!client.connected()) {
    if (DEBUG) {
      Serial.print(F("Intentant connecció MQTT..."));
    }
    
    // Attempt to connect
    if (client.connect("CascoAlado")) {
      if (DEBUG) {
        Serial.println(F("connectat MQTT"));      
      }
      client.subscribe("sala1/reinici");
      client.subscribe("sala1/casco1");
      client.subscribe("sala1/casco2");
      client.subscribe("sala1/casco3");
      client.subscribe("sala1/casco4");
      client.subscribe("sala1/casco5");      
      client.subscribe("sala1/error"); 
      client.subscribe("sala1/reset"); 
      client.subscribe("sala1/resetCascoAlado"); 
      client.subscribe("sala1/comensa");
      client.subscribe("sala1/baixaPlataforma");
      client.subscribe("sala1/baixaEscalo");
      
    } else {
      if (DEBUG) {
        Serial.print(F("FALLA, rc="));
        Serial.print(client.state());
      }
      //Serial.println(" try again in 5 seconds");
      // Wait 3 seconds before retrying
      delay(3000);
    }
  }

}


bool seHaAbierto=false;
bool primeraVegada=false;
bool iniciado=false;
int cont1=0;

void loop() {

  boolean puertaCerrada = digitalRead(puerta);


   if (DEBUG) {  
    if ( cont1 == 0 || (cont1 % 5000 == 0)){
      Serial.print(F("puertaCerrada: ")); 
      Serial.println(puertaCerrada);
      Serial.print("comensa: ");
      Serial.println(comensa);
      Serial.print("seHaAbierto: ");
      Serial.println(seHaAbierto);
      Serial.print("primeraVegada: ");
      Serial.println(primeraVegada);
      Serial.print("iniciado: ");
      Serial.println(iniciado);
      Serial.println("***************");
    }
    cont1++;
  }
 
  if (comensa && !puertaCerrada){
    seHaAbierto=true;      
    if ( cont1 == 0 || (cont1 % 8000 == 0)){
      client.publish("sala1/comensaiportaoberta","on");
    }
  }
  
  if (comensa && puertaCerrada && seHaAbierto && tiempoCerrada==0) {
    Serial.println("ENTRA en tiempoCerrada");
    tiempoCerrada=millis();        
    client.publish("sala1/comensaiportatancada","on");
  }
    
  if (comensa & puertaCerrada && seHaAbierto && !iniciado) {


    //if (DEBUG) {
      //Serial.print("tiempoCerrada: ");
      //Serial.println(tiempoCerrada);
      //Serial.print("millis(): ");
      //unsigned long a=millis();
      //Serial.println(a);
    //}
    
    
     if (tiempoCerrada > 0 && millis() - tiempoCerrada > 5000){
        if (DEBUG) {
            Serial.println("publica sala1/inici");
        }      
        client.publish("sala1/inici","ON");
        iniciado=true;
        comensa=false;
        EEPROM.update(0,false);
     }
  }
  
  boolean estado = analogRead(pulsador); //Leemos el estado del interruptor  
  
  if (!estado) {
    para=true;   
    if ( cont == 0 || (cont % 40000 == 0)){
      client.publish("sala1/finalcarrera","on");    
       if (DEBUG) {
          Serial.print(F("FINAL de CARRERA: "));          
        }
      paraPisto();
    }
    cont++;
  }

  switch(estat){
    case 1:      
      if (!para && !entra1 && millis() - marcaTemps1 < 11000 ) {
        baixaPisto();
      }else{
        paraPisto();
        entra1=true;
      }
      break;
    case 2:
      if (!para && !entra2 && millis() - marcaTemps2 < 11000 ) {
        baixaPisto();
      }else{
        paraPisto();
        entra2=true;
      }
      break;
    case 3:
      if (!para && !entra3 && millis() - marcaTemps3 < 11000 ) {
        baixaPisto();
      }else{
        paraPisto();
        entra3=true;
      }
      break;
    case 4:
      if (!para && !entra4 && millis() - marcaTemps4 < 11000 ) {
        baixaPisto();
      }else{
        paraPisto();
        entra4 = true;
      }
      break;
    case 5:
      if (!para && !entra5 && millis() - marcaTemps5 < 11500 ) {
        baixaPisto();
      }else{
        paraPisto();
        entra5 = true;
      }
      break;
    case 6:
      if (millis() - marcaTemps6 < 60000) {
        pujaPisto();        
      }else{
        paraPisto();        
        estat=0;
      }
      break;
    case 7:
      if (!para && millis() - marcaTemps7 < 60000) {
        baixaPisto();
      }else{
        paraPisto();
        estat=9;
      }
    case 8:
      if (!para && millis() - marcaTemps8 < 11200) {
        baixaPisto();
      }else{
        paraPisto();
        if (estat < 6) estat++;        
      }
      
  }
  
  if (!client.connected()) {
      reconnect();
  }
  client.loop();

}
